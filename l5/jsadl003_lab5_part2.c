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

enum CNT_States { CNT_SMStart, CNT_Wait, CNT_Wait_UP, CNT_Wait_DOWN, CNT_UP, CNT_DOWN,CNT_Reset } CNT_State;

void TickFct_Cnt()
{
  switch(CNT_State) {
    case CNT_SMStart:
        PORTC = 0x07;
       CNT_State = CNT_Wait;
    break;
    case CNT_Wait:
        if(!(~PINA&0x01) && !(~PINA&0x02)){
            CNT_State = CNT_Wait;
        }
        else if((~PINA&0x01) && !(~PINA&0x02)){
            CNT_State = CNT_UP;
        }
        else if(!(~PINA&0x01) && (~PINA&0x02)){
            CNT_State = CNT_DOWN;
        }
        else if((~PINA&0x03) == 0x03){
            CNT_State = CNT_Reset;
        }
    break;
    case CNT_Wait_UP:
        if((~PINA&0x01) && !(~PINA&0x02)){
            CNT_State = CNT_Wait_UP;
        }
        else if((~PINA&0x03)==0x03){
           CNT_State = CNT_Reset;
        }
        else if(!(~PINA&0x01)){
            CNT_State = CNT_Wait;
        }
    break;
    case CNT_Wait_DOWN:
        if(!(~PINA&0x01) && (~PINA&0x02)){
            CNT_State = CNT_Wait_DOWN;
        }
        else if((~PINA&0x03)){
            CNT_State =  CNT_Reset;
        }
        else if(!(~PINA&0x02)){
            CNT_State = CNT_Wait;
        }
    break;
    case CNT_UP:
        CNT_State = CNT_Wait_UP;
    break;
    case CNT_DOWN:
        CNT_State = CNT_Wait_DOWN;
    break;
    case CNT_Reset:
        if((~PINA&0xFF) == 0x00){
        CNT_State = CNT_Wait;
        }
        else{
                CNT_State = CNT_Reset;
        }

    break;
  }

   switch(CNT_State) {
    case CNT_SMStart:

    break;
    case CNT_Wait:

    break;
    case CNT_Wait_UP:

    break;
    case CNT_Wait_DOWN:

    break;
    case CNT_UP:
        if(PORTC < 9){
            PORTC = PORTC + 0x01;
        }
    break;
    case CNT_DOWN:
        if(PORTC > 0){
            PORTC = PORTC - 0x01;
        };
    break;
    case CNT_Reset:
        PORTC = 0x00;
    break;
  }
}


int main(void){
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    CNT_State = CNT_SMStart; // Indicates initial call
    PORTC = 0x07;
   while(1) {
      TickFct_Cnt();
   }
   return 0;
}

