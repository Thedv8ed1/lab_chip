/*  Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #11  Exercise #5
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timer.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#define obstacleSize 10
unsigned char play = 1;
unsigned score = 0;
unsigned char player = '>';
const unsigned char player1Bullet = '-';
const unsigned char player2Bullet = 'O';
const unsigned player1Skins[] = {'>',')',']','}','b','E','L','1'};
const unsigned player2Skins[] = {'<','(','[','{','d','3','J','2'};
unsigned char players = 1;
const unsigned char obstacleBreakable = '#';
const unsigned char obstacleSolid = '%';
unsigned char obstacleLocation[obstacleSize] = {0};
enum ObstacleUpdateStates {obstacleCreation, obstacleUpdate,obstacleStop};
int obstacleTick(int state){
    switch(state){
        case obstacleCreation:
	    if(play == 1){ state = obstacleUpdate; }
	    else{state = obstacleStop;} break;
	case obstacleUpdate:
	    if(play == 1){ state = obstacleCreation; }
	    else{state = obstacleStop;} break;
	case obstacleStop:
	    if(play == 1){state = obstacleCreation;} break;
    }
    switch(state){
    case obstacleCreation:
        for(int i = 0; i < obstacleSize; i++){ // checking if obstacle has passed player
	    if(obstacleLocation[i] == 0){ // obstacle has passed so recycle it
		unsigned int location = rand()% 40; // randomize location
		
	        if(location < 17){location = 16;} // top row
	        else if(location < 32){location = 32;} // bottom row
	        else{ continue;} // we wont spawn an obstacle
		for(int j = 0; j < obstacleSize; j++){ // check location of other obstacles so we dont make an impassable wall for the player
		    
		    if((obstacleLocation[j] > 13 && obstacleLocation[j] < 17) ||
				    obstacleLocation[j] > 27) { location = 0;}
		}
		obstacleLocation[i] = location;
	    }
	}
		break;
    case obstacleUpdate:
        for(int i = 0; i < obstacleSize; i++){
	    if(obstacleLocation[i] == 0){continue;}// dont need to update
	    LCD_Cursor(obstacleLocation[i]);
	    LCD_WriteData(' ');
	    if(obstacleLocation[i] == 17){ obstacleLocation[i] = 0;continue;} // bottom had reached its end
	    obstacleLocation[i] = obstacleLocation[i] - 1;
	    LCD_Cursor(obstacleLocation[i]);
	    LCD_WriteData(obstacleBreakable);
	  
  	}
    break;
    default: break;
        }
    return state;
}
unsigned playerLocation;
enum playerStates {playerState_init, playerState_render, playerState_update, playerState_stop};
int playerTick(int state){
    switch(state){
        case playerState_init:
           playerLocation = rand() % 2; // randomize starting row
	   if(playerLocation == 1){playerLocation = 17;} 
	   else{playerLocation = 1;}
	   state = playerState_render;
        break;
	case playerState_render:
	    if(play == 1){state = playerState_update;}
	    else{state = playerState_stop;}
	break;
        case playerState_update:
            if(play == 1){state = playerState_render;}
            else{state = playerState_stop;}
        break;
	case playerState_stop: if(play == 1){state = playerState_render;} break;
    }
    switch(state){
        case playerState_render: LCD_Cursor(playerLocation); LCD_WriteData(player); break;
	case playerState_update:
	    // check if player colided with an obstacle
	    for(int i = 0; i < obstacleSize; i++){
	        if(playerLocation == obstacleLocation[i]){play = 3;return state;} // game over
	    }
	    unsigned char input = GetKeypadKey();
	    if(input == '1'){
		if(playerLocation == 17){
			LCD_Cursor(17);
			LCD_WriteData(' '); // this should really be in render
		    playerLocation = 1;
		}
	    }
	    else if(input == '4'){
		if(playerLocation == 1){
                    LCD_Cursor(1);
                    LCD_WriteData(' '); // this should really be in render
                    playerLocation = 17;
		}
	    }
	    LCD_Cursor(33);
		break;
	}
	return state;
}

const unsigned char *GAMEOVER = "      GAME            OVER        ";
const unsigned char *PAUSED = "     PAUSED                        ";
enum GameStates{ GameState_init, GameState_mainMenuWait,GameState_mainMenuWaitPressed,GameState_modeSelect,GameState_modeSelectPressed, GameState_choosePlayer1Character, GameState_choosePlayer1Character_selectionPress,GameState_choosePlayer1Character_chosenPress ,GameState_Play, GameState_Paused, GameState_Over,GameState_PausedPressed,GameState_PlayPressed};
int gameState(int state){
    unsigned char input = GetKeypadKey();
    static char opt = 17;
    switch(state){
	case GameState_init:
	    play = 0;
	    score = 0;
	    LCD_DisplayString(5,"Welcome!     PRESS ANY KEY   ");
            state = GameState_mainMenuWait;
	break;
	case GameState_mainMenuWait:
            if(input != '\0'){
                state = GameState_mainMenuWaitPressed;
	       	LCD_DisplayString(2,"1 Player        2 Player");
	        LCD_Cursor(1); LCD_WriteData('*');       
		LCD_Cursor(33);}

	break;
	case GameState_mainMenuWaitPressed:
	    if(input == '\0'){state = GameState_modeSelect;}
	break;
	case GameState_modeSelect:
            if(input == '5'){state = GameState_modeSelectPressed;}
	    break;
        case GameState_modeSelectPressed:
	    if(input == '\0'){
	     state = GameState_choosePlayer1Character;
	     LCD_ClearScreen();
	     for(int i = 1; i <17; i+=2){
		LCD_Cursor(i);
		LCD_WriteData(player1Skins[(i/2)]);
	     }
             LCD_Cursor(opt); LCD_WriteData('*');
	     LCD_Cursor(33);
	    }
        break;
	case GameState_choosePlayer1Character:
	   if(input == '5'){
	       if(players == 2){
                 
	       }
	       else{
                state = GameState_choosePlayer1Character_chosenPress;
		play = 1;
	       }	       
	   }
	break;
	case GameState_choosePlayer1Character_selectionPress:
             if(input == '\0'){state = GameState_choosePlayer1Character;}
	break;
	case GameState_choosePlayer1Character_chosenPress:
          if(input == '\0'){
            state = GameState_Play;
	      player = player1Skins[(opt-16) /2];
	      LCD_ClearScreen();
	  }
	break;
        case GameState_Play:
	    if(play == 2){state = GameState_PausedPressed;LCD_DisplayString(1,PAUSED);}
	    else if(play == 3){state = GameState_Over; LCD_DisplayString(1,GAMEOVER);}
	break;
	case GameState_Paused:
	    if(play == 1){state = GameState_PlayPressed; LCD_ClearScreen();} break;
	case GameState_Over:break;
	case GameState_PausedPressed: if(input == '\0'){ state = GameState_Paused; } break;
	case GameState_PlayPressed: if(input == '\0'){ state = GameState_Play;} break;
    }
    switch(state){
        case GameState_init: break;
        case GameState_modeSelect:
        if(input == '1'){
	    LCD_Cursor(17); LCD_WriteData(' ');
	    LCD_Cursor(1); LCD_WriteData('*'); LCD_Cursor(33);
            players = 1;
        }
	else if(input == '4'){
            LCD_Cursor(1); LCD_WriteData(' ');
            LCD_Cursor(17); LCD_WriteData('*'); LCD_Cursor(33);
	    players = 2;
	}
	break;
	case GameState_choosePlayer1Character:
	   if(input == '1'){
	    opt+=2;
            if(opt > 32){opt-=2; break;}

            LCD_Cursor(opt-2); LCD_WriteData(' ');
            LCD_Cursor(opt); LCD_WriteData('*'); LCD_Cursor(33);
	    state = GameState_choosePlayer1Character_selectionPress;
	   }
	   if(input == '4'){
              opt-=2;
              if(opt < 17){ opt+=2;break; }

              LCD_Cursor(opt+2); LCD_WriteData(' ');
              LCD_Cursor(opt); LCD_WriteData('*'); LCD_Cursor(33);
	      state = GameState_choosePlayer1Character_selectionPress;
	   }
	break;
        case GameState_Play: if(input == '5'){ play = 2;}break; // pause game
	case GameState_Paused: if(input == '5'){play = 1;} break; // resume game
	case GameState_Over:
	    if(input == '5'){
		play = 1;
		for(int i = 0; i < obstacleSize; i++){
			obstacleLocation[i] = 0; // reset the obstacles
		}
		state = GameState_PlayPressed;
		LCD_ClearScreen();
	    }
	break;
    }
    return state;
}
int main(){
	DDRB = 0xFF; PORTB = 0x00;
        DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xFF; PORTD = 0x00;
LCD_init();
LCD_Cursor(1);
srand((long)time(NULL));
	//Declare an array of tasks
	static task task1, task2,task3;
	task *tasks[] = { &task1, &task2,&task3};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// task1
	task1.state = 0;
	task1.period = 125;
	task1.elapsedTime = task1.period;
	task1.TickFct = &obstacleTick;
	// task2
	task2.state = 0;
	task2.period = 75;
	task2.elapsedTime = task2.period;
	task2.TickFct = &playerTick;
	 // task3
        task3.state = 0;
        task3.period = 75;
        task3.elapsedTime = task3.period;
        task3.TickFct = &gameState;


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
