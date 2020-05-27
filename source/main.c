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
typedef enum ping_states { PInit, PWait, PBlip         } ping_states;
typedef enum detect_eq_states { DEQInit, DEQWait,DEQMotion          } detect_eq_states;
typedef enum detect_max_amp_states { DMAInit, DMAWait, DMADetect     } detect_max_amp_states;
typedef enum detect_zc_states { DZCInit, DZCWait, DZCFirst, DZCSecond } detect_zc_states;
typedef enum transmit_states {TInit, TTrans                                       } transmit_states;

/* shared variables --- fill in as needed */

unsigned int direction;
unsigned int amplitude;
unsigned char detected;
unsigned char vibration;
unsigned char ping;


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
//#include "ping.h"
//#include "detect_eq.h"
//#include "detect_max_amp.h"
//#include "detect_zc.h"
//#include "transmit.h"
unsigned char consecutive = 0;
void Detect_EQ()
{
    switch(detect_eq_state)
    {
        case DEQInit:
            //init variable(s) here.
	    detect_eq_state = DEQWait;
            break;
	case DEQWait:
		if((~PINA&0x07) > 0x00 ){// motion detected
			detect_eq_state = DEQMotion;
		}
	    break;
	case DEQMotion:
	    if(consecutive < 0x0A){
		    detect_eq_state = DEQMotion;
	    }
	    else{
		detect_eq_state = DEQWait;
	    }
	    break;
        default:
            detect_eq_state = DEQInit;
            break;
    }
    switch(detect_eq_state)
    {
        case DEQInit:
            break;
	case DEQWait:direction = 0x00; consecutive = 0x00; detected = 0x00; break;
	case DEQMotion:
		  if((~PINA&0x07) == 0x00){
			consecutive++;
		  }else{
			consecutive = 0x00;
		  }
		   direction = (~PINA&0xF8);
		   detected = 0x01;

		   break;

        default:
            break;
    }
}

unsigned char firstTick;
unsigned char secondTick;
void Detect_ZC()
{
    switch(detect_zc_state)
    {
        case DZCInit:
		detect_zc_state = DZCWait;
            break;
	case DZCWait:
		if(detected == 0x00){
			detect_zc_state = DZCWait;
		}
		else{
			detect_zc_state = DZCFirst;
		}
	    break;
	case DZCFirst:
		detect_zc_state = DZCSecond;
		    break;
	case DZCSecond:
                detect_zc_state = DZCWait;
		break;
        default:
            detect_zc_state = DZCInit;
            break;
    }
    switch(detect_zc_state)
    {
        case DZCInit:
            break;
	    case DZCFirst:
		firstTick = direction;
		    break;
	    case DZCSecond:
		    secondTick = direction;

		    if((firstTick&0x03) == (secondTick&0x03)){
			if((firstTick&0x04) != (secondTick&0x04)){
				vibration = 0x01;
			}
		    }
		    else{ vibration = 0x00;}
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
		detect_max_amp_state = DMAWait;
            break;
	case DMAWait:
		if((~PINA&0xF8) == 0x00){
			detect_max_amp_state = DMAWait;
		}
		else{
			detect_max_amp_state = DMADetect;
		}
	    break;
	case DMADetect: 
		
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

unsigned int pingCount;
void Ping()
{
    switch(ping_state)
    {
        case PInit:
		ping = 0;
		ping_state = PWait;
            break;
	case PWait:
	    if(pingCount == 10){
		    ping_state = PBlip;
	    }
	    break;
	case PBlip:
		ping_state = PWait;
	    break;
        default:
            ping_state = PInit;
            break;
    }
    switch(ping_state)
    {
        case PInit:
            break;
	case PWait:
	    pingCount++;
	    ping = 0;
	    break;
	case PBlip:
	    ping = 1;
	    pingCount = 0;
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
		transmit_state  = TTrans;
            break;
	case TTrans:
	PORTB = (direction)|(vibration<<2)|(detected<<1)|ping;
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
