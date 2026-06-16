#pragma once
#include <SDL3/SDL.h>
#include <stdbool.h>
#include "Utils.h"
typedef struct Frame {
    Quad* frameQuad;
    float frameDuration;
} Frame;

typedef struct Animation {
    Frame* frames;
    int frameCount;
    int currentFrame;
    float elapsedTime;   // Time elapsed since the last frame change
    bool flipX;
    bool flipY;
    bool loop; // Whether the animation should loop
} Animation;

typedef struct AnimatedSprite {
    SDL_Texture* texture;
    char* imagePath;
    Animation* animations;
    int animationCount;
    int currentAnimationIdx;
} AnimatedSprite;


AnimatedSprite* ANIMATEDSPRITE_ParseFromQuadTasticJSON(const char* jsonFilePath, SDL_Renderer* renderer);
void ANIMATEDSPRITE_Render(AnimatedSprite* sheet, SDL_Renderer* renderer, int x, int y, float sx, float sy, double angle, float pivotX, float pivotY, SDL_FlipMode flip);
void ANIMATEDSPRITE_Destroy(AnimatedSprite* sheet);