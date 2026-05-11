#pragma once


typedef struct State {
    void (*enter)( void* data);
    void (*update)(void* data);
    void (*exit)(void* data);
} State;

typedef struct StateMachine {
    State* states;
    int stateCount;
    int currentState;
    void* stateData; // Optional pointer to hold any state-specific data
} StateMachine;

StateMachine* STATEMACHINE_Create(void* initialStateData);
void STATEMACHINE_ChangeState(StateMachine* sm, int newState, void* data);
void STATEMACHINE_AddState(StateMachine* sm, State state);
void STATEMACHINE_Update(StateMachine* sm, void* data);
void STATEMACHINE_Destroy(StateMachine* sm);