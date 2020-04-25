/*	Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK: 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
volatile unsigned char TimerFlag=0;

enum CNT_States { CNT_SMStart, CNT_Wait,CNT_UP, CNT_DOWN,CNT_Reset } CNT_State;

void TimerISR(){
    TimerFlag = 1;
}

void Tick(){
   switch(CNT_State) {   
    case CNT_SMStart:
        PORTB = 0x07;
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
    case CNT_UP:
        if((~PINA&0x01) && !(~PINA&0x02)){
            CNT_State = CNT_UP;
        }
	else if((~PINA&0x03)==0x03){
           CNT_State = CNT_Reset;
        }
        else if(!(~PINA&0x01)){
            CNT_State = CNT_Wait;
        }
    break;
    case CNT_DOWN:
        if(!(~PINA&0x01) && (~PINA&0x02)){
            CNT_State = CNT_DOWN;
        }
        else if((~PINA&0x03)){
            CNT_State =  CNT_Reset;
        }
        else if(!(~PINA&0x02)){
            CNT_State = CNT_Wait;
        }
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
    case CNT_UP:

    break;
    case CNT_DOWN:
   
    break;
    case CNT_Reset:
        PORTB = 0x00;
    break;
  }
}

void CNT(){
    switch(CNT_State){
      case CNT_UP:
         if(B < 9){
            PORTB = PORTB + 0x01;
        }
      break;
     case CNT_DOWN:
        if(PORTB > 0){
            PORTB = PORTB - 0x01;
        };
    break;
    }
}

void main()
{
  DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0x00; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
    unsigned long elapsedTime = 1000;
    const unsigned long period = 100;
  
   TimerSet(period);
   TimerOn();
   CNT_State = CNT_SMStart;
   while (1) { 
       if(elapsedTime >= 1000){
           CNT();
           elapsedTime = 0;
       }
      Tick();
      while(!TimerFlag){}
      TimerFlag = 0;
      elapsedTime += period;
   }
}
