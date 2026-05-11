#include "Idle.h"
#include "../Player.h"
#include "../../Engine/StateMachine.h"
#include "../../Engine/Input.h"
#include <stdio.h>
void Idle_Enter(void* data)
{
    Entity* entity = (Entity*)data;
}

void Idle_Update(void* data)
{

    Entity* entity = (Entity*)data;
    if (!entity) return;

    float rawH = INPUT_GetAxis("Horizontal");
    float rawV = INPUT_GetAxis("Vertical");
    if (rawH != 0.0f || rawV != 0.0f)
    {
        STATEMACHINE_ChangeState(entity->stateMachine, 1, entity); // Switch to walk state
        return;
    }
    if (entity->animIdx != PLAYER_STATE_IDLE + entity->direction)
        TransitionToAnimation(entity, PLAYER_STATE_IDLE + entity->direction, true, false);

    if (INPUT_GetButton("Space"))
    {
        STATEMACHINE_ChangeState(entity->stateMachine, 2, entity); // Switch to squish state
        return;
    }
}

void Idle_Exit(void* data)
{
}