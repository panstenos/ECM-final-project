#include <xc.h>
#include "interrupts.h"
#include "dc_motor.h"

//extern int seconds;
/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{   
    INTCONbits.PEIE = 1;    //turns on all the peripheral interrupts
    PIE0bits.TMR0IE = 1;    // enable Timer0 interrupt
    PIE2bits.C1IE = 1;    //enable the comparator interrupt bit
    INTCONbits.GIE = 1; 	//turn on interrupts globally (when this is off, all interrupts are deactivated)    
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{   
    if(PIR0bits.TMR0IF == 1){//check the interrupt flag
    increment_seconds(); //call the function to increment the seconds
    // set the timer to reset at 38375 every time the it overflows to measure 1/10 of a second
    TMR0H=0b11100111;            
    TMR0L=0b10010101;

    PIR0bits.TMR0IF = 0; // turn flag off
	}
}