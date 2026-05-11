#pragma once
#include "SDL3/SDL_surface.h"
#include <SDL3/SDL.h>
#include <stdbool.h>

//Struct to describe a texture
typedef struct {
    SDL_Texture* texture;
    char* imagePath;
} Texture;

void TEXTURE_Destroy(Texture* texture);
void TEXTURE_Render(Texture* texture, SDL_Renderer* renderer, int x, int y);
void TEXTURE_RenderEx(Texture* texture, SDL_Renderer* renderer, int x, int y, double angle, SDL_FlipMode flip);
// Renders the texture centered at (cx, cy) scaled by 'scale'.
void TEXTURE_RenderCentered(Texture* texture, SDL_Renderer* renderer, int cx, int cy, float scale);
Texture* TEXTURE_LoadFromFile(const char* filePath, SDL_Renderer* renderer);