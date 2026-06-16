#include "AnimatedSprite.h"
#include "DrawStats.h"
#include "SDL3/SDL_render.h"
#include <stdio.h>
#include <SDL3_image/SDL_image.h>
#include <stdlib.h>
#include <cJSON.h>
#include <stdio.h>
#include <string.h>
AnimatedSprite* ANIMATEDSPRITE_ParseFromQuadTasticJSON(const char* jsonFilePath, SDL_Renderer* renderer) {
    AnimatedSprite* sheet = malloc(sizeof(AnimatedSprite));
    
    char imagePathStr[256] = {0}; // Buffer to hold the constructed image path

    cJSON *root;
    cJSON *quads;
    cJSON *metaData;
    cJSON *imagePath;
    cJSON *pathElements;
    cJSON *pathRoot;
    cJSON *animations;
    cJSON* temp;


    FILE* file; 
    fopen_s(&file, jsonFilePath, "rb");
    fseek(file, 0, SEEK_END); // seek to end of file
    size_t size = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file
    char* buffer = malloc(size + 1); // allocate buffer for file content
    if (!file) {
        fprintf(stderr, "Failed to open JSON file: %s\n", jsonFilePath);
        return NULL;
    }
    size_t readSize = fread(buffer, 1, size, file);
    if (readSize <= 0) {
        fprintf(stderr, "Failed to read JSON file: %s\n", jsonFilePath);
        fclose(file);
        return NULL;
    }
    buffer[readSize] = '\0'; // Null-terminate the buffer
    fclose(file);


    size_t temp_n;
    if (!sheet) {
        fprintf(stderr, "Failed to allocate memory for AnimatedSprite\n");
        return NULL;
    }

    root = cJSON_Parse(buffer);
    if (!root) {
        fprintf(stderr, "Failed to load JSON file: %s\n", cJSON_GetErrorPtr());
        free(buffer);
        return NULL;
    }

    quads = cJSON_GetObjectItem(root, "quads");
    if (!quads) {
        fprintf(stderr, "Failed to get quads object from JSON\n");
        free(buffer);
        return NULL;
    }

    metaData = cJSON_GetObjectItem(quads, "_META");
    if (!metaData) {
        fprintf(stderr, "Failed to get _META object from JSON\n");
        free(buffer);
        return NULL;
    }

    imagePath = cJSON_GetObjectItem(metaData, "image_path");
    if (!imagePath) {
        fprintf(stderr, "Failed to get image_path from JSON\n");
        free(buffer);
        return NULL;
    } 

    bool relative = false;
    pathRoot = cJSON_GetObjectItem(imagePath, "root");
    if (!pathRoot) {
        relative = true;
    }    
    if (!relative){
        pathElements = cJSON_GetObjectItem(imagePath, "elements");
        if (!pathElements) {
            fprintf(stderr, "Failed to get elements array from JSON\n");
            free(buffer);
            return NULL;
        }     

        //Append root to string
        char* rootStr = cJSON_GetStringValue(pathRoot);
        if (!rootStr) {
            fprintf(stderr, "Failed to get string value from root\n");
            free(buffer);
            return NULL;
        } 
        imagePathStr[0] = '\0'; //Initialize string to empty
        strcat_s(imagePathStr, sizeof(imagePathStr), rootStr);
        temp_n = cJSON_GetArraySize(pathElements);

        if (temp_n) {
            for (int i = 0; i < temp_n; i++) {
                temp = cJSON_GetArrayItem(pathElements, i);
                cJSON* pathElement = cJSON_GetArrayItem(pathElements, i);
                char* elementStr = cJSON_GetStringValue(pathElement);
                if (!elementStr) {
                    fprintf(stderr, "Failed to get string value from path element at index %d\n", i);
                    free(buffer);
                    return NULL;
                }
                //Append path element to string with a / separator
                strcat_s(imagePathStr, sizeof(imagePathStr), elementStr);
                if (i < temp_n - 1)
                    strcat_s(imagePathStr, sizeof(imagePathStr), "/");
            }
        }
    }
    else{
        char* relativePathStr = cJSON_GetStringValue(imagePath);
        if (!relativePathStr) {
            fprintf(stderr, "Failed to get string value from image_path\n");
            free(buffer);
            return NULL;
        } 
        //Extract directory from jsonFilePath
        char directory[256] = {0};
        strncpy_s(directory, sizeof(directory), jsonFilePath, strrchr(jsonFilePath, '/') - jsonFilePath);
        //Append directory to relative path
        snprintf(imagePathStr, sizeof(imagePathStr), "%s%s", directory, relativePathStr);
    }

    animations = cJSON_GetObjectItem(root, "animations");

    if (!animations) {
        fprintf(stderr, "Failed to get animations object from JSON\n");
        free(buffer);
        return NULL;
    }
    temp_n = cJSON_GetArraySize(animations);
    sheet->animationCount = temp_n;
    sheet->animations = malloc(sizeof(Animation) * temp_n);
    sheet->currentAnimationIdx = 0;
    for (int i = 0; i < temp_n; i++) {
        cJSON* animation = cJSON_GetArrayItem(animations, i);
        cJSON* flipX = cJSON_GetObjectItem(animation, "flipX");
        cJSON* flipY = cJSON_GetObjectItem(animation, "flipY");
        cJSON* frames = cJSON_GetObjectItem(animation, "frames");
        cJSON* loop = cJSON_GetObjectItem(animation, "loop");
        size_t frameCount = cJSON_GetArraySize(frames);

        if (!animation || !frames || !flipX || !flipY || !loop) {
            fprintf(stderr, "Invalid animation at index %d\n", i);
            free(buffer);
            return NULL;
        }

        sheet->animations[i].frames = malloc(sizeof(Frame) * frameCount);
        sheet->animations[i].frameCount = frameCount;
        sheet->animations[i].currentFrame = 0;
        sheet->animations[i].elapsedTime = 0;
        sheet->animations[i].flipX = flipX->valueint ? true : false;
        sheet->animations[i].flipY = flipY->valueint ? true : false;
        sheet->animations[i].loop = loop->valueint ? true : false;
        for (int j = 0; j < frameCount; j++) {
            cJSON* frame = cJSON_GetArrayItem(frames, j);
            if (!frame) {
                fprintf(stderr, "Invalid frame at index %d for animation %d\n", j, i);
                free(buffer);
                return NULL;
            }
            int duration = atoi(cJSON_GetObjectItem(frame, "duration")->valuestring);
            if (duration <= 0) {
                fprintf(stderr, "Invalid frame duration at index %d for animation %d\n", j, i);
                free(buffer);
                return NULL;
            }

            cJSON* quad = cJSON_GetObjectItem(frame, "quad");
            if (!quad) {
                fprintf(stderr, "Failed to get quad object from JSON\n");
                free(buffer);
                return NULL;
            }
            cJSON* x = cJSON_GetObjectItem(quad, "x");
            cJSON* y = cJSON_GetObjectItem(quad, "y");
            cJSON* width = cJSON_GetObjectItem(quad, "w");
            cJSON* height = cJSON_GetObjectItem(quad, "h");
            cJSON* offsetX = cJSON_GetObjectItem(quad, "ox");
            cJSON* offsetY = cJSON_GetObjectItem(quad, "oy");

            if(!cJSON_IsNumber(x) || !cJSON_IsNumber(y) || !cJSON_IsNumber(width) || !cJSON_IsNumber(height) || !cJSON_IsNumber(offsetX) || !cJSON_IsNumber(offsetY)) {
                fprintf(stderr, "Invalid quad values for frame at index %d for animation %d\n", j, i);
                //Print the quad object for debugging
                char* quadStr = cJSON_Print(quad);
                fprintf(stderr, "Quad JSON: %s\n", quadStr);
                free(quadStr);
                free(buffer);
                return NULL;
            }
            
            float ox = (float)offsetX->valuedouble;
            float oy = (float)offsetY->valuedouble;
            //Convert offset from relative to absolute by multiplying it by the frame dimensionson;

            float absOffsetX = flipX ? width->valuedouble * ox : width->valuedouble * -ox;
            float absOffsetY = flipY ? height->valuedouble * oy : height->valuedouble * -oy;

            Quad* q = malloc(sizeof(Quad)); 
            *q = (Quad){x->valuedouble, y->valuedouble, width->valuedouble, height->valuedouble, absOffsetX, absOffsetY};

            Frame f = {q, duration};
            sheet->animations[i].frames[j] = f;
        }
    }


    sheet->imagePath = malloc(sizeof(char) * (strlen(imagePathStr) + 1));
    strcpy_s(sheet->imagePath, strlen(imagePathStr) + 1, imagePathStr);
    sheet->texture = IMG_LoadTexture(renderer, sheet->imagePath);
    if (!sheet->texture) {
        fprintf(stderr, "Failed to load texture: %s\n", SDL_GetError());
        free(buffer);
        return NULL;
    }
    SDL_SetTextureScaleMode(sheet->texture, SDL_SCALEMODE_NEAREST);
    free(buffer);

    

    return sheet;
}

void ANIMATEDSPRITE_Render(AnimatedSprite* sheet, SDL_Renderer* renderer, int x, int y, float sx, float sy, double angle, float pivotX, float pivotY, SDL_FlipMode flip) {
    if (!sheet || !renderer) return;

    Animation* anim = &sheet->animations[sheet->currentAnimationIdx];
    Frame* frame = &anim->frames[anim->currentFrame];

    float ox = frame->frameQuad->offsetX * sx;
    float oy = frame->frameQuad->offsetY * sy;

    if (pivotX == 0 && pivotY == 0) {
        pivotX = ox;
        pivotY = oy;
    }

    SDL_FRect srcRect = {
        .x = frame->frameQuad->x,
        .y = frame->frameQuad->y,
        .w = frame->frameQuad->width,
        .h = frame->frameQuad->height
    };

    //Scale destRect around pivotX and pivotY by sx and sy
    SDL_FRect destRect = {
        .x = x - frame->frameQuad->offsetX * sx,
        .y = y - frame->frameQuad->offsetY * sy,
        .w = srcRect.w * sx,
        .h = srcRect.h * sy
    };


    DRAWSTATS_Inc();
    //Visualize pivot
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderPoint(renderer, (int)(destRect.x + pivotX), (int)(destRect.y + pivotY));
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderTextureRotated(renderer, sheet->texture, &srcRect, &destRect, angle, &(SDL_FPoint){pivotX, pivotY}, flip);
}



void ANIMATEDSPRITE_Destroy(AnimatedSprite* sheet) {
    if (!sheet) return;

    SDL_DestroyTexture(sheet->texture);
    free(sheet->imagePath);
    for (int i = 0; i < sheet->animationCount; i++) {
        for (int j = 0; j < sheet->animations[i].frameCount; j++) {
            free(sheet->animations[i].frames[j].frameQuad);
        }
        free(sheet->animations[i].frames);
    }
    free(sheet->animations);
    free(sheet);
}