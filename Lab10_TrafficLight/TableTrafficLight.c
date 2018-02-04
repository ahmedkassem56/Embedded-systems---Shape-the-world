


// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
struct State {
  unsigned long OutB;  // output for Port B
  unsigned long OutF;  // output for Port F
  unsigned long Time;  // duration of the state
  unsigned long Next[8];};  // Next state
typedef const struct State SType;

#define goWest 0
#define waitWest 1
#define goSouth 2
#define waitSouth 3
#define walk 4
#define flash1 5
#define flash2 6
#define flash3 7
#define flash4 8
#define alloff 9
	
/*SType FSM[10] = {			// 000		001		010		011		100		101		110		111
	{ 0x0C,0x02,100, 		{ goWest,goWest,waitWest,waitWest,waitWest,waitWest,waitWest,waitWest } },   // goWest
	{ 0x14,0x02,50,			{ goSouth,goSouth,goSouth,goSouth,walk,goSouth,goSouth,goSouth } },									 // waitWest
	{ 0x21,0x02,100,		{ goSouth,waitSouth,goSouth,3,3,3,3,3} }, // waitSouth == 3									 // goSouth
	{ 0x22,0x02,50,			{ goWest,goWest,goWest,goWest,walk,walk,walk,walk } },										 	 // waitSouth
	{ 0x24,0x08,100,    { walk,flash1,flash1,flash1,walk,flash1,flash1,flash1 } },									 // walk
	{ 0x24,0x02,30,			{ 6,6,6,6,6,6,6,6 } }, // flash2 == 6																				 // flash1
	{ 0x24,0x00,30,			{ 7,7,7,7,7,7,7,7 } }, // flash3 == 7																				 // flash2
	{ 0x24,0x02,30,			{ 8,8,8,8,8,8,8,8 } }, // flash4 == 8																				 // flash3
	{ 0x24,0x00,30,			{ alloff,goWest,goSouth,goWest,walk,goWest,goSouth,goWest } },               // flash4
	{ 0x00,0x00,50,     { alloff,goWest,goSouth,goWest,walk,goWest,goSouth,goWest } }								 // alloff
};*/

SType FSM[10] = {			// 000		001		010		011		100		101		110		111
	{ 0x0C,0x02,100, 		{ goWest,goWest,waitWest,waitWest,waitWest,waitWest,waitWest,waitWest } },   // goWest
	{ 0x14,0x02,50,			{ goSouth,goSouth,goSouth,goSouth,walk,walk,goSouth,goSouth } },									 // waitWest
	{ 0x21,0x02,100,		{ goSouth,waitSouth,goSouth,waitSouth,waitSouth,waitSouth,waitSouth,waitSouth} }, // goSouth
	{ 0x22,0x02,50,			{ goWest,goWest,goWest,goWest,walk,walk,walk,walk } },										 	 // waitSouth
	{ 0x24,0x08,100,    { walk,flash1,flash1,flash1,walk,flash1,flash1,flash1 } },									 // walk
	{ 0x24,0x02,30,			{ 6,6,6,6,6,6,6,6 } }, // flash2 == 6																				 // flash1
	{ 0x24,0x00,30,			{ 7,7,7,7,7,7,7,7 } }, // flash3 == 7																				 // flash2
	{ 0x24,0x02,30,			{ 8,8,8,8,8,8,8,8 } }, // flash4 == 8																				 // flash3
	{ 0x24,0x00,30,			{ alloff,goWest,goSouth,goWest,walk,goWest,goSouth,goWest } },               // flash4
	{ 0x00,0x00,50,     { alloff,goWest,goSouth,goWest,walk,goWest,goSouth,goWest } }								 // alloff
};

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortInit(void);
unsigned long getInput(void);
void setOutput(unsigned long portB,unsigned long portF);
void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay);
unsigned long s;

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	SysTick_Init();
	PortInit();
  EnableInterrupts();
	s = alloff; // initial state
  while(1){
    setOutput(FSM[s].OutB,FSM[s].OutF); // This function will set the ports according to the parameters
		SysTick_Wait10ms(FSM[s].Time);
		s = FSM[s].Next[getInput()];  // returns PortE bits 0,1,2 
  }
}


unsigned long getInput(void) {
		return GPIO_PORTE_DATA_R & 0x07; 
}

void setOutput(unsigned long portB,unsigned long portF) {
	GPIO_PORTB_DATA_R = portB;
	GPIO_PORTF_DATA_R = portF;
}

void PortInit(void)
{
	unsigned long delay;
	SYSCTL_RCGC2_R |= 0x32; // enable B, E, F clock
	delay = SYSCTL_RCGC2_R; // wait for clock initialization 
	// PORT F
  GPIO_PORTF_AMSEL_R &= ~0x0A;        // disable analog function
  GPIO_PORTF_PCTL_R &= ~0x0000F0F0;   // GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R |= 0x0A;          // PF3,PF1 output   
  GPIO_PORTF_AFSEL_R &= ~0x0A;        // no alternate function
  GPIO_PORTF_DEN_R |= 0x0A;          // enable digital pins PF1-3   
  // PORT B  
  GPIO_PORTB_AMSEL_R &= ~0x3F; 			// disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    			// outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; 			// regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    			// enable digital on PB5-0  
	// PORT E
	GPIO_PORTE_AMSEL_R &= ~0x07;			// disable analog function on PE0-2
	GPIO_PORTE_PCTL_R &= ~0x00000FFF; // regular GPIO
	GPIO_PORTE_DIR_R &= ~0x07;				// input
	GPIO_PORTE_AFSEL_R &= ~0x07; 			// no alternate function
	GPIO_PORTE_DEN_R |= 0x07;					// enable digitap pins PE0-2;
}

#define NVIC_ST_CTRL_R      (*((volatile unsigned long *)0xE000E010))
#define NVIC_ST_RELOAD_R    (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R   (*((volatile unsigned long *)0xE000E018))
	
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}


void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}

void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}
