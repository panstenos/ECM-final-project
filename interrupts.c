#include <xc.h>
#include "interrupts.h"
#include "dc_motor.h"
#include "color.h"

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
unsigned int incr_sec_counter = 0; //Counter for the increment_second function
unsigned int wall_detection_counter = 0; //Counter for the wall_detection function
void __interrupt(high_priority) HighISR()
{   
    if(PIR0bits.TMR0IF == 1){//check the interrupt flag
        incr_sec_counter += 1;
        wall_detection_counter += 1;
       
        if(incr_sec_counter == 10){ //Trigger every 0.1 sec
            increment_seconds(); //call the function to increment the seconds in dc_motors.C
            incr_sec_counter = 0;
        }
        
        if(wall_detection_counter == 22){//Trigger every 440ms
            set_wall_detection(0); //Setup the clear1 value in color.C
        }
        else if(wall_detection_counter == 44){//Trigger every 440ms
            set_wall_detection(1); //Setup the clear2 value in color.C
            wall_detection_counter = 0;
        }
        // set the timer to reset at 64912 every time the it overflows to measure 1/100 of a second
        TMR0H=0b11111101;
        TMR0L=0b10010000;

        PIR0bits.TMR0IF = 0; // turn flag off
	}
}