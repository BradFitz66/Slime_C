#pragma once
#include "../Engine/Entity.h"
#include <stdbool.h>

Entity* PLAYER_Create(SDL_Renderer* renderer, int x, int y);
void TransitionToAnimation(Entity* entity, int animIdx, bool waitForFinish, bool preserveFrame);
void PLAYER_update(Entity* entity, void* state);
void PLAYER_render(Entity* entity, SDL_Renderer* renderer, struct nk_context* ctx);
int GetDirectionIndex(float h, float v);
//Defines the offset from 0 that the animations for each state start at. For example, idle animations are 0-7, so the offset is 0. Walk animations are 8-15, so the offset is 8.
//Directional animations are stored in clockwise order, so it's trivial to get the correct animation based on their direction.
enum PlayerAnimationState{
    PLAYER_STATE_IDLE = 0,
    PLAYER_STATE_WALK = 8,
    PLAYER_STATE_SQUISH = 16,
    PLAYER_STATE_JUMP = 24,
    PLAYER_STATE_FLOAT = 32,
    PLAYER_STATE_STRETCH = 40,
    PLAYER_STATE_CHARGED = 44,
    PLAYER_STATE_WALLHIT = 48,
    PLAYER_STATE_CHARGEDBLAST = 56
};
