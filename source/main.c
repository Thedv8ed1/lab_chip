/*	Author: <your name>
 *	Lab Section:
 *  Exam #2
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#include "timer.h"

/* SM state declarations --- fill in as needed */
typedef enum ping_states { PInit,                                           } ping_states;
typedef enum detect_eq_states { DEQInit,                                    } detect_eq_states;
typedef enum detect_max_amp_states { DMAInit,                               } detect_max_amp_states;
typedef enum detect_zc_states { DZCInit,                                    } detect_zc_states;
typedef enum transmit_states {TInit,                                        } transmit_states;

/* shared variables --- fill in as needed */





/* state variables --- do not alter */
ping_states ping_state;
detect_eq_states detect_eq_state;
detect_max_amp_states detect_max_amp_state;
detect_zc_states detect_zc_state;
transmit_states transmit_state;

/* SM definitions --- complete each task as a SM in the appropriate file.
 * Alternatively, you can remove the #include statement and insert your
 *   SM implementation directly. 
 */
#include "ping.h"
#include "detect_eq.h"
#include "detect_max_amp.h"
#include "detect_zc.h"
#include "transmit.h"

void Detect_EQ()
{
    switch(detect_eq_state)
    {
        case DEQInit:
            //init variable(s) here.
            break;
        default:
            detect_eq_state = DEQInit;
            break;
    }
    switch(detect_eq_state)
    {
        case DEQInit:
            break;
        default:
            break;
    }
}
void Detect_ZC()
{
    switch(detect_zc_state)
    {
        case DZCInit:
            break;
        
        default:
            detect_zc_state = DZCInit;
            break;
    }
    switch(detect_zc_state)
    {
        case DZCInit:
            break;
        default:
            break;
    }
}

void Detect_Max_Amp()
{
    switch(detect_max_amp_state)
    {
        case DMAInit:
            break;
        
        default:
            break;
    }
    switch(detect_max_amp_state)
    {
        case DMAInit:
            break;
        default:
            break;
    }
}

void Ping()
{
    switch(ping_state)
    {
        case PInit:
            break;
        default:
            ping_state = PInit;
            break;
    }
    switch(ping_state)
    {
        case PInit:
            break;
        default:
            break;
    }
}

void Transmit()
{
    switch(transmit_state)
    {
        case TInit:
            break;
        default:
            transmit_state = TInit;
            break;
    }
    switch(transmit_state)
    {
        case TInit:
            break;
        default:
            break;
    }

}

/* main function --- do not alter */
int main(void) {
    /* DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    // Common period for all tasks set to 100ms
    TimerSet(100);
    TimerOn();

    // init state vars
    ping_state = PInit;
    detect_eq_state = DEQInit;
    detect_max_amp_state = DMAInit;
    detect_zc_state = DZCInit;
    transmit_state = TInit;

    while (1) {
        Ping();
        Detect_EQ();
        Detect_Max_Amp();
        Detect_ZC();
        Transmit();
        while (!TimerFlag) { }
        TimerFlag = 0;
    }
    return 1;
}
