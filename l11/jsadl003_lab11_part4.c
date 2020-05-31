/*  Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #11  Exercise #4
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
//#include "queue.h"
//#include "seven_seg.h"
//#include "stack.h"
//#include "usart.h"
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

const unsigned char * string = "--Quetzalcoatl--";
int index = 1;

enum DisplayStates {displayState_init,displayState_wait};
int display(int state){
	switch(state){
		case displayState_init:			
			LCD_DisplayString(1,string);
			state = displayState_wait;
		break;
		case displayState_wait:
			if(index == 17){ state = displayState_init; index = 1;}
		break;
                default: break;
        }
	return state;
}
enum ReplaceStates {replaceState_wait,replaceState_replace, replaceState_pressed};
int replace(int state){
	unsigned char key = GetKeypadKey();
	switch(state){
		case replaceState_wait:
		 	if(key != '\0'){
				state = replaceState_replace;
			}
		break;
		case replaceState_replace:
		
			LCD_Cursor(index);
			LCD_WriteData(key);
			index++;
			state = replaceState_pressed;
		break;
		case replaceState_pressed:
			if(key == '\0'){
				state = replaceState_wait;
			}
	}
	return state;
}
int tick(){return 0;}
int main(){
	DDRB = 0xFF; PORTB = 0x00;
        DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xFF; PORTD = 0x00;
LCD_init();
LCD_Cursor(1);
	//Declare an array of tasks
	static task task1, task2;
	task *tasks[] = { &task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1(pauseButtontoggle
	task1.state = 0;
	task1.period = 150;
	task1.elapsedTime = task1.period;
	task1.TickFct = &display;
	// task 2 toggleLED0SM
	task2.state = 0;
	task2.period = 75;
	task2.elapsedTime = task2.period;
	task2.TickFct = &replace;

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
