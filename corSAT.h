#ifndef CORSAT_H
#define CORSAT_H
#endif



#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>


/*
* Library manager for a cubesat state machine
*
*
*/


/* MAIN DEFINITIONS THAT THE USER CAN CHANGE */

#define COR_DEF_MAX_TRANSITIONS_RECORD 64



/* states machine list */
enum COR_STATE
{
    COR_STATE_NOMINAL,
    COR_STATE_COMM,
    COR_STATE_STARTUP,
    COR_STATE_SAFE,
    COR_STATE_MAINTENANCE,
    COR_STATE_HYPERSAFE,
};

/* definition of types */
typedef struct cor_state_transition COR_StateTransition;
typedef struct cor_state_transtion_instance COR_StateTransition_Instance;


/* 
* state transition record struct
* Fields:
* - from_state: the state that the software is exiting
* - to_state: the state that the software is entering
* - reason: a number that indicates the reason of the transition
* - time: time when this transition is happening (seconds from boot up, or unix time)
* Notes:
* This struct is created whenever a state transition happens. All these records are reported in the COR_StateTransition_Instance struct
*/
struct cor_state_transition
{
    COR_STATE from_state;
    COR_STATE to_state;
    uint16_t reason;
    uint32_t time;
};


/*
* state transition instance records
* Fields:
* - COR_StateTransition: array where the transitions record are memorized
* - max_index: maximum number of elements that can be memorized
* - index: number of elements memorized in the structure
* Notes:
* When this struct is populated, the suggestion is to reset the index whenever this telemetry is given to the ground. If the elments
* are more than max_index the new transition is recorded at the first index (circular array)
*/
struct cor_state_transition_instance
{
    COR_StateTransition transitions[COR_DEF_MAX_TRANSITIONS_RECORD];
    size_t max_index;
    size_t index;
};



struct cor_state
{
    uint8_t current_state;

};









/*
* cor_init
* ---------------
* Parameters:
* Returns:
*/
int cor_init();











#ifdef __cplusplus
}
#endif