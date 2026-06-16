#define CUTE_TILED_IMPLEMENTATION
#include "Tilemap.h"
#include <cJSON.h>
#include <stdio.h>
#include <cute_tiled.h>
#include "TextureSheet.h"

Tilemap* TILEMAP_LoadFromFile(const char* filePath, SDL_Renderer* renderer) {
    Tilemap* tilemap = malloc(sizeof(Tilemap));
    if (!tilemap) {
        fprintf(stderr, "Failed to allocate memory for Tilemap\n");
        return NULL;
    }
    tilemap->mapData = cute_tiled_load_map_from_file(filePath,NULL);
    if (!tilemap->mapData) {
        fprintf(stderr, "Failed to load tilemap from file: %s\n", filePath);
        free(tilemap);
        return NULL;
    }
    cute_tiled_layer_t* layer = tilemap->mapData->layers;
    int* tileIDs = layer->data;
    int tileCount = layer->width * layer->height;

    assert(tileCount > 0);

    Quad** quads = malloc(sizeof(Quad*) * tileCount);
    int* quadIndices = malloc(sizeof(int) * tileCount);

    for (int i = 0; i < tileCount; i++) {
		int hflip, vflip, dflip;
		int global_tile_id = tileIDs[i];
		cute_tiled_get_flags(global_tile_id, &hflip, &vflip, &dflip);
		global_tile_id = cute_tiled_unset_flags(global_tile_id);

		// assume the map file only has one tileset
		// resolve the tile id based on the matching tileset's first gid
		// the final index can be used on the `images` global array
		int id = global_tile_id - tilemap->mapData->tilesets->firstgid;
        Quad* quad = malloc(sizeof(Quad));
        quad->x = (id % tilemap->mapData->tilesets->columns) * tilemap->mapData->tilesets->tilewidth;
        quad->y = (id / tilemap->mapData->tilesets->columns) * tilemap->mapData->tilesets->tileheight;
        quad->width = tilemap->mapData->tilesets->tilewidth;
        quad->height = tilemap->mapData->tilesets->tileheight;
        quad->offsetX = 0;
        quad->offsetY = 0;
        quads[i] = quad;
        quadIndices[i] = id;

    }

    const char* imagePath = tilemap->mapData->tilesets->image.ptr;
    //Make path relative to assets
    char imagePathStr[256];
    const char* lastSlash = strrchr(imagePath, '/');
    if (lastSlash) {
        snprintf(imagePathStr, sizeof(imagePathStr), "Assets/%s", lastSlash + 1);
    } else {
        snprintf(imagePathStr, sizeof(imagePathStr), "Assets/%s", imagePath);
    }
    
    
    tilemap->texture = TEXTURESHEET_New(imagePathStr, quads, tileCount, renderer);
    tilemap->tileIndices = quadIndices;
    
    return tilemap;
}

void TILEMAP_Render(Tilemap* tilemap, SDL_Renderer* renderer, int x, int y) {
    if (!tilemap || !renderer) return;

    int mapWidth   = tilemap->mapData->width;
    int mapHeight  = tilemap->mapData->height;
    int tileWidth  = tilemap->mapData->tilesets->tilewidth;
    int tileHeight = tilemap->mapData->tilesets->tileheight;
    int tileCount  = mapWidth * mapHeight;

    int* xs = malloc(sizeof(int) * tileCount);
    int* ys = malloc(sizeof(int) * tileCount);
    int* seq = malloc(sizeof(int) * tileCount);
    if (!xs || !ys || !seq) { free(xs); free(ys); free(seq); return; }

    for (int i = 0; i < tileCount; i++) {
        seq[i] = i;
        xs[i]  = x + (i % mapWidth) * tileWidth;
        ys[i]  = y + (i / mapWidth) * tileHeight;
    }

    TEXTURESHEET_RenderAllQuadsBatched(tilemap->texture, renderer, seq, xs, ys, tileCount);

    free(xs); free(ys); free(seq);
}

void TILEMAP_Destroy(Tilemap* tilemap) {
    if (!tilemap) return;

    TEXTURESHEET_Destroy(tilemap->texture);
    free(tilemap->tileIndices);
    cute_tiled_free_map(tilemap->mapData);
    free(tilemap);
}