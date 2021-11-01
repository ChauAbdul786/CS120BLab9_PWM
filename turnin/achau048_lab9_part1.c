/*	Author: Abdullah Chaudhry
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
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

void PWN_on(){
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off(){
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    /* Insert your solution below */
    PWM_on();
    while (1) {
        if(~PINA == 0x01){
            set_PWM(261.63);
        }else if(~PINA == 0x02){
            set_PWM(293.66);
        }else if(~PINA == 329.63){
            set_PWM(329.63);
        }else{
            set_PWM(0);
        }
    }
    return 1;
}
