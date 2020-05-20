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
#include "keypad.h"
#include "lcd_8bit_task.h"
#include "queue.h"
#include "scheduler.h"
#include "seven_seg.h"
#include "stack.h"
#include "usart.h"
#include "timer.h"
#include "receive.hex"
#include "send.hex"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//---------Shared Variables----------
unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;
// ------------End shared variables---------

// Enumeration of states
enum pauseButtonSM_States {pauseButton_wait, pauseButton_press, pauseButton_release};

// monitors button connected to PA0
// when button is pressed, shared cariable "pause" is toggled.
int pauseButtonSMTick(int state){
	//local variable
	unsigned char press = ~PINA&0x01;
	
	switch(state){ // state machine transition
		case pauseButton_wait:
			state = press == 0x01? pauseButton_press: pauseButton_wait; break;
		case pauseButton_press:
			state = pauseButton_release; break;
		case pauseButton_release:
			state = press == 0x00? pauseButton_wait:pauseButton_press; break;
		default: state = pauseButton_wait; break;
	}
	switch(state){ // state action
		case pauseButton_wait: break;
		case pauseButton_press:
			pause = (pause == 0) ? 1 : 0; // toggle pause
		case pauseButton_release: break;
	}
	return state;
}

enum toggleLED0_States {toggleLED0_wait, toggleLED0_blink};
//paused: no toggle PB0
//unpaused toggle PB0
int toggleLED0SMTick(int state){
	switch(state){
		case toggleLED0_wait: state = !pause? toggleLED0_blink: toggleLED0_wait; break;
		case toggleLED0_blink: state = pause? toggleLED0_wait: toggleLED0_blink; break;
		default: state = toggleLED0_wait; break;
	}
	switch(state){
		case toggleLED0_wait: break;
		case toggleLED0_blink:
			led0_output = (led0_output == 0x00)? 0x01: 0x00; break;
	}
	return state;
}
enum toggleLED1_States {toggleLED1_wait, toggleLED1_blink};
//paused: no toggle PB1
//unpaused toggle PB1
int toggleLED1SMTick(int state){
	switch(state){
		case toggleLED1_wait: state = !pause? toggleLED1_blink: toggleLED1_wait; break;
		case toggleLED1_blink: state = pause? toggleLED1_wait: toggleLED1_blink; break;
		default: state = toggleLED1_wait; break;
	}
	switch(state){
		case toggleLED1_wait: break;
		case toggleLED1_blink:
			led0_output = (led1_output == 0x00)? 0x01: 0x00; break;
	}
	return state;
}

enum display_States {display_display};
int displaySMTick(int state){
	unsigned char output;
	
	switch(state){
		case display_display: state = display_display; break;
		default: state = display_display; break;
	}
	switch(state){
		case display_display:
			output = led0_output | led1_output << 1;
			break;
	}
	PORTB = output;
	return state;
}

int main(){
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	//Declare an array of tasks
	static _task task1, task2, task3, task4;
	_task *tasks[] = { &task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1(pauseButtontoggle
	task1.state = start;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &pauseButtonToggleSMTick;
	// task 2 toggleLED0SM
	task2.state = start;
	task2.period = 500;
	task2.elapsedTime = task2.period;
	task2.TickFct = &toggleLED0SMTick;
	// task 3 toggleLED1SM	
	task3.state = start;
	task3.period = 1000;
	task3.elapsedTime = task3.period;
	task3.TickFct = &toggleLED1SMTick;
	//task 4 displaySM
	task4.state = start;
	task4.period = 10;
	task4.elapsedTime = task4.period;
	task4.TickFct = &displaySMTick;
	
	unsigned long GCD = tasks[0]->period;
	for(int i = 1; i < numTasks; i++){
		GCD = findGCD(GCD,tasks[i]->period);	
	}
	
	TimerSet(GCD);
	TimerOn();
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
    x = GetKeyPadKey();
    switc(x){
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
	    case default: PORTB = 0x1B; break;
    }
    while(!TimerFlag){}
    TimerFlag = 0;
  }
   return 0;
}
*/
