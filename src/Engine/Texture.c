#include "Texture.h"
#include <SDL3_image/SDL_image.h>
#include <stdio.h>
#include <string.h>

Texture* TEXTURE_LoadFromFile(const char* filePath, SDL_Renderer* renderer) {
    Texture* texture = malloc(sizeof(Texture));
    if (!texture) {
        fprintf(stderr, "Failed to allocate memory for Texture\n");
        return NULL;
    }

    texture->imagePath = malloc(sizeof(char) * (strlen(filePath) + 1));
    if (!texture->imagePath) {
        fprintf(stderr, "Failed to allocate memory for Texture image path\n");
        free(texture);
        return NULL;
    }
    strcpy_s(texture->imagePath, strlen(filePath) + 1, filePath);
    texture->texture = IMG_LoadTexture(renderer, texture->imagePath);
    if (!texture->texture) {
        fprintf(stderr, "Failed to load texture: %s\n", SDL_GetError());
        free(texture->imagePath);
        free(texture);
        return NULL;
    }


    return texture;
}

void TEXTURE_Destroy(Texture* texture) {
    if (!texture) return;

    SDL_DestroyTexture(texture->texture);
    free(texture->imagePath);
    free(texture);
}

void TEXTURE_Render(Texture* texture, SDL_Renderer* renderer, int x, int y) {
    if (!texture || !renderer) return;

    SDL_FRect destRect = {x, y, 0, 0};
    SDL_GetTextureSize(texture->texture, &destRect.w, &destRect.h);
    SDL_RenderTexture(renderer, texture->texture, NULL, &destRect);
}

void TEXTURE_RenderCentered(Texture* texture, SDL_Renderer* renderer, int cx, int cy, float scale) {
    if (!texture || !renderer) return;

    float w, h;
    SDL_GetTextureSize(texture->texture, &w, &h);
    float sw = w * scale;
    float sh = h * scale;
    SDL_FRect destRect = { cx - sw / 2, cy - sh / 2, sw, sh };
    SDL_RenderTexture(renderer, texture->texture, NULL, &destRect);
}