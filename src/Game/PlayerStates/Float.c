#include "Float.h"
#include "../Player.h"
#include "../../Engine/StateMachine.h"
#include "../../Engine/Input.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif
void Float_Enter(void* data)
{
    Entity* entity = (Entity*)data;
    TransitionToAnimation(entity, PLAYER_STATE_FLOAT + entity->direction, false, false);
    entity->zVel = 0.625f; // 0xa0 raw NDS fx32 → (160/4096)*16 px/frame
}

void Float_Update(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;




    entity->z += entity->zVel;

    entity->timer = (entity->timer + 4) & 0xFF;

    //The NDS game uses a lookup table for the oscillation while hovering, we can just use math functions.
    float phase = ((entity->timer+40) / 256.0f) * 2.0f * M_PI;
    entity->z += cosf(phase);
    entity->zVel -= 0.02f;

    if (entity->z < 1){
        entity->z = 0;
        entity->zVel = 0;
        STATEMACHINE_ChangeState(entity->stateMachine, 0, entity); // Switch to idle state
        return;
    }
    
    if (!INPUT_GetButton("Space") && entity->z > 1){
        entity->subState = 1;
        STATEMACHINE_ChangeState(entity->stateMachine, 3, entity); 
        return;
    }

    float rawH = INPUT_GetAxis("Horizontal");
    float rawV = INPUT_GetAxis("Vertical");
    int direction = GetDirectionIndex(rawH, rawV);
    if (direction != entity->direction && direction != -1 && entity->z > 0)
    {
        entity->direction = direction;
        TransitionToAnimation(entity, PLAYER_STATE_FLOAT + entity->direction, false, true);
    }

    entity->x += entity->xVel;
    entity->y += entity->yVel;
}

void Float_Exit(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;
}