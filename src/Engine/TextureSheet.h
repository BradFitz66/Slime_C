#pragma once

//File for rendering tilesets based on a  list of quads

#include "Texture.h"
#include "Utils.h"
#include <SDL3/SDL.h>

typedef struct {
    Texture* texture;
    char* imagePath;
    Quad** quads;
    int quadCount;
} TextureSheet;

TextureSheet* TEXTURESHEET_New(const char* imagePath, Quad** quads, int quadCount, SDL_Renderer* renderer);
void TEXTURESHEET_Destroy(TextureSheet* sheet);
void TEXTURESHEET_RenderQuad(TextureSheet* sheet, SDL_Renderer* renderer, int quadIndex, int x, int y);
void TEXTURESHEET_RenderAllQuadsBatched(TextureSheet* sheet, SDL_Renderer* renderer, int* quadIndices, int* xs, int* ys, int count);