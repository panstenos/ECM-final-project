// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "dc_motor.h"
#include "timers.h"
#include "interrupts.h"
#include "color.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    Timer0_init();
    Interrupts_init();
    initDCmotorsPWM(99);
    unsigned int PWMcycle = 99;
    color_click_init();
        
    ANSELFbits.ANSELF2 = 0; //turn off analogue input on pin  
    TRISFbits.TRISF2 = 1; // set F2 to input
    struct DC_motor motorL, motorR; // declare two motor structures
    motorL.power=0; //zero power to start
    motorL.direction=1; //set default motor direction
    motorL.brakemode=1; //brake mode (slow decay)
    motorL.posDutyHighByte=(unsigned char *)(&CCPR1H); //store address of CCP1 duty high byte
    motorL.negDutyHighByte=(unsigned char *)(&CCPR2H); //store address of CCP2 duty high byte
    motorL.PWMperiod=PWMcycle; //store PWMperiod for motor (value of T2PR in this case)
    
    motorR.power=0; //zero power to start
    motorR.direction=1; //set default motor direction
    motorR.brakemode=1; //brake mode (slow decay)
    motorR.posDutyHighByte=(unsigned char *)(&CCPR3H); //store address of CCP1 duty high byte
    motorR.negDutyHighByte=(unsigned char *)(&CCPR4H); //store address of CCP2 duty high byte
    motorR.PWMperiod=PWMcycle; //store PWMperiod for motor (value of T2PR in this case)
    
    fullSpeedAhead(&motorL,&motorR); //start by moving ahead
    while(1){
        
        if(get_wall_presence() == 1){
            add_seconds_to_list(); // add seconds of moving ahead to the list
            stop(&motorL,&motorR); // stop and add seconds movement to the list
            unsigned int color_code = get_color_code(); // get the color code
            moveBack(&motorL, &motorR, 10); // move back to the centre of the block 
            RobotMovement(color_code, &motorL, &motorR); // move according to the colour
            stop(&motorL,&motorR); // stop and add seconds movement to the list
            fullSpeedAhead(&motorL,&motorR); //move ahead
        }
    }    
}

// fetching test

// fetching test