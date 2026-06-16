#pragma once

#include "Texture.h"
#include "StateMachine.h"
#include "AnimatedSprite.h"

//Forward declaration of nk_context to avoid including nuklear.h in this header
struct nk_context;

typedef struct Entity {
    float x;
    float y;
    float z; //Used to offset player sprite when rendering to give a sense of depth 
    
    float xVel;
    float yVel;
    float zVel;

    float xScale;
    float yScale;

    float rotationPivotX; // The point around which the entity rotates, in world coordinates. If 0, it defaults to the current animation frame's offset values
    float rotationPivotY;
    float rotation;

    unsigned int animIdx;
    unsigned int direction; 
    unsigned int animationState; //Basically an offset for which set of animations to use
    unsigned int animTimer;
    unsigned int timer; // Generic timer for any purpose
    unsigned int subState; // For more complex states 

    float tempVal1; //generic float value for any purpose

    Texture* shadowTexture;
    AnimatedSprite* sprite;
    StateMachine* stateMachine;

    bool renderShadow;

    //Function pointer for updating the entity each frame
    void (*update)(struct Entity* entity, void* state);
    //Function pointer for rendering the entity each frame
    void (*render)(struct Entity* entity, SDL_Renderer* renderer, struct nk_context* ctx);

} Entity;