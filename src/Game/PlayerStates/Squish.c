#include "Squish.h"
#include "../Player.h"
#include "../../Engine/StateMachine.h"
#include "../../Engine/Input.h"
void Squish_Enter(void* data)
{
    Entity* entity = (Entity*)data;
    TransitionToAnimation(entity, PLAYER_STATE_SQUISH + entity->direction, false, false);
}

void Squish_Update(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;

    

    if(!INPUT_GetButton("Space")){
        entity->zVel = 2.96f;
        STATEMACHINE_ChangeState(entity->stateMachine, 3, entity); 
        return;
    }
    float rawH = INPUT_GetAxis("Horizontal");
    float rawV = INPUT_GetAxis("Vertical");
    if (rawH != 0.0f || rawV != 0.0f)
    {
        entity->direction = GetDirectionIndex(rawH, rawV);
        TransitionToAnimation(entity, PLAYER_STATE_SQUISH + entity->direction, false, true);

    }
}

void Squish_Exit(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;
}
