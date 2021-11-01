/*  Author: Abdullah Chaudhry
 *  Partner(s) Name: 
 *  Lab Section:
 *  Assignment: Lab #9  Exercise #1
 *  Exercise Description: [optional - include for your own benefit]
 *
 *  I acknowledge all content contained herein, excluding template or example
 *  code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {

    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if (_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
    static double current_frequency;
    if (frequency != current_frequency) {
        if (!frequency) { TCCR3B &= 0x08; }
        else { TCCR3B |= 0x03; }
        if (frequency < 0.954) { OCR3A = 0xFFFF; }
        else if (frequency > 31250) { OCR3A = 0x0000; }
        else { OCR3A = (short) (8000000 / (128 * frequency)) -1; }
        TCNT3 = 0;
        current_frequency = frequency;
    }
}

void PWM_on() {
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

enum SM_STATES {SM1_SMStart, SM_FirstButton, SM_SecondButton, SM_ThirdButton} SM_STATE;

unsigned char whichLight = 0;
unsigned char afterThree = 0;

void Tick_LoHi() {
    switch(SM_STATE) {
       case SM1_SMStart:
          if ((~PINA & 0x04) && !(~PINA & 0x02) && !(~PINA & 0x01))
          {
             SM_STATE =  SM_ThirdButton;
          }
          else if ((~PINA & 0x02) && !(~PINA & 0x04) && !(~PINA & 0x01))
          {
             SM_STATE = SM_SecondButton;
          }
          else if ((~PINA & 0x01) && !(~PINA & 0x04) && !(~PINA & 0x02))
          {
             SM_STATE = SM_FirstButton;
          }
          else
          {
         SM_STATE = SM1_SMStart;
          }
          break;
       case SM_FirstButton:
          if ((~PINA & 0x01) && !(~PINA & 0x04) && !(~PINA & 0x02)) {
         SM_STATE = SM_FirstButton;
          }
          else {
             SM_STATE = SM1_SMStart;
          }
          break;
       case SM_SecondButton:
          if ((~PINA & 0x02) && !(~PINA & 0x04) && !(~PINA & 0x01)) {
         SM_STATE = SM_SecondButton;
          }
          else {
             SM_STATE = SM1_SMStart;
          }
          break;
       case SM_ThirdButton:
          if ((~PINA & 0x04) && !(~PINA & 0x02) && !(~PINA & 0x01)) {
         SM_STATE = SM_ThirdButton;
          }
          else {
             SM_STATE = SM1_SMStart;
          }
          break;
       default:
          SM_STATE = SM1_SMStart;
          break;
    }

    switch(SM_STATE) {
       case SM1_SMStart:
          set_PWM(0);
          break;
       case SM_FirstButton:
          set_PWM(261.63);
          break;
       case SM_SecondButton:
          set_PWM(293.66);
          break;
       case SM_ThirdButton:
          set_PWM(329.63);
          break;
       default:
          break;
    }
}

void main() {

    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;

    TimerSet(200);
    TimerOn();
    
    PWM_on();

    SM_STATE = SM1_SMStart;

    while(1) {
        Tick_LoHi();
        while (!TimerFlag);
        TimerFlag = 0;
    }
}