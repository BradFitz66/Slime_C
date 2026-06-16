#include "TextureSheet.h"
#include "DrawStats.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
TextureSheet* TEXTURESHEET_New(const char* imagePath, Quad** quads, int quadCount, SDL_Renderer* renderer) {
    TextureSheet* sheet = malloc(sizeof(TextureSheet));
    if (!sheet) {
        fprintf(stderr, "Failed to allocate memory for TextureSheet\n");
        return NULL;
    }
    sheet->imagePath = malloc(sizeof(char) * (strlen(imagePath) + 1));
    if (!sheet->imagePath) {
        fprintf(stderr, "Failed to allocate memory for TextureSheet image path\n");
        free(sheet);
        return NULL;
    }
    strcpy_s(sheet->imagePath, strlen(imagePath) + 1, imagePath);
    sheet->texture = TEXTURE_LoadFromFile(imagePath, renderer);
    if (!sheet->texture) {
        fprintf(stderr, "Failed to load texture for TextureSheet: %s\n", imagePath);
        free(sheet->imagePath);
        free(sheet);
        return NULL;
    }
    sheet->quads = quads;
    sheet->quadCount = quadCount;
    return sheet;
}

void TEXTURESHEET_RenderAllQuadsBatched(TextureSheet* sheet, SDL_Renderer* renderer, int* quadIndices, int* xs, int* ys, int count) {
    if (!sheet || !renderer || !quadIndices || !xs || !ys || count <= 0) return;

    float texW, texH;
    SDL_GetTextureSize(sheet->texture->texture, &texW, &texH);

    SDL_Vertex* vertices = malloc(sizeof(SDL_Vertex) * count * 4);
    int*        indices  = malloc(sizeof(int)        * count * 6);
    if (!vertices || !indices) { free(vertices); free(indices); return; }

    for (int i = 0; i < count; i++) {
        if (quadIndices[i] < 0 || quadIndices[i] >= sheet->quadCount) continue;

        Quad* quad = sheet->quads[quadIndices[i]];

        float dx = (float)xs[i];
        float dy = (float)ys[i];
        float dw = (float)quad->width;
        float dh = (float)quad->height;

        float u0 = (float)quad->x          / texW;
        float v0 = (float)quad->y          / texH;
        float u1 = (float)(quad->x + quad->width)  / texW;
        float v1 = (float)(quad->y + quad->height) / texH;

        SDL_FColor white = {1.0f, 1.0f, 1.0f, 1.0f};

        int vi = i * 4;
        vertices[vi + 0] = (SDL_Vertex){{dx,      dy     }, white, {u0, v0}};
        vertices[vi + 1] = (SDL_Vertex){{dx + dw, dy     }, white, {u1, v0}};
        vertices[vi + 2] = (SDL_Vertex){{dx + dw, dy + dh}, white, {u1, v1}};
        vertices[vi + 3] = (SDL_Vertex){{dx,      dy + dh}, white, {u0, v1}};

        int ii = i * 6;
        indices[ii + 0] = vi + 0;
        indices[ii + 1] = vi + 1;
        indices[ii + 2] = vi + 2;
        indices[ii + 3] = vi + 0;
        indices[ii + 4] = vi + 2;
        indices[ii + 5] = vi + 3;
    }

    DRAWSTATS_Inc();
    SDL_RenderGeometry(renderer, sheet->texture->texture, vertices, count * 4, indices, count * 6);

    free(vertices);
    free(indices);
}

void TEXTURESHEET_RenderQuad(TextureSheet* sheet, SDL_Renderer* renderer, int quadIndex, int x, int y) {
    if (!sheet || !renderer || quadIndex < 0 || quadIndex >= sheet->quadCount) return;

    Quad* quad = sheet->quads[quadIndex];
    SDL_FRect srcRect = {quad->x, quad->y, quad->width, quad->height};
    SDL_FRect destRect = {x, y, quad->width, quad->height};
    DRAWSTATS_Inc();
    SDL_RenderTexture(renderer, sheet->texture->texture, &srcRect, &destRect);
}

void TEXTURESHEET_Destroy(TextureSheet* sheet) {
    if (!sheet) return;

    TEXTURE_Destroy(sheet->texture);
    free(sheet->imagePath);
    for (int i = 0; i < sheet->quadCount; i++) {
        free(sheet->quads[i]);
    }
    free(sheet->quads);
    free(sheet);
}

