/*	Author: Abdullah Chaudhry
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://drive.google.com/file/d/1uAy0s-yjmbd9Q5DMutbNh2qzZhoCplyR/view?usp=sharing
 *
 */
#include <avr/io.h>
#include "timer.h"
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

enum States {Start, Button1Press, Button1Release, Button2Press, Button2Release, Button3Press, Button3Release} state;

unsigned char noteNum = 0;
unsigned char systemOn = 0;
double notes[8] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25}
void Tick(){
    switch(state){
    case Start:
        if(~PINA == 0x01){
            state = Button1Press;
        }else if(~PINA == 0x02){
            state = Button2Press;
        }else if(~PINA == 0x04){
            state = Button3Press;
        }else{
            state = Start;
        }
        break;

    case Button1Press:
        state = Button1Release;
        break;

    case Button1Release:
        if(~PINA == 0x00){
            if(!noteNum == 7){
                noteNum++;
            }
        }else{
            state = Button1Release; //Wait for button release
        }
        break;

    case Button2Press:
        state = Button2Release;
        break;

    case Button2Release:
        if(~PINA == 0x00){
            if(!noteNum == 0){
                noteNum--;
            }
        }else{
            state = Button2Release; //Wait for button release
        }
        break;

    case Button3Press:
        state = Button3Release;
        break;

    case Button3Release:
        if(~PINA == 0x00){
            if(~systemOn){
                set_PWM(notes[noteNum]);
                systemOn = 1;
            }else{
                set_PWM(0);
                systemOn = 0;
            }
        }else{
            state = Button3Release; //Wait for button release
        }

    default:
        state = Start;
        break;
    }

    switch(state){
    case Start:
        break;
    case Button1Press:
        if(systemOn){
            set_PWM(notes[noteNum]);
        }
        break;
    case Button1Release:
        break;
    case Button2Press:
        if(systemOn){
            set_PWM(notes[noteNum]);
        }
        break;
    case Button2Release:
        break;
    case Button3Press:
        break;
    case Button3Release:
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
    TimerSet(50);
    TimerOn();
    PWM_on();
    while (1) {
        Tick();
	while(!TimerFlag) {};
	TimerFlag = 0;
    }
    return 1;
}
