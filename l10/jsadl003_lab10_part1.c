/*  Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK:
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
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
unsigned char threeLEDs;
enum ThreeState {three0,three1,three2,three3} threeState;
void ThreeLEDsSM(){
	    switch(threeState){
	    case three0:
		    threeState = three1;
		    break;		    
	    case three1:
		    threeState = three2;
		    break;		    
	    case three2:
		    threeState = three3;
		    break;		    
	    case three3:
		    threeState = three0;
		    break;
    }
    switch(threeState){
	    case three0:
		    threeLEDs = 0x01;
		    break;		    
	    case three1:
		    threeLEDs = 0x02;
		    break;		    
	    case three2:
		    threeLEDs = 0x04
		    break;		    
	    case three3:
		    threeLEDs = 0x02;
		    break;
    }
}
unsigned char blinkingLED;
enum BlinkState{blink3,blink0}blinkState;
void BlinkingLEDSM(){
    switch(blinkState){
	    case blink3:
		    blinkState = blink3;
		    break;		    
	    case blink0:
		    blinkState = blink0;
		    break;		    
    }
    switch(blinkState){
	    case blink3:
		    blinkingLED = 0x04;
		    break;		    
	    case blink0:
		    blinkingLED = 0x01;
		    break;		    
    }
}
//enum CombineState {} combineState;
void CombineLEDsSM(){
	PORTB = threeLEDs | blinkingLED;
}

int main(void){ // lower 8 on d and upper 2 on c
   DDRA = 0x00; PORTA = 0xFF; 
   DDRB = 0xFF; PORTB = 0x00; 
	blinkState = blink0;
	threeState = three0;
   unsigned int elapsedTime = 0;
     unsigned short period = 100;
   TimerSet(period);
   TimerOn();
  while (1){
    if(elapsedTime >= 1000){
      ThreeLEDsSM();
      BlinkingLEDSM();
      CombineLEDsSM();
    }
    while(!TimerFlag){}
    TimerFlag = 0;
    elapsedTime += period;
  }
   return 0;
}
