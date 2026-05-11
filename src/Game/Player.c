#include "Player.h"
#include "../Engine/TextureSheet.h"
#include "../Engine/Input.h"
#include "../Engine/StateMachine.h"
#include "PlayerStates/PlayerStates.h"

#include <stdio.h>
#include <math.h>
#include <nuklear.h>
// Animation order: Up=0, DiagTR=1, R=2, DiagDR=3, Down=4, DiagDL=5, L=6, DiagTL=7
int GetDirectionIndex(float h, float v)
{
    int hSign = (h > 0.1f) ? 1 : (h < -0.1f) ? -1
                                             : 0;
    int vSign = (v > 0.1f) ? 1 : (v < -0.1f) ? -1
                                             : 0;

    if (hSign == 0 && vSign == -1)
        return 0; // Up
    if (hSign == 1 && vSign == -1)
        return 1; // DiagTR
    if (hSign == 1 && vSign == 0)
        return 2; // R
    if (hSign == 1 && vSign == 1)
        return 3; // DiagDR
    if (hSign == 0 && vSign == 1)
        return 4; // Down
    if (hSign == -1 && vSign == 1)
        return 5; // DiagDL
    if (hSign == -1 && vSign == 0)
        return 6; // L
    if (hSign == -1 && vSign == -1)
        return 7; // DiagTL
    return -1; // No movement
}

Entity *PLAYER_Create(SDL_Renderer *renderer, int x, int y)
{
    Entity *player = malloc(sizeof(Entity));
    if (!player)
    {
        fprintf(stderr, "Failed to allocate memory for player entity\n");
        return NULL;
    }

    player->x = x;
    player->y = y;
    player->animIdx = 0;
    player->direction = 0;
    player->z = 0;
    player->zVel = 0;
    player->animationState = 0;
    player->animTimer = 0;
    player->timer = 0;
    player->subState = 0;

    player->textureSheet = TEXTURESHEET_ParseFromQuadTasticJSON("Assets/SlimeAnim.json", renderer);
    player->shadowTexture = TEXTURE_LoadFromFile("Assets/Shadow.png", renderer);
    player->stateMachine = STATEMACHINE_Create(player); // Placeholder state machine with 3 states and no initial data
    
    STATEMACHINE_AddState(player->stateMachine, (State){Idle_Enter, Idle_Update, Idle_Exit});
    STATEMACHINE_AddState(player->stateMachine, (State){Walk_Enter, Walk_Update, Walk_Exit});
    STATEMACHINE_AddState(player->stateMachine, (State){Squish_Enter, Squish_Update, Squish_Exit});
    STATEMACHINE_AddState(player->stateMachine, (State){Jump_Enter, Jump_Update, Jump_Exit});
    STATEMACHINE_AddState(player->stateMachine, (State){Float_Enter, Float_Update, Float_Exit});

    STATEMACHINE_ChangeState(player->stateMachine, 0, player); // Start in idle state

    if (!player->textureSheet)
    {
        fprintf(stderr, "Failed to load texture sheet for player entity\n");
        free(player);
        return NULL;
    }

    player->update = PLAYER_update;
    player->render = PLAYER_render;

    return player;
}

void TransitionToAnimation(Entity *entity, int animIdx, bool waitForFinish, bool preserveFrame)
{
    if (!entity || !entity->textureSheet)
        return;

    if (animIdx < 0 || animIdx >= entity->textureSheet->animationCount)
    {
        fprintf(stderr, "Invalid animation index: %d\n", animIdx);
        return;
    }

    if (waitForFinish)
    {
        // Are we at the final frame of the current animation?
        Animation *currentAnim = &entity->textureSheet->animations[entity->animIdx];
        if (currentAnim->currentFrame < currentAnim->frameCount - 1)
        {
            return;
        }
    }

    Animation *prevAnim = &entity->textureSheet->animations[entity->animIdx];
    float elapsedTime = prevAnim->elapsedTime;
    int prevFrame = prevAnim->currentFrame;


    Animation *anim = &entity->textureSheet->animations[animIdx];
    if (preserveFrame)
    {
        int frameCount = anim->frameCount;
        anim->currentFrame = prevFrame < frameCount ? prevFrame : 0;
        anim->elapsedTime = elapsedTime;
    }
    else
    {
        anim->elapsedTime = 0;
        anim->currentFrame = 0;
    }
    entity->animIdx = animIdx;
    entity->textureSheet->currentAnimationIdx = animIdx;
}


