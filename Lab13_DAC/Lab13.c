// Lab13.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// edX Lab 13 
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC
// Port E bits 3-0 have 4 piano keys

#include "..//tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void delay(unsigned long msec);
int main(void){ // Real Lab13 
	// for the real board grader to work 
	// you must connect PD3 to your DAC output
  TExaS_Init(SW_PIN_PE3210, DAC_PIN_PB3210,ScopeOn); // activate grader and set system clock to 80 MHz
// PortE used for piano keys, PortB used for DAC        
  Sound_Init(); // initialize SysTick timer and DAC
  Piano_Init(); // initialize piano buttons
  EnableInterrupts();  // enable after all initialization are done
	//Sound_Tone(50000);
  while(1){                
// input from keys to select tone
		static unsigned long prev_input;
		unsigned long input = Piano_In(); 
		if (input != prev_input) {
			if (input == 0) 
				Sound_Off();
			else if (input == 1)
				Sound_Tone(9555);
			else if (input == 2)
				Sound_Tone(8513);
			else if (input == 4)
				Sound_Tone(7584);
			else if (input == 8)
				Sound_Tone(6377);
		}
		prev_input = input;
		delay(25);
  }
            
}

// Inputs: Number of msec to delay
// Outputs: None
void delay(unsigned long msec){ 
  unsigned long count;
  while(msec > 0 ) {  // repeat while there are still delay
    count = 16000;    // about 1ms
    while (count > 0) { 
      count--;
    } // This while loop takes approximately 3 cycles
    msec--;
  }
}


