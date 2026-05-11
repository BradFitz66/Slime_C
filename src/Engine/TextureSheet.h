#pragma once
#include <SDL3/SDL.h>
#include <stdbool.h>
typedef struct {
    int x;
    int y;
    int width;
    int height;
    double offsetX;
    double offsetY;
} Quad;

typedef struct {
    Quad* frameQuad;
    float frameDuration;
} Frame;

typedef struct {
    Frame* frames;
    int frameCount;
    int currentFrame;
    float elapsedTime;   // Time elapsed since the last frame change
    bool flipX;
    bool flipY;
    bool loop; // Whether the animation should loop
} Animation;

typedef struct {
    SDL_Texture* texture;
    char* imagePath;
    Animation* animations;
    int animationCount;
    int currentAnimationIdx;
} TextureSheet;


TextureSheet* TEXTURESHEET_ParseFromQuadTasticJSON(const char* jsonFilePath, SDL_Renderer* renderer);
void TEXTURESHEET_Render(TextureSheet* sheet, SDL_Renderer* renderer, int x, int y);
void TEXTURESHEET_Destroy(TextureSheet* sheet);