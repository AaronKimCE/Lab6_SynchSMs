/*	Author: akim106
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0

// Internal variables mapping AVR's ISR to our clean ISR
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum Timer_States{LED_1, LED_2, LED_3} Timer_State; //Enumerating states
unsigned char Output;

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

//C programmer will use our ISR rather than this one
ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if (_avr_timer_cntcurr == 0) {
      TimerISR();
      _avr_timer_cntcurr = _avr_timer_M;
    }
}

//Sets TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

void TickSM() {
    // State Transitions
    switch(Timer_State) {
      case LED_1:
        Timer_State = LED_2;
        break;
      case LED_2:
        Timer_State = LED_3;
        break;
      case LED_3:
        Timer_State = LED_1;
        break;
    }
    // State Actions
    switch(Timer_State) {
      case LED_1:
        Output = 0x01;
        break;
      case LED_2:
        Output = 0x02;
        break;
      case LED_3:
        Output = 0x04;
        break;
    } 
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // PORTA = Inputs
    DDRB = 0xFF; PORTB = 0x00; // PORTB = Outputs
    Timer_State = LED_1; // Setting initial state   
    TimerSet(1000); // Initialize the timer with period 1000 ms
    TimerOn();

    // Execute SM
    while (1) {
      TickSM();
      PORTB = Output;
      while (!TimerFlag);
      TimerFlag = 0;
    }
    return 1;
}
