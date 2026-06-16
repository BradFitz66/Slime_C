#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include "SDL3/SDL_events.h"

#include <stdio.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_INCLUDE_COMMAND_USERDATA
#include <nuklear.h>
#define NK_SDL3_RENDERER_IMPLEMENTATION
#include <nuklear_sdl3_renderer.h>
#include "Engine/Entity.h"
#include "Engine/Tilemap.h"
#include "Engine/Input.h"
#include "Engine/DrawStats.h"
#include "Game/Player.h"


#define WINDOW_TITLE  "Slime"
#define WINDOW_WIDTH  256*4
#define WINDOW_HEIGHT 192*4

typedef struct {
    Entity** entities;
    int entityCount;
    
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Event event;

    Tilemap* tilemap; 

    int paused;
    int frameStep;

    int TARGET_FPS;
    int MS_PER_FRAME;
    float fps;
    Uint64 lastFrameTime;
    float  frameTimeMs;
    Uint64 fpsLastTime;
    int    fpsFrameCount;

    struct nk_context *nkContext;
} GameState;

SDL_AppResult SDL_Fail(){
    SDL_LogError(SDL_LOG_CATEGORY_CUSTOM, "Error %s", SDL_GetError());
    return SDL_APP_FAILURE;
}

SDL_AppResult SDL_AppInit(void** AppState, int argc, char** argv) {
	SDL_Init(SDL_INIT_VIDEO);

	GameState* gameState = malloc(sizeof(GameState));
	if (!gameState) {
		fprintf(stderr, "Failed to allocate memory for GameState\n");
		SDL_Quit();
		exit(1);
	}

	gameState->TARGET_FPS     = 60;
	gameState->MS_PER_FRAME   = 1000 / gameState->TARGET_FPS;
    gameState->lastFrameTime  = SDL_GetPerformanceCounter();
    gameState->paused         = 0;
    gameState->frameStep      = 0;
    gameState->frameTimeMs    = 0.0f;
    gameState->fpsLastTime    = SDL_GetTicks();
    gameState->fpsFrameCount  = 0;

	INPUT_AddButton("Space", SDLK_SPACE);
	INPUT_AddButton("Quit", SDLK_ESCAPE);
	INPUT_AddButton("Pause", SDLK_P);
	INPUT_AddButton("FrameStep", SDLK_N);

	INPUT_AddAxis("Horizontal", SDLK_D, SDLK_A);
	INPUT_AddAxis("Vertical", SDLK_S, SDLK_W);

	SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE,WINDOW_WIDTH, WINDOW_HEIGHT, 0);

	if (!window) {
		return SDL_Fail();
	}
	gameState->window = window;

	SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		return SDL_Fail();
	}

	gameState->renderer = renderer;
    gameState->nkContext = nk_sdl_init(gameState->window, gameState->renderer, nk_sdl_allocator());
	{
		struct nk_font_atlas *atlas;
        struct nk_font_config config = nk_font_config(0);
        struct nk_font *font;

        atlas = nk_sdl_font_stash_begin(gameState->nkContext);
        font = nk_font_atlas_add_default(atlas, 13, &config);
        nk_sdl_font_stash_end(gameState->nkContext);

        nk_style_set_font(gameState->nkContext, &font->handle);
	}

	Entity* player = PLAYER_Create(gameState->renderer, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	if (!player) {
		fprintf(stderr, "Failed to create player entity\n");
		return 1;
	}

    Tilemap* test = TILEMAP_LoadFromFile("Assets/GrassMap.json", gameState->renderer);
    if (!test) {
        fprintf(stderr, "Failed to load tilemap\n");
        return 1;
    }

	gameState->entityCount = 1;
	gameState->entities = malloc(sizeof(Entity*) * gameState->entityCount);
	gameState->entities[0] = player;

	*AppState = gameState;

    gameState->tilemap = test;

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* AppState, SDL_Event* Event) {
  
    GameState* gameState = (GameState*)AppState;
    // nk_input_begin(gameState->nkContext);
	if (Event->type == SDL_EVENT_QUIT) {
		exit(0);
	}
    SDL_ConvertEventToRenderCoordinates(gameState->renderer, Event);
    nk_sdl_handle_event(gameState->nkContext, Event);

	return SDL_APP_CONTINUE;
}

