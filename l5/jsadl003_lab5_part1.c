/*	Author: jsadl003
 *  Partner(s) Name: Jason Sadler
 *	Lab Section: 021
 *	Assignment: Lab #5  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *	DEMO LINK: https://drive.google.com/drive/folders/1qPxwPy1MA2t37H73jzSAGhsBZ2QuHa1n?usp=sharing
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {

  /* Insert DDR and PORT initializations */
  DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
    DDRC = 0xFF; PORTC = 0x00;
    /* Insert your solution below */

unsigned char fuel;
    unsigned char fuelCheck;
    unsigned char indicator;
    while (1) {
	    
	 fuelCheck = ~PINA&0x0F;
        fuel = 0x00;
        indicator = 0;

        if(fuelCheck == 0){ // 0
            fuel = 0;
        }
        else if(fuelCheck <= 0x02){ // 1-2
            fuel = 0x20;
        }
        else if(fuelCheck <= 0x04){ // 3-4
            fuel = 0x30;
        }
        else if(fuelCheck <= 0x06){ // 5-6
            fuel = 0x38;
        }
        else if(fuelCheck <= 0x09){ // 7-9
            fuel = 0x3C;
        }
        else if(fuelCheck <= 0x0C){ // 10 -12
            fuel = 0x3E;
        }
        else if(fuelCheck <= 0x0F){
            fuel = 0x3F;
        }

        if(fuelCheck <= 0x04){
            fuel = (fuel | 0x40);
        }
        else{
            fuel = (fuel  & 0x3F);
        }
        fuelCheck = PINA ;
        if(((fuelCheck>>4)&0x01==0x01) && ((fuelCheck>>5)&0x01==0x01) && !((fuelCheck>>6)&0x01==0x01)){
            indicator = 0x80;
        }
        PORTC = (fuel | indicator);
	

    }
    return 1;
}

