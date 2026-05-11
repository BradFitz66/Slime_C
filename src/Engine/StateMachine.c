#include "StateMachine.h"
#include <stdlib.h>
#include <stdio.h>

StateMachine* STATEMACHINE_Create(void* initialStateData) {
    StateMachine* sm = malloc(sizeof(StateMachine));
    if (!sm) {
        fprintf(stderr, "Failed to allocate memory for StateMachine\n");
        return NULL;
    }
    sm->states = NULL;
    sm->stateCount = 0;
    sm->currentState = -1; // No state active initially
    sm->stateData = initialStateData;
    return sm;
}

void STATEMACHINE_ChangeState(StateMachine* sm, int newState, void* data) {
    if (!sm || newState < 0 || newState >= sm->stateCount) return;

    if (sm->currentState != -1 && sm->states[sm->currentState].exit) {
        sm->states[sm->currentState].exit(data);
    }
    sm->currentState = newState;
    if (sm->states[newState].enter) {
        sm->states[newState].enter(data);
    }
}

void STATEMACHINE_Update(StateMachine* sm, void* data) {
    if (!sm || sm->currentState == -1) return;

    if (sm->states[sm->currentState].update) {
        sm->states[sm->currentState].update(data);
    }
}

void STATEMACHINE_AddState(StateMachine* sm, State state) {
    if (!sm) return;
    sm->stateCount++;
    if (!sm->states) {
        sm->states = malloc(sizeof(State) * sm->stateCount);
        if (!sm->states) {
            fprintf(stderr, "Failed to allocate memory for StateMachine states\n");
            return;
        }
    } else {
        //Reallocate states array to add new state
        sm->states = realloc(sm->states, sizeof(State) * sm->stateCount);
        if (!sm->states) {
            fprintf(stderr, "Failed to reallocate memory for StateMachine states\n");
            return;
        }
    }
    sm->states[sm->stateCount - 1] = state;
}

void STATEMACHINE_Destroy(StateMachine* sm) {
    if (!sm) return;

    free(sm->states);
    free(sm);
}