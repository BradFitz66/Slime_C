#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "SDL3/SDL_events.h"

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
#include "Engine/Input.h"
#include <stdio.h>
#include "Game/Player.h"

#define WINDOW_TITLE  "Slime"
#define WINDOW_WIDTH  1024
#define WINDOW_HEIGHT 384*2

typedef struct {
    Entity** entities;
    int entityCount;
    SDL_Renderer* renderer;
    SDL_Window* window;
    SDL_Event event;
    int TARGET_FPS;
    int MS_PER_FRAME;
    float FPS;
    int paused;
    int frameStep;
    Uint64 lastFrameTime;
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
	gameState->TARGET_FPS = 60;
	gameState->MS_PER_FRAME = 1000 / gameState->TARGET_FPS;
    gameState->lastFrameTime = SDL_GetTicks();
    gameState->paused = 0;
    gameState->frameStep = 0;
	INPUT_AddButton("Space", SDLK_SPACE);

	INPUT_AddButton("Quit", SDLK_ESCAPE);

	INPUT_AddAxis("Horizontal", SDLK_D, SDLK_A);
	INPUT_AddAxis("Vertical", SDLK_S, SDLK_W);

	INPUT_AddButton("Pause", SDLK_P);
	INPUT_AddButton("FrameStep", SDLK_N);

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

	gameState->entityCount = 1;
	gameState->entities = malloc(sizeof(Entity*) * gameState->entityCount);
	gameState->entities[0] = player;

	*AppState = gameState;
    nk_input_begin(gameState->nkContext);
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

SDL_AppResult SDL_AppIterate(void* AppState) {
    GameState* gameState = (GameState*)AppState;

    Uint64 now = SDL_GetTicks();
    Uint64 elapsed = now - gameState->lastFrameTime;
    if (elapsed < (Uint64)gameState->MS_PER_FRAME) {
        return SDL_APP_CONTINUE;
    }
    gameState->lastFrameTime = now;
    nk_input_end(gameState->nkContext);
    INPUT_Poll();

    if (INPUT_GetButtonDown("Pause")) {
        gameState->paused = !gameState->paused;
    }

    int shouldStep = !gameState->paused || INPUT_GetButtonDown("FrameStep");

    INPUT_Flush();

    SDL_SetRenderDrawColor(gameState->renderer, 100, 149, 237, 255);
    SDL_RenderClear(gameState->renderer);

    for (int i = 0; i < gameState->entityCount; i++) {
        if (shouldStep) {
            gameState->entities[i]->update(gameState->entities[i], AppState);
        }
        gameState->entities[i]->render(gameState->entities[i], gameState->renderer, gameState->nkContext);
    }
    nk_sdl_render(gameState->nkContext, NK_ANTI_ALIASING_ON);
    nk_sdl_update_TextInput(gameState->nkContext);
    SDL_RenderPresent(gameState->renderer);

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

