#include "Walk.h"
#include "../Player.h"
#include "../../Engine/StateMachine.h"
#include "../../Engine/Input.h"

void Walk_Enter(void* data)
{
    Entity* entity = (Entity*)data;
    TransitionToAnimation(entity, PLAYER_STATE_WALK + entity->direction, false, false);
}

void Walk_Update(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;

    float rawH = INPUT_GetAxis("Horizontal");
    float rawV = INPUT_GetAxis("Vertical");
    int direction = GetDirectionIndex(rawH, rawV);
    if (direction != entity->direction && direction != -1)
    {
        entity->direction = direction;
        TransitionToAnimation(entity, PLAYER_STATE_WALK + entity->direction, false, true);
    }
    if (rawH == 0.0f && rawV == 0.0f)
    {
        // Wait for the current walk cycle to finish, then switch to idle
        Animation* anim = &entity->sprite->animations[entity->animIdx];
        if (anim->currentFrame >= anim->frameCount - 1)
        {
            STATEMACHINE_ChangeState(entity->stateMachine, 0, entity);
        }
        return;
    }
    if (INPUT_GetButton("Space"))
    {
        STATEMACHINE_ChangeState(entity->stateMachine, 2, entity); // Switch to squish state
        return;
    }
    
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

    entity->x += entity->xVel;
    entity->y += entity->yVel;
}

void Walk_Exit(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;
    entity->xVel = 0.0f;
    entity->yVel = 0.0f;
}