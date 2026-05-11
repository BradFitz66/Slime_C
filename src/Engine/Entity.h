#pragma once

#include "Fixed.h"
#include "Texture.h"
#include "TextureSheet.h"
#include "StateMachine.h"

//Forward declaration of nk_context to avoid including nuklear.h in this header
struct nk_context;

typedef struct Entity {
    //Position
    float x;
    float y;
    float z; //Used to offset player sprite when rendering to give a sense of depth (20.12 fixed-point)
    
    //Velocity
    float xVel;
    float yVel;
    float zVel;

    int animIdx;
    int direction; 
    int animationState; //Basically an offset for which set of animations to use
    int animTimer;
    int timer; // Generic timer for any purpose
    int subState; // For more complex states that require multiple steps, like attack animations

    Texture* shadowTexture;
    TextureSheet* textureSheet;
    StateMachine* stateMachine;

    //Function pointer for updating the entity each frame
    void (*update)(struct Entity* entity, void* state);
    //Function pointer for rendering the entity each frame
    void (*render)(struct Entity* entity, SDL_Renderer* renderer, struct nk_context* ctx);

} Entity;