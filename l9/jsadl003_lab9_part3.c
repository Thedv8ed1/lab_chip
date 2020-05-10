/*      Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #9  Exercise #1
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

const double notes[] = {0,261.63,293.66,329.63,349.23, 392.0, 440.0,493.88,523.25};
const int melody[] = {1,3,6,0,1,3,6,0,7,0,8,7,8,7,6,5,0,5,1,3,4,5,0,5,1,3,4,2,0};
const int noteDuration[] = {156,156,312,312,156,156,312,312,312,156,156,156,156,156,156,243,156,312,312,156,156,312,234,312,312,156,156,312,312};
enum States {OFF,ON,WAIT}state;
unsigned char note;
unsigned short period = 78;
unsigned int elapsedTime = 0;
void Tick(){
	switch(state){
		case OFF: set_PWM(0);break;
		case ON: set_PWM(notes[melody[note]]);break;
	        case WAIT: break;
	}
}
void TickInput(){
    static int counter = 0;
    static int duration;
    switch(state){
        case OFF:
	    if((~PINA&0x01) == 0x01){
                state = ON;
		duration = noteDuration[note];
            }
	    break;
         case ON:
	     elapsedTime += period;
	     if(elapsedTime >= duration){
		note++;
		elapsedTime = 0;
		duration = noteDuration[note];
		if(note > 29){
                    state = WAIT;
                    note = 0;
		    set_PWM(0);
                }
	     }
	     break;
	  case WAIT:
	      if((~PINA&0x01) > 0x00){
		    state = WAIT;
	      }
	      else{
	       state = OFF;
	      }
          break;
        }
}

int main(void){ // lower 8 on d and upper 2 on c
  DDRA = 0x00; PORTA = 0xFF; 
    DDRB = 0xFF; PORTB = 0x00; 
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    TimerSet(period);
   TimerOn();
   note = 0;
   state = OFF;
   PWM_on();
  while (1){
     Tick();
     TickInput();
     while(!TimerFlag){};
     TimerFlag = 0;	 
  }
   return 0;
}
