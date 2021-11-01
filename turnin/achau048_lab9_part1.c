/*  Author: Abdullah Chaudhry
 *  Partner(s) Name: 
 *  Lab Section:
 *  Assignment: Lab #9  Exercise #1
 *  Exercise Description: [optional - include for your own benefit]
 *
 *  I acknowledge all content contained herein, excluding template or example
 *  code, is my own original work.
 *
 *  Demo Link: https://drive.google.com/file/d/18ankF7LQiyj19twoE3AC2NgOSvL-hOxp/view?usp=sharing
 *
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency){
    static double currFreq;
    if(frequency != currFreq){
        if(!frequency) { 
            TCCR3B |= 0x08;
        }else{
            TCCR3B |= 0x03;
        }

        if(frequency < .954){
            OCR3A = 0xFFFF;
        }else if(frequency > 31250){
            OCR3A = 0x0000;
        }else{
            OCR3A = (short)(8000000 / (128 * frequency)) - 1;
        }

        TCNT3 = 0;
        currFreq = frequency;
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

enum States {Start, Button1, Button2, Button3} state;


void Tick() {
    switch(state) {
       case Start:
          if(PINA & 0x07 == 0x01){
             state =  Button1;
          }else if(PINA & 0x07 == 0x02){
             state = Button2;
          }else if(PINA & 0x07 == 0x04){
             state = Button3;
          }else{
            state = Start;
          }
          break;

       case Button1:
          if(PINA & 0x07 == 0x01) {
            state = Button1;
          }else{
            state = Start;
          }
          break;
       case Button2:
          if(PINA & 0x07 == 0x02) {
            state = Button2;
          }else{
            state = Start;
          }
          break;
       case Button3:
          if (PINA & 0x07 == 0x04) {
            state = Button3;
          }else{
            state = Start;
          }
          break;
       default:
          state = Start;
          break;
    }

    switch(state) {
       case Start:
          set_PWM(0);
          break;
       case Button1:
          set_PWM(261.63);
          break;
       case Button2:
          set_PWM(293.66);
          break;
       case Button3:
          set_PWM(329.63);
          break;
       default:
          break;
    }
}


int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    /* Insert your solution below */
    PWM_on();
    state = Start;

    while(1) {
        Tick();
    }
    return 1;
}
