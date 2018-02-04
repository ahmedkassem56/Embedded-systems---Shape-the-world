// TuningFork.c Lab 12
// Runs on LM4F120/TM4C123
// Use SysTick interrupts to create a squarewave at 440Hz.  
// There is a positive logic switch connected to PA3, PB3, or PE3.
// There is an output on PA2, PB2, or PE2. The output is 
//   connected to headphones through a 1k resistor.
// The volume-limiting resistor can be any value from 680 to 2000 ohms
// The tone is initially off, when the switch goes from
// not touched to touched, the tone toggles on/off.
//                   |---------|               |---------|     
// Switch   ---------|         |---------------|         |------
//
//                    |-| |-| |-| |-| |-| |-| |-|
// Tone     ----------| |-| |-| |-| |-| |-| |-| |---------------
//
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */


#include "TExaS.h"
#include "..//tm4c123gh6pm.h"
#define SYSTICK_RELOAD 90908

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode


unsigned long prev_button = 0;
unsigned long tune_running = 0;

// input from PA3, output from PA2, SysTick interrupts
void Sound_Init(void){ unsigned long delay;
	// PORT A Initialization
	SYSCTL_RCGC2_R |= (1 << 0); // turn on clock for port A
	delay = SYSCTL_RCGC2_R; // wait for the clock to initialize 
	GPIO_PORTA_DIR_R |= (1 << 2); // set PA2 to output (PA3 is input by default)
	GPIO_PORTA_DEN_R |= (1 << 2) | (1 << 3); // enable digital for PA2,PA3
	GPIO_PORTA_AFSEL_R &= ~0x0C; // alternative function disable 
	GPIO_PORTA_AMSEL_R &= ~0x0C; // analog disable
	GPIO_PORTA_PCTL_R &= ~0x0000FF00; // configure as GPIO
	GPIO_PORTA_PDR_R |= (1 << 3); // enable pull down resistor on PA3
	GPIO_PORTA_DATA_R &= ~(1 << 2); // clear bit 2 (stop the tune at startup)
	// SysTick Init
	NVIC_ST_CTRL_R = 0x00; // disable systick during setup
	NVIC_ST_RELOAD_R = SYSTICK_RELOAD; // set reload value
	NVIC_ST_CURRENT_R = 0; // clearing current register
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // set priority 2 
	NVIC_ST_CTRL_R = 0x07; // enable systick with interrupt
}

// called at 880 Hz
void SysTick_Handler(void){
	unsigned long button = GPIO_PORTA_DATA_R&(1<<3); // read switch at bit 3
	if (button && !prev_button) {
		if (!tune_running) {
			tune_running = 1;
		}
		else {
			GPIO_PORTA_DATA_R &= ~(1 << 2); // stop tune
			tune_running = 0;
		}
	}
	prev_button = button;
	if (tune_running) 
		GPIO_PORTA_DATA_R ^= (1 << 2); // toggle bit 2
}

int main(void){// activate grader and set system clock to 80 MHz
  TExaS_Init(SW_PIN_PA3, HEADPHONE_PIN_PA2,ScopeOn); 
  Sound_Init();         
  EnableInterrupts();   // enable after all initialization are done
  while(1){
    // main program is free to perform other tasks
    // do not use WaitForInterrupt() here, it may cause the TExaS to crash
  }
}
