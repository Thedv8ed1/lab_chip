/*	Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #6  Exercise #1
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

enum State{Start, sB0, sB1, sB2} state;

void TimerISR(){
    TimerFlag = 1;
}

void Tick(){
    switch(state){
        case Start:
         state = sB0;
        break;
        case sB0:
         state = sB1;
        break;
        case sB1:
         state = sB2;
        break;
        case sB2:
         state = sB0;
        break;
        default:
        break;
    }
    
    switch(state){
        case Start:
        
        break;
        case sB0:
         PORTB = 0x01;
        break;        
        case sB1:
         PORTB = 0x02;
        break;        
        case sB2:
         PORTB = 0x04;
        break;
        default:
        break;
    }
}

void main()
{
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0x00; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
   TimerSet(1000);
   TimerOn();
   state = Start;
   while (1) { 
      Tick();
      while(!TimerFlag){}
      TimerFlag = 0;
   }
}
