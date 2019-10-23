/*	Author: akim106
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 6  Exercise 2
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

enum Timer_States{LED_1, LED_2, LED_3, Held_1, Wait, Held_2} Timer_State; //Enumerating states
unsigned char Output;
unsigned char B1;
const long ms300 = 30;
long counter = 0;

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
	B1 = ~PINA & 0x01; // Getting Input
    // State Transitions
    switch(Timer_State) {
      case LED_1:
	    if (B1) {
	   	   Timer_State = Held_1;
		   break;
		} else if (counter < ms300) {
		   Timer_State = LED_1;
		   counter++;
		   break;
		} else {
           Timer_State = LED_2;
		   counter = 0;
           break;
		}
      case LED_2:
	    if (B1) {
	       Timer_State = Held_1;
		   break;
	    } else if (counter < ms300) {
	       Timer_State = LED_2;
	       counter++;
		   break;
	    } else {
		   Timer_State = LED_3;
		   counter = 0;
		   break;
		}
      case LED_3:
	    if (B1) {
		    Timer_State = Held_1;
		    break;
		} else if (counter < ms300){
		    Timer_State = LED_3;
			counter++;
		    break;
	    } else {
			Timer_State = LED_1;
			counter = 0;
			break;
		}
	  case Held_1:
	    if (B1) {
			Timer_State = Held_1;
			break;
		} else {
			Timer_State = Wait;
			break;
		}
	  case Wait:
	    if (B1) {
			Timer_State = Held_2;
			break;
		} else {
			Timer_State = Wait;
			break;
		}
	  case Held_2:
	    if (B1) {
			Timer_State = Held_2;
			break;
		} else {
			Timer_State = LED_1;
			counter = 0;
			break;
		}
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
	  case Held_1:
	    break;
	  case Wait:
	    break;
	  case Held_2:
	    break;
    } 
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; // PORTA = Inputs
    DDRB = 0xFF; PORTB = 0x00; // PORTB = Outputs
    Timer_State = LED_3; // Setting initial state   
    TimerSet(10); // Initialize the timer with period 1000 ms
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
