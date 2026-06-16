#pragma once
#include "TextureSheet.h"
#include <cute_tiled.h>
typedef struct {
    TextureSheet* texture;
    cute_tiled_map_t* mapData;
    int* tileIndices; // Preprocessed list of tile indices for rendering
} Tilemap;

Tilemap* TILEMAP_LoadFromFile(const char* filePath, SDL_Renderer* renderer);
void TILEMAP_Render(Tilemap* tilemap, SDL_Renderer* renderer, int x, int y);
void TILEMAP_Destroy(Tilemap* tilemap);