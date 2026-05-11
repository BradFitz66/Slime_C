#include "Jump.h"
#include "../Player.h"
#include "../../Engine/StateMachine.h"
#include "../../Engine/Input.h"

void Jump_Enter(void* data)
{
    Entity* entity = (Entity*)data;
    TransitionToAnimation(entity, PLAYER_STATE_JUMP + entity->direction, false, false);
}


void Jump_Update(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;

    entity->z += entity->zVel;
    entity->zVel -= 0.15;


    float rawH = INPUT_GetAxis("Horizontal");
    float rawV = INPUT_GetAxis("Vertical");
    int direction = GetDirectionIndex(rawH, rawV);
    if (direction != entity->direction && direction != -1 && entity->z > 0)
    {
        entity->direction = direction;
        TransitionToAnimation(entity, PLAYER_STATE_JUMP + entity->direction, false, true);
    }

    if (entity->z <= 0){
        entity->z = 0;
        entity->zVel = 0;
        entity->subState = 0;
        //Transition to walk/idle animations based on input
        if (rawH != 0.0f || rawV != 0.0f)
        {
            //TransitionToAnimation(entity, PLAYER_STATE_WALK + entity->direction, false, true);
            STATEMACHINE_ChangeState(entity->stateMachine, 1, entity); // Switch to walk state
            return;
        }
        TransitionToAnimation(entity, PLAYER_STATE_IDLE + entity->direction, false, false);
        STATEMACHINE_ChangeState(entity->stateMachine, 0, entity); // Switch back to idle state when landing
        return;
    }
    else if (entity->z > 16.0f) 
    {
        if (INPUT_GetButtonDown("Space") && entity->subState == 0){
            STATEMACHINE_ChangeState(entity->stateMachine, 4, entity); // Switch to float state when in air
            return;
        }
    }


    entity->x += entity->xVel;
    entity->y += entity->yVel;
}

void Jump_Exit(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;
}