#ifndef INPUT_H
#define INPUT_H

#include <SDL3/SDL.h>
#include <stdbool.h>

#define INPUT_MAX_AXES    16
#define INPUT_MAX_BUTTONS 16

typedef struct InputAxis {
    const char *name;
    int positiveKey;
    int negativeKey;
    float value; // -1.0 to 1.0
} InputAxis;

typedef struct InputButton {
    const char *name;
    int key;
    bool held;
    bool pressed;         // true only on the frame the key was first pressed
    bool released;        // true only on the frame the key was released
    bool prevDown;        // raw state from last poll
    bool pendingPressed;  // accumulated since last flush
    bool pendingReleased; // accumulated since last flush
} InputButton;

typedef struct InputSystem {
    InputAxis axes[INPUT_MAX_AXES];
    InputButton buttons[INPUT_MAX_BUTTONS];
    int axisCount;
    int buttonCount;
} InputSystem;

// Global input instance — accessible from anywhere via extern
extern InputSystem gInput;

void INPUT_Init(void);
void INPUT_Poll(void);  // Call once per frame — reads raw input, accumulates edges
void INPUT_Flush(void); // Call once per fixed step — delivers accumulated edges to getters

void INPUT_AddAxis(const char *name, int positiveKey, int negativeKey);
void INPUT_AddButton(const char *name, int key);

float INPUT_GetAxis(const char *name);
bool  INPUT_GetButton(const char *name);
bool  INPUT_GetButtonDown(const char *name);
bool  INPUT_GetButtonUp(const char *name);

#endif // INPUT_H
