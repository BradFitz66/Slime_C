#include "Stretch.h"
#include "../Player.h"
#include "../../Engine/StateMachine.h"
#include "../../Engine/Input.h"
#include <math.h>
float initialWidth = 1.43f;
float initialHeight = 0.54f;
float curXScale = 0;

float rotatePivotX = 0;
float rotatePivotY = 0;

void Stretch_Enter(void* data)
{
    Entity* entity = (Entity*)data;

    TransitionToAnimation(entity, PLAYER_STATE_STRETCH, false, false);

    curXScale = 0;
    entity->xScale = initialWidth;
    entity->renderShadow = false;
    entity->yScale = initialHeight;
    entity->rotation = entity->direction * 45.0f;

    // Compute world-relative pivot from the squish frame center for the current direction. 
    int squishLastIdx = entity->sprite->animations[PLAYER_STATE_SQUISH + entity->direction].frameCount - 1;
    Quad* squishQuad = entity->sprite->animations[PLAYER_STATE_SQUISH + entity->direction].frames[squishLastIdx].frameQuad;
    rotatePivotX = squishQuad->width / 2.0f - (float)squishQuad->offsetX;
    rotatePivotY = squishQuad->height / 2.0f - (float)squishQuad->offsetY;

    Animation* stretchAnim = &entity->sprite->animations[entity->animIdx];
    Frame* initFrame = &stretchAnim->frames[stretchAnim->currentFrame];
    entity->rotationPivotX = rotatePivotX + (float)initFrame->frameQuad->offsetX * entity->xScale;
    entity->rotationPivotY = rotatePivotY + (float)initFrame->frameQuad->offsetY * entity->yScale;
}

float Lerp_F(float a, float b, float t)
{
    return a + (b - a) * t;
}

void Stretch_Update(void* data)
{
    Entity* entity = (Entity*)data;
    if (!entity) return;
    entity->timer++;
    if(!INPUT_GetButton("Space")){
        STATEMACHINE_ChangeState(entity->stateMachine, 6, entity); 
        return;
    }
    float rawH = INPUT_GetAxis("Horizontal");
    float rawV = INPUT_GetAxis("Vertical");
    int direction = GetDirectionIndex(rawH, rawV);
    if (direction != entity->direction && direction != -1)
    {
        entity->direction = direction;
        entity->rotation = direction * 45.0f;
        TransitionToAnimation(entity, PLAYER_STATE_STRETCH, false, true);

        // Recompute world-relative pivot for the new direction's squish frame
        int squishLastIdx = entity->sprite->animations[PLAYER_STATE_SQUISH + entity->direction].frameCount - 1;
        Quad* squishQuad = entity->sprite->animations[PLAYER_STATE_SQUISH + entity->direction].frames[squishLastIdx].frameQuad;
        rotatePivotX = squishQuad->width / 2.0f - (float)squishQuad->offsetX;
        rotatePivotY = squishQuad->height / 2.0f - (float)squishQuad->offsetY;
    }
    float inputMagnitude = sqrtf(rawH * rawH + rawV * rawV);
    if (inputMagnitude > 0.0f) {
        
        // Advance t by 1/40 per frame so the stretch completes in exactly 40 frames.
        curXScale += 1.0f / 40.0f;
        if (curXScale > 1.0f) curXScale = 1.0f;
        entity->xScale = Lerp_F(initialWidth, 1.0f, curXScale);
        entity->yScale = Lerp_F(initialHeight, 1.0f, curXScale);
    }
    else
    {
        curXScale -= 1.0f / 40.0f;
        if (curXScale < 0.0f) curXScale = 0.0f;
        entity->xScale = Lerp_F(initialWidth, 1.0f, curXScale);
        entity->yScale = Lerp_F(initialHeight, 1.0f, curXScale);
    }
    if (curXScale < 0.01f && inputMagnitude == 0.0f)
    {
        STATEMACHINE_ChangeState(entity->stateMachine, 2, entity); 
        return;
    }

    // Keep pivot at the same world-space position as scale changes.
    Animation* currAnim = &entity->sprite->animations[entity->animIdx];
    Frame* currFrame = &currAnim->frames[currAnim->currentFrame];
    entity->rotationPivotX = rotatePivotX + (float)currFrame->frameQuad->offsetX * entity->xScale;
    entity->rotationPivotY = rotatePivotY + (float)currFrame->frameQuad->offsetY * entity->yScale;

    //TempVal1 should go from 128 to 256 based on scale

    entity->tempVal1 = Lerp_F(128.0f, 256.0f, curXScale);
}

void Stretch_Exit(void* data)
{
    Entity* entity = (Entity*)data;
    curXScale = 0;
    entity->rotationPivotX = 0;
    entity->rotationPivotY = 0;
    entity->timer = 0;
    entity->renderShadow = true;
    if (!entity) return;
}