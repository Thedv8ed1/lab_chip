/*	Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #6  Exercise #2
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

enum State{Start, sB0, sB1, sB2, sB3,gameOver,gameOverWait} state;

void Tick(){
    switch(state){
        case Start: state = sB0; PORTB = 0x01; break;
        case sB0: state = sB1; break;
        case sB1: state = sB2; break;
        case sB2: state = sB3; break;  
	case sB3: state = sB0; break; 	  
	case gameOver: break;
	case gameOverWait: break;
        default: break;
    }
    switch(state){
        case Start: break;
        case sB0: PORTB = 0x01; break;        
        case sB1: PORTB = 0x02; break;        
        case sB2: PORTB = 0x04; break;
	case sB3: PORTB = 0x02; break;
	case gameOver: break;
        case gameOverWait: break;
        default: break;
    }
}

void Tick_input(){	
 switch(state){
        case Start: break;
        case sB0:         
        case sB1:   
        case sB2:
	case sB3:
         if((~PINA&0x01) == 0x01){
            state = gameOverWait;
          }
	 break;
	case gameOverWait:	 	
	 if((~PINA&0x01) == 0x01){
            state = gameOverWait;
          }
	  else{
	   state = gameOver;
	  }
	break;
	case gameOver:
          if((~PINA&0x01) == 0x01){
            state = Start;
          }
	break;
        default: break;
    }
 switch(state){
        case Start: break;
        case sB0: break;
        case sB1: break;
        case sB2: break;
	case sB3: break;
	case gameOverWait: break;
	case gameOver: break;
        default: break;
    }
}

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRB = 0xFF; PORTB = 0x01; // Configure port B's 8 pins as outputs, initialize to 0s

    unsigned int elapsedTime = 0;  
   unsigned char period = 300; 
   TimerSet(period);
   TimerOn();
   state = sB0;
   while (1) { 
      Tick_input();
      while(!TimerFlag){}
      TimerFlag = 0;
      elapsedTime += period;
      if(elapsedTime >= 300){
          Tick();
          elapsedTime = 0;
      }

   }
   return 0;
}
