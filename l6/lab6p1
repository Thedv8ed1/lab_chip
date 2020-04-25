#include "rims.h"

volatile unsigned char TimerFlag=0;

enum State{Start, sB0, sB1, sB2} state;

void TimerISR(){
    TimerFlag = 1;
}

void Tick(){
    switch(state){
        case Start:
         state = sB0;
        break;
        case sB0:
         state = sB1;
        break;
        case sB1:
         state = sB2;
        break;
        case sB2:
         state = sB0;
        break;
        default:
        break;
    }
    
    switch(state){
        case Start:
        
        break;
        case sB0:
         PORTB = 0x01;
        break;        
        case sB1:
         PORTB = 0x02;
        break;        
        case sB2:
         PORTB = 0x04;
        break;
        default:
        break;
    }
}

void main()
{
   PORTB = 0; //Init outputs
   TimerSet(1000);
   TimerOn();
   state = Start;
   while (1) { 
      Tick();
      while(!TimerFlag){}
      TimerFlag = 0;
   }
}
