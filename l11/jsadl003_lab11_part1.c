/*  Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK: 
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.h"
#include "scheduler.h"
#include "keypad.h"
//#include "lcd_8bit_task.h"
//#include "queue.h"
//#include "seven_seg.h"
//#include "stack.h"
//#include "usart.h"
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char keypadValue;
enum readStates { readState_read};
int read(int state){
	switch(state){
		case readState_read:
			keypadValue = GetKeypadKey();
		break;
                default: break;
        }
	return state;
}

enum writeStates {writeState_write};
int write(int state){
	switch(state){
		case writeState_write:
                    switch(keypadValue){
                        case '\0': PORTB = 0x00; break;
            		    case '1': PORTB = 0x01; break;
			    case '2': PORTB = 0x02; break;
		            case '3': PORTB = 0x03; break;
		            case '4': PORTB = 0x04; break;
		            case '5': PORTB = 0x05; break;
		            case '6': PORTB = 0x06; break;
		            case '7': PORTB = 0x07; break;
		            case '8': PORTB = 0x08; break;
		            case '9': PORTB = 0x09; break;
		            case '0': PORTB = 0x00; break;
		            case '*': PORTB = 0x0E; break;
		            case '#': PORTB = 0x0F; break;
		            case 'A': PORTB = 0x0A; break;
		            case 'B': PORTB = 0x0B; break;
		            case 'C': PORTB = 0x0C; break;
		            case 'D': PORTB = 0x0D; break;
		            default: PORTB = 0x1B; break;
		    }
		break;
	}
	return state;
}

int main(){	
	DDRB = 0xFF; PORTB = 0x00;
        DDRC = 0xF0; PORTC = 0x0F;
	
	//Declare an array of tasks
	static task task1, task2;
	task *tasks[] = { &task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1(pauseButtontoggle
	task1.state = 0;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &read;
	// task 2 toggleLED0SM
	task2.state = 0;
	task2.period = 50;
	task2.elapsedTime = task2.period;
	task2.TickFct = &write;
	
	unsigned long GCD = tasks[0]->period;
	for(int i = 1; i < numTasks; i++){
		GCD = findGCD(GCD,tasks[i]->period);	
	}
	
	TimerSet(GCD);
	TimerOn();
	
	unsigned short i;
	while(1){
	    for(i = 0; i < numTasks; i++){
		    if(tasks[i]->elapsedTime == tasks[i]->period){
			    tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			    tasks[i]->elapsedTime = 0;
		    }
		    tasks[i]->elapsedTime+=GCD;
	    }
	    while(!TimerFlag);
	    TimerFlag = 0;
	}

	return 0;
}
/*
int main(void){ // lower 8 on d and upper 2 on c
   DDRB = 0xFF; PORTB = 0x00; 
   DDRC = 0xF0; PORTC = 0x0F;
   unsigned int elapsedTime = 0;
     unsigned short period = 100;
   TimerSet(period);
   TimerOn();
	unsigned char x;
  while (1){
    x = GetKeypadKey();
    switch(x){
	    case '\0': PORTB = 0x1F; break;
	    case '1': PORTB = 0x01; break;
	    case '2': PORTB = 0x02; break;
	    case '3': PORTB = 0x03; break;
	    case '4': PORTB = 0x04; break;
	    case '5': PORTB = 0x05; break;
	    case '6': PORTB = 0x06; break;
	    case '7': PORTB = 0x07; break;
	    case '8': PORTB = 0x08; break;
	    case '9': PORTB = 0x09; break;
	    case '0': PORTB = 0x00; break;
	    case '*': PORTB = 0x0E; break;
	    case '#': PORTB = 0x0F; break;
	    case 'A': PORTB = 0x0A; break;
	    case 'B': PORTB = 0x0B; break;
	    case 'C': PORTB = 0x0C; break;
	    case 'D': PORTB = 0x0D; break;
	    default: PORTB = 0x1B; break;
    }
    while(!TimerFlag){}
    TimerFlag = 0;
  }
   return 0;
}
*/