static void DrawSDLStatsWindow(GameState* gameState) {
    struct nk_context* ctx = gameState->nkContext;
    if (!nk_begin(ctx, "SDL Stats", nk_rect(10, 10, 210, 158),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
    {
        nk_end(ctx);
        return;
    }

    char buf[80];
    int outW = 0, outH = 0;
    SDL_GetRenderOutputSize(gameState->renderer, &outW, &outH);
    const char* backend = SDL_GetRendererName(gameState->renderer);

    nk_layout_row_dynamic(ctx, 16, 1);
    snprintf(buf, sizeof(buf), "FPS:       %.1f", gameState->fps);
    nk_label(ctx, buf, NK_TEXT_LEFT);
    snprintf(buf, sizeof(buf), "Frame:     %.2f ms", gameState->frameTimeMs);
    nk_label(ctx, buf, NK_TEXT_LEFT);
    snprintf(buf, sizeof(buf), "Backend:   %s", backend ? backend : "?");
    nk_label(ctx, buf, NK_TEXT_LEFT);
    snprintf(buf, sizeof(buf), "Viewport:  %dx%d", outW, outH);
    nk_label(ctx, buf, NK_TEXT_LEFT);
    snprintf(buf, sizeof(buf), "Entities:  %d", gameState->entityCount);
    nk_label(ctx, buf, NK_TEXT_LEFT);
    snprintf(buf, sizeof(buf), "Draw calls: %d", g_drawCallCount);
    nk_label(ctx, buf, NK_TEXT_LEFT);

    nk_end(ctx);
}

SDL_AppResult SDL_AppIterate(void* AppState) {
    GameState* gameState = (GameState*)AppState;

    Uint64 freq    = SDL_GetPerformanceFrequency();
    Uint64 now     = SDL_GetPerformanceCounter();
    Uint64 elapsed = now - gameState->lastFrameTime;
    if (elapsed < freq / (Uint64)gameState->TARGET_FPS) {
        return SDL_APP_CONTINUE;
    }
    gameState->lastFrameTime = now;
    gameState->frameTimeMs   = (float)elapsed * 1000.0f / (float)freq;
    gameState->fpsFrameCount++;
    Uint64 nowMs = SDL_GetTicks();
    if (nowMs - gameState->fpsLastTime >= 500) {
        gameState->fps           = gameState->fpsFrameCount * 1000.0f / (float)(nowMs - gameState->fpsLastTime);
        gameState->fpsFrameCount  = 0;
        gameState->fpsLastTime    = nowMs;
    }
    nk_input_end(gameState->nkContext);
    INPUT_Poll();

    if (INPUT_GetButtonDown("Pause")) {
        gameState->paused = !gameState->paused;
    }

    int shouldStep = !gameState->paused || INPUT_GetButtonDown("FrameStep");

    INPUT_Flush();

    g_drawCallCount = 0;
    SDL_SetRenderDrawColor(gameState->renderer, 100, 149, 237, 255);
    SDL_RenderClear(gameState->renderer);
    TILEMAP_Render(gameState->tilemap, gameState->renderer, 0, 0);
    for (int i = 0; i < gameState->entityCount; i++) {
        if (shouldStep) {
            gameState->entities[i]->update(gameState->entities[i], AppState);
        }
        gameState->entities[i]->render(gameState->entities[i], gameState->renderer, gameState->nkContext);
    }
    
    DrawSDLStatsWindow(gameState);
    nk_sdl_render(gameState->nkContext, NK_ANTI_ALIASING_ON);
    nk_sdl_update_TextInput(gameState->nkContext);
    SDL_RenderPresent(gameState->renderer);
    nk_input_begin(gameState->nkContext);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* AppState, SDL_AppResult Result) {
    GameState* gameState = (GameState*)AppState;
    for (int i = 0; i < gameState->entityCount; i++) {
        free(gameState->entities[i]);
    }
    free(gameState->entities);
    SDL_DestroyRenderer(gameState->renderer);
    SDL_DestroyWindow(gameState->window);

	//Wait until user presses a key before quitting to allow them to see any final messages
	printf("Press Enter to quit...");
	getchar();
	SDL_Quit();
}

