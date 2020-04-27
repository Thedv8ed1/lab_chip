/*      Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK: 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag=0;

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn(){
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B; // bit 3 = 0: CTC mode (clear timer on compare)
			//bit2bit1bit0 = 011: pre-scaler /64
			//00001011: 0x0B
			//so, 8 mhz clocl or 8,000,000 /64 = 125,000 ticks/s
			//thus TCNT1 register will count at 125,000 ticks
	// AVR output compare register OCR1A
	OCR1A = 125; // Timer interrupt will be generated when TCNT1==OCR1A
			//we want a 1ms tick. 0.001 s * 125,000 ticks = 125
			//so when TCNT1 register equals 125,
			//1 ms has passed. this, we comapre to 125
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	// init avr counter
	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr mullisecond
	//
	// Enable global interrupt
	SREG |= 0x80; // 0x08: 1000000
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	// cpu automaticalls calls when TCNT1 == OCR1(every 1 ms per timeON settings)
	_avr_timer_cntcurr--; // count down to 0 rather then up to TOP
	if(_avr_timer_cntcurr == 0){
		TimerISR(); // call the isr that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// set timerISR() to tick every m ms
void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum CNT_States { CNT_SMStart, CNT_Wait,CNT_UP,CNT_UP_PRESSED,CNT_DOWN,CNT_DOWN_PRESSED, CNT_Reset } CNT_State;

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
        if((~PINA&0x03)==0x03){
           CNT_State = CNT_Reset;
        }
        else if ((~PINA&0x01) == 0x01){
            CNT_State = CNT_UP_PRESSED;
        }
	else{
	 CNT_State = CNT_Wait;
	}
    break;
    case CNT_UP_PRESSED:
        if((~PINA&0x03)==0x03){
           CNT_State = CNT_Reset;
        }
        else if ((~PINA&0x01) == 0x01){
            CNT_State = CNT_UP_PRESSED;
        }
        else{
         CNT_State = CNT_Wait;
        }
	break;
    case CNT_DOWN:
        if((~PINA&0x03)==0x03){
           CNT_State = CNT_Reset;
        }
        else if ((~PINA&0x02) == 0x02){
            CNT_State = CNT_DOWN_PRESSED;
        }
        else{
         CNT_State = CNT_Wait;
        }
    break;
    case CNT_DOWN_PRESSED:
        if((~PINA&0x03)==0x03){
           CNT_State = CNT_Reset;
        }
        else if ((~PINA&0x02) == 0x02){
            CNT_State = CNT_DOWN_PRESSED;
        }
        else{
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
    	if(PORTB<9){
         PORTB = PORTB+1;
	}
    break;
    case CNT_DOWN:
    	if(PORTB >0){
   	PORTB = PORTB-1;
	}
    break;
    case CNT_Reset:
        PORTB = 0x00;
    break;
  }
}

void CNT(){
    switch(CNT_State){
      case CNT_SMStart: break;
      case CNT_UP: break;
      case CNT_DOWN: break;
      case CNT_Reset: break;
      case CNT_UP_PRESSED:
         if(PORTB < 9){
            PORTB = PORTB + 0x01;
        }
      break;
     case CNT_DOWN_PRESSED:
        if(PORTB > 0){
            PORTB = PORTB - 0x01;
        };
    break;
    }
}

int main(void)
{
  DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs, initialize to 0s
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
   return 0;
}
