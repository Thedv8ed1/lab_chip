/*  Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #10  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK: https://drive.google.com/drive/folders/1daQ4V2l1pZbBfR-VGE9UhORS-BQyPCyH?usp=sharing
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
// 0.954 hz is lowest frequency possitble with this function
// based on settings in PWM_on()
// Passing in 0 as the frewuency will stop the speaker from generating sound
void set_PWM(double frequency){
  static double current_frequency; // keps track of the currently set frequency
  // will only update the registers when the frequency changes, otherwise allows
  // music to play uninterrupted
  if (frequency != current_frequency){
    if(!frequency){ TCCR3B &= 0x08;  } // stops timer/counter
    else{TCCR3B |= 0x03; } // resumes/continues timer/counter
    
    // prevents ocr3a from overflowing, using prescaler64
    // 0.954 is smallest frequency that will not result in overflow
    if(frequency < 0.954){OCR3A = 0xFFFF;}
    
    // prevents ocr3a from overflowing, using prescaler64
    // 0.954 is smallest frequency that will not result in underflow
    else if (frequency > 31250){OCR3A = 0x0000;}
    
    // set OCR3A based on desired frequency
    else{OCR3A = (short)(8000000 / (128*frequency)) -1;}
    TCNT3 = 0; // resets counter
    current_frequency = frequency; // updates the current frequency
  }
}

void PWM_on(){
  TCCR3A = (1 << COM3A0);
  TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
  set_PWM(0);
}
void PWM_off(){
  TCCR3A = 0x00;
  TCCR3B = 0x00;
}

unsigned char threeLEDs;
enum ThreeState {three0,three1,three2} threeState;
void ThreeLEDsSM(){
     switch(threeState){
	    case three0: threeState = three1; break;		    
	    case three1: threeState = three2; break;		    
            case three2: threeState = three0; break; }
    switch(threeState){
	    case three0: threeLEDs = 0x01; break;		    
	    case three1: threeLEDs = 0x02; break;		    
	    case three2: threeLEDs = 0x04; break; }
}
unsigned char blinkingLED = 0;
enum BlinkState{blink3,blink0}blinkState;
void BlinkingLEDSM(){
    switch(blinkState){
	    case blink3: blinkState = blink0; break;		    
	    case blink0: blinkState = blink3; break; }
    switch(blinkState){
	    case blink3: blinkingLED = 0x08; break;		    
	    case blink0: blinkingLED = 0x00; break; }
}
unsigned char speaker = 0;
enum SpeakerState {ON, OFF} speakerState;
void SpeakerSM(){
    if((~PINA&0x04) == 0x04){
	    switch(speakerState){
                case ON: speakerState = OFF; break;
                case OFF: speakerState = ON; break; }
	    switch(speakerState){
		case ON: speaker = 0x10; break;
		case OFF: speaker = 0x00; break; }
    }
    else{ speaker = 0; speakerState = OFF;}
}
double frequency = 1;
enum InputStates {released,pressed} inputState;
void input(){
	switch(inputState){
            case released:
	        if((~PINA&0x03) == 0x00){ inputState = released; }
		else if((~PINA&0x03) == 0x01){
		    if(frequency < 10){
                        frequency++;
			inputState = pressed;
		    }
		}
		 else if((~PINA&0x03) == 0x02){
                    if(frequency > 0){
                        frequency--;
                        inputState = pressed;
                    }
                 } break;
	    case pressed:
                    if((~PINA&0x03) > 0x00){ inputState = pressed; }
		    else{inputState = released;} break;
   }
}
void CombineLEDsSM(){ PORTB = threeLEDs | blinkingLED | speaker; }
int main(void){ 
   DDRA = 0x00; PORTA = 0xFF; 
   DDRB = 0xFF; PORTB = 0x00; 
   blinkState = blink0;
   threeState = three0;
   speakerState = OFF;
   unsigned int threeElapsedTime = 0;
   unsigned int blinkElapsedTime = 0;
   unsigned int speakerElapsedTime = 0;
   unsigned int inputElapsedTime = 0;
   unsigned short period = 1;
   TimerSet(period);
   TimerOn();
  while (1){
    if(blinkElapsedTime >= 1000){ BlinkingLEDSM(); blinkElapsedTime = 0; }
    if(threeElapsedTime >= 300){ ThreeLEDsSM(); threeElapsedTime = 0; }
    if(speakerElapsedTime >= frequency){ SpeakerSM(); speakerElapsedTime = 0; }
    if(inputElapsedTime >= 75){input(); inputElapsedTime = 0;}
    CombineLEDsSM();
    while(!TimerFlag){}
    blinkElapsedTime += period;
    threeElapsedTime += period;
    speakerElapsedTime += period;
    inputElapsedTime += period;
    TimerFlag = 0;
  }
   return 0;
}