void PLAYER_update(Entity *entity, void *state)
{
    if (!entity)
        return;

    Animation *anim = &entity->textureSheet->animations[entity->animIdx];
    entity->animTimer += 1;
    int frameDuration = (int)(anim->frames[anim->currentFrame].frameDuration / 16.0f); // Convert ms to frames at ~60fps
    if (entity->animTimer >= frameDuration)
    {
        entity->animTimer = 0;
        anim->currentFrame++;
        if (anim->currentFrame >= anim->frameCount)
        {
            if (anim->loop)
            {
                anim->currentFrame = 0;
            }
            else
            {
                anim->currentFrame = anim->frameCount - 1; // Stay on last frame if not looping
            }
        }
    }
 
    float rawH = INPUT_GetAxis("Horizontal");
    float rawV = INPUT_GetAxis("Vertical");

    entity->xVel = rawH * 1.5f; 
    entity->yVel = rawV * 1.5f;

    //Normalize diagonal movement
    if (rawH != 0.0f && rawV != 0.0f)
    {
        entity->xVel *= 0.7071f; 
        entity->yVel *= 0.7071f;
    }

    // Animation direction switching
    int moving = (rawH != 0.0f || rawV != 0.0f);
    int spaceDown = INPUT_GetButton("Space");
    int targetAnim = 0;
    
    STATEMACHINE_Update(entity->stateMachine, entity);
}

void PLAYER_render(Entity *entity, SDL_Renderer *renderer, struct nk_context* ctx)
{
    if (!entity || !renderer || !ctx)
        return;

    TextureSheet *sheet = entity->textureSheet;
    if (!sheet)
        return;

    if (nk_begin(ctx, "Rocket", nk_rect(50, 50, 230, 250),
        NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
        NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
    {
        //Show entity values for debugging
        nk_layout_row_dynamic(ctx, 20, 1);

        nk_label(ctx, "Player Debug Info:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 20, 1);  

        nk_label(ctx, "Direction:", NK_TEXT_LEFT);
        const char* directions[] = {"Up", "DiagTR", "Right", "DiagDR", "Down", "DiagDL", "Left", "DiagTL"};
        nk_label(ctx, directions[entity->direction], NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 20, 1);
        
        nk_label(ctx, "Current Animation:", NK_TEXT_LEFT);
        char animStr[64];
        snprintf(animStr, sizeof(animStr), "AnimIdx: %d, Frame: %d/%d", entity->animIdx, sheet->animations[entity->animIdx].currentFrame + 1, sheet->animations[entity->animIdx].frameCount);
        nk_label(ctx, animStr, NK_TEXT_LEFT);

        //Velocity
        nk_label(ctx, "Velocity:", NK_TEXT_LEFT);
        char velStr[64];
        snprintf(velStr, sizeof(velStr), "X: %.2f, Y: %.2f, Z: %.2f", entity->xVel, entity->yVel, entity->zVel);
        nk_label(ctx, velStr, NK_TEXT_LEFT);

        //Position
        nk_label(ctx, "Position:", NK_TEXT_LEFT);
        char posStr[64];
        snprintf(posStr, sizeof(posStr), "X: %.2f, Y: %.2f, Z: %.2f", entity->x, entity->y, entity->z);
        nk_label(ctx, posStr, NK_TEXT_LEFT);

        //Timer
        nk_label(ctx, "Timer:", NK_TEXT_LEFT);
        char timerStr[64];
        snprintf(timerStr, sizeof(timerStr), "Timer: %d", entity->timer);
        nk_label(ctx, timerStr, NK_TEXT_LEFT);
    }
    nk_end(ctx);

  
    //Render the shadow with transparent color
    SDL_SetTextureColorMod(entity->shadowTexture->texture, 0, 0, 0);
    SDL_SetTextureAlphaMod(entity->shadowTexture->texture, 128); // Set alpha to 50%

    float shadow_radius = 16.0f - (entity->z/16.0f); // Max radius of 16 when z=0, shrinking to 0 as z approaches 256
    if (shadow_radius < 0.0f) shadow_radius = 0.0f;
    float shadow_scale = shadow_radius / 16.0f; // Scale factor for shadow texture based on radius

    // Shadow center: horizontally centred on player, 7px above feet
    TEXTURE_RenderCentered(entity->shadowTexture, renderer, entity->x, entity->y - 7, shadow_scale);
    
    TEXTURESHEET_Render(sheet, renderer, entity->x, entity->y - (int)entity->z);
}