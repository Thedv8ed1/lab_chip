/*	Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum States { Start, OFF, OFFW, A,AW, Five,FiveW,BLINK, BLINKW } state;
unsigned char cnt;
void TickFct_led()
{

  switch(state) {
    case Start:
        PORTC = 0x00;
	cnt = 0;
       state = OFF;
    break;
    case OFF:
	if((~PINA&0x01) == 0x00){
	    state = OFF;
	}
	else if((~PINA&0x01) == 0x01){
	    state = FiveW;
	}
    break;
    case FiveW:
	if((~PINA&0x01) == 0x01){
		state = FiveW;
	}
	else if((~PINA&0x01) == 0x00){
		state = Five;
	}
    break;
    case Five:
	if((~PINA&0x01) == 0x00){
		state = Five;
	}
	else if((~PINA&0x01) == 0x01){
		state = AW;
		cnt++;
	}
    break;
    case AW:
    	if((~PINA&0x01) == 0x01){
                state = AW;
        }
        else if((~PINA&0x01) == 0x00){
                state = A;
        }
    break;
    case A:
	if((~PINA&0x01) == 0x00){
                state = A;
        }
        else if((~PINA&0x01) == 0x01){
		if(cnt >= 2){
			state = BLINKW;	
			cnt = 0;
			PORTC = 0xFF;
		}else{
		state = FiveW;
		}
        }
	break;
	case BLINKW:
        if((~PINA&0x01) == 0x01){
                state = BLINKW;
        }
        else if((~PINA&0x01) == 0x00){
                state = BLINK;
        }
	break;
    case BLINK:
        if((~PINA&0x01) == 0x00){
                state = BLINK;
        }
        else if((~PINA&0x01) == 0x01){
		state = BLINKW;
		cnt++;
		PORTC = ~PORTC;
		if(cnt >= 3){
			state = OFFW;
			cnt = 0;
		}				
	}
        break;
       case OFFW:
        if((~PINA&0x01) == 0x01){
                state = OFFW;
        }
        else if((~PINA&0x01) == 0x00){
                state = OFF;
        }
        break;


  }

   switch(state) {
    case Start:

    break;
    case OFF:
        PORTC = 0x00;
    break;
    case Five:
	PORTC = 0x55;
    break;
    case FiveW:
	PORTC = 0x55;
    break;
    case A:
	PORTC = 0xAA;
    break;
    case AW:
	PORTC = 0xAA;
    break;
    case OFFW:
	PORTC = 0x00;
    break;
    default:
    break;
  }
}


int main(void){
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    state = Start; // Indicates initial call

   while(1) {
	   TickFct_led();
   }
   return 0;
}

