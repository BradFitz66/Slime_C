#include "Blasting.h"
#include "../Player.h"
#include "../../Engine/StateMachine.h"
#include "../../Engine/Input.h"
#include <stdio.h>
#include <math.h>
#include <limits.h>
void Blasting_Enter(void* data)
{
    Entity* entity = (Entity*)data;
    //blasting uses same animation as jumping
    TransitionToAnimation(entity, PLAYER_STATE_JUMP + entity->direction, false, false);
    //Get magnitude of scale to determine stretch amount
    


    entity->timer = ((int)entity->tempVal1 << 2) - 64;
    //Get the rotation and use it to determine the velocity direction
    switch (entity->direction)
    {    
        case 0: // Up
            entity->yVel = -7.0f;
            break;
        case 1: // DiagTR
            entity->yVel = -7.0f * 0.7071f;
            entity->xVel = 7.0f * 0.7071f;
            break;
        case 2: // R
            entity->xVel = 7.0f;
            break;
        case 3: // DiagDR
            entity->yVel = 7.0f * 0.7071f;
            entity->xVel = 7.0f * 0.7071f;
            break;
        case 4: // Down
            entity->yVel = 7.0f;
            break;
        case 5: // DiagDL
            entity->yVel = 7.0f * 0.7071f;
            entity->xVel = -7.0f * 0.7071f;
            break;
        case 6: // L
            entity->xVel = -7.0f;
            break;
        case 7: // DiagTL
            entity->yVel = -7.0f * 0.7071f;
            entity->xVel = -7.0f * 0.7071f;
            break;
    }
    entity->rotation = 0;
    entity->xScale = 1.0f;
    entity->yScale = 1.0f;
}

bool approxEquals(float value, float other, float epsilon) {
    return fabsf(value - other) < epsilon;
}

unsigned int tsub_ok(unsigned int x, unsigned int y) {
    
    if ((y < 0) ? (x > UINT_MAX + y) : (x < y)) {
        printf("OVERFLOW\n");
        return 0;    
    } 
    return 1;    
}

int result = 0; // Global variable to hold the result of subtraction
void Blasting_Update(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;
    if (entity->timer == 0 )
    {
        entity->xVel = entity->xVel * 0.5f; // Slow down after blasting
        entity->yVel = entity->yVel * 0.5f;
    }

    if (tsub_ok(entity->timer, 64)) {
        entity->timer -= 64; 
    } else {
        entity->timer = 0; // Handle overflow case
    }

    float xVel = fabsf(entity->xVel);
    float yVel = fabsf(entity->yVel);

    if (approxEquals(xVel, 0.0f, 0.25f) && approxEquals(yVel, 0.0f, 0.25f)) {
        float rawH = INPUT_GetAxis("Horizontal");
        float rawV = INPUT_GetAxis("Vertical");
        if (rawH != 0.0f || rawV != 0.0f)
        {
            STATEMACHINE_ChangeState(entity->stateMachine, 1, entity); // Switch to walk state
            TransitionToAnimation(entity, PLAYER_STATE_WALK + entity->direction, false, false);
        }else{
            STATEMACHINE_ChangeState(entity->stateMachine, 0, entity); // Switch to idle state
            TransitionToAnimation(entity, PLAYER_STATE_IDLE + entity->direction, false, false);
        }
        return;
    }

    entity->x += entity->xVel;
    entity->y += entity->yVel;
}

void Blasting_Exit(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;
}