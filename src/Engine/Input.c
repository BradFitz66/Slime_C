#include "Input.h"
#include <string.h>

InputSystem gInput = {0};

void INPUT_Init(void) {
    gInput.axisCount = 0;
    gInput.buttonCount = 0;
}

void INPUT_Poll(void) {
    const bool *state = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < gInput.axisCount; i++) {
        InputAxis *axis = &gInput.axes[i];
        float target = 0.0f;
        if (state[SDL_GetScancodeFromKey(axis->positiveKey, SDL_SCANCODE_UNKNOWN)]) target += 1.0f;
        if (state[SDL_GetScancodeFromKey(axis->negativeKey, SDL_SCANCODE_UNKNOWN)]) target -= 1.0f;
        axis->value = target;
    }
    for (int i = 0; i < gInput.buttonCount; i++) {
        InputButton *btn = &gInput.buttons[i];
        bool down = state[SDL_GetScancodeFromKey(btn->key, SDL_SCANCODE_UNKNOWN)] != 0;
        if (down && !btn->prevDown) btn->pendingPressed = true;
        if (!down && btn->prevDown) btn->pendingReleased = true;
        btn->prevDown = down;
    }
}

void INPUT_Flush(void) {
    for (int i = 0; i < gInput.buttonCount; i++) {
        InputButton *btn = &gInput.buttons[i];
        btn->held = btn->prevDown;
        btn->pressed = btn->pendingPressed;
        btn->released = btn->pendingReleased;
        btn->pendingPressed = false;
        btn->pendingReleased = false;
    }
}

void INPUT_AddAxis(const char *name, int positiveKey, int negativeKey) {
    if (gInput.axisCount >= INPUT_MAX_AXES) return;
    InputAxis *axis = &gInput.axes[gInput.axisCount++];
    axis->name = name;
    axis->positiveKey = positiveKey;
    axis->negativeKey = negativeKey;
    axis->value = 0.0f;
}

void INPUT_AddButton(const char *name, int key) {
    if (gInput.buttonCount >= INPUT_MAX_BUTTONS) return;
    InputButton *btn = &gInput.buttons[gInput.buttonCount++];
    btn->name = name;
    btn->key = key;
    btn->held = false;
    btn->pressed = false;
    btn->released = false;
    btn->prevDown = false;
    btn->pendingPressed = false;
    btn->pendingReleased = false;
}

float INPUT_GetAxis(const char *name) {
    for (int i = 0; i < gInput.axisCount; i++) {
        if (strcmp(gInput.axes[i].name, name) == 0)
            return gInput.axes[i].value;
    }
    return 0.0f;
}

bool INPUT_GetButton(const char *name) {
    for (int i = 0; i < gInput.buttonCount; i++) {
        if (strcmp(gInput.buttons[i].name, name) == 0)
            return gInput.buttons[i].held;
    }
    return false;
}

bool INPUT_GetButtonDown(const char *name) {
    for (int i = 0; i < gInput.buttonCount; i++) {
        if (strcmp(gInput.buttons[i].name, name) == 0)
            return gInput.buttons[i].pressed;
    }
    return false;
}

bool INPUT_GetButtonUp(const char *name) {
    for (int i = 0; i < gInput.buttonCount; i++) {
        if (strcmp(gInput.buttons[i].name, name) == 0)
            return gInput.buttons[i].released;
    }
    return false;
}



