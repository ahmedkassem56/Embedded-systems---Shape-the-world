// Sound.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Use the SysTick timer to request interrupts at a particular period.
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// This routine calls the 4-bit DAC

#include "Sound.h"
#include "DAC.h"
#include "..//tm4c123gh6pm.h"

const unsigned char SineWave[16] = {8,10,12,15,15,15,12,10,8,
                                    6,3,1,1,1,3,6}; 
unsigned char Index=0;
unsigned char stop = 0;
// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
	DAC_Init();
  NVIC_ST_CTRL_R = 0; // disables systick
	NVIC_ST_CURRENT_R = 0; // clear current register
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // set priority 
	NVIC_ST_CTRL_R = 0x07; // start SysTick timer with interrupt enabled
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	NVIC_ST_RELOAD_R = period-1; // set reload value
	NVIC_ST_CURRENT_R = 0; // clear current register
	NVIC_ST_CTRL_R = 0x07; // start SysTick timer with interrupt enabled
	stop = 0;
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	//NVIC_ST_CTRL_R = 0;
	DAC_Out(0);
	stop = 1;
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	if (!stop)
   DAC_Out(SineWave[(Index++)&0x0F]); 
}
