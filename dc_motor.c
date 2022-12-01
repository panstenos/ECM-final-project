#include <xc.h>
#include "dc_motor.h"

int seconds = 0;

// function initialise T2 and CCP for DC motor control
void initDCmotorsPWM(int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    TRISEbits.TRISE2=0; //output on RE2
    TRISCbits.TRISC7=0; //output on RC7
    TRISGbits.TRISG6=0; //output on RG6
    TRISEbits.TRISE4=0; //output on RE4
    
    LATEbits.LATE2=0; // set output of RE2 to 0
    LATCbits.LATC7=0; // set output of RC7 to 0
    LATGbits.LATG6=0; // set output of RG6 to 0
    LATEbits.LATE4=0; // set output of RE4 to 0
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS=0b100; // 1:16 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/16 -1 = 99
    T2PR=99; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
}

// function to set CCP PWM output from the values in the motor structure
void setMotorPWM(DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time
		negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

//function to stop the robot gradually 
void stop(struct DC_motor *mL, struct DC_motor *mR)
{
    (*mL).power=0;
    (*mR).power=0;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

//function to make the robot turn left 
void turnLeft(struct DC_motor *mL, struct DC_motor *mR)
{
    /* Small surface roughness turning
    int i;
    for(i=20;i<30;i+=5){ //increase power from 20 to 30 in steps of 5
    (*mL).direction=0;
    (*mR).direction=1;
    (*mL).power=i;   
    (*mR).power=i;
    setMotorPWM(mL);
    setMotorPWM(mR); 
    __delay_ms(115); //turn at the specified power for 135ms
    }
    for(i=20;i>5;i-=5){ //decrease power from 25 to 10 in steps of 5
    (*mL).direction=0;
    (*mR).direction=1;
    (*mL).power=i;   
    (*mR).power=i;
    setMotorPWM(mL);
    setMotorPWM(mR); 
    __delay_ms(135); //turn at the specified power for 160ms
    if(i==10){__delay_ms(350);} //turn with 10 power for another 120ms
    }
    */
    
    /* High surface roughness turning */
    int i;
    for(i=20;i<30;i+=5){ //increase power from 20 to 30 in steps of 5
    (*mL).direction=0;
    (*mR).direction=1;
    (*mL).power=i;   
    (*mR).power=i;
    setMotorPWM(mL);
    setMotorPWM(mR); 
    __delay_ms(137); //turn at the specified power for 135ms
    }
    for(i=20;i>5;i-=5){ //decrease power from 25 to 10 in steps of 5
    (*mL).direction=0;
    (*mR).direction=1;
    (*mL).power=i;   
    (*mR).power=i;
    setMotorPWM(mL);
    setMotorPWM(mR); 
    __delay_ms(146); //turn at the specified power for 160ms
    if(i==10){__delay_ms(350);} //turn with 10 power for another 120ms
    }
    // */
}
//function to make the robot turn right 
void turnRight(struct DC_motor *mL, struct DC_motor *mR)
{
    /*  Small surface roughness turning 
    int i;
    for(i=20;i<30;i+=5){ //increase power from 20 to 30 in steps of 5
    (*mL).direction=1;
    (*mR).direction=0;
    (*mL).power=i;   
    (*mR).power=i;
    setMotorPWM(mL);
    setMotorPWM(mR); 
    __delay_ms(115); //turn at the specified power for 135ms
    }
    for(i=20;i>5;i-=5){ //decrease power from 25 to 10 in steps of 5
    (*mL).direction=1;
    (*mR).direction=0;
    (*mL).power=i;   
    (*mR).power=i;
    setMotorPWM(mL);
    setMotorPWM(mR); 
    __delay_ms(135); //turn at the specified power for 160ms
    if(i==10){__delay_ms(350);} //turn with 10 power for another 120ms
    }
    */
    
    /*  High surface roughness turning */
    int i;
    for(i=25;i<40;i+=5){ //increase power from 20 to 30 in steps of 5
    (*mL).direction=1;
    (*mR).direction=0;
    (*mL).power=i;   
    (*mR).power=i;
    setMotorPWM(mL);
    setMotorPWM(mR); 
    __delay_ms(107); //turn at the specified power for 135ms
    }
    for(i=30;i>15;i-=5){ //decrease power from 25 to 10 in steps of 5
    (*mL).direction=1;
    (*mR).direction=0;
    (*mL).power=i;   
    (*mR).power=i;
    setMotorPWM(mL);
    setMotorPWM(mR); 
    __delay_ms(105); //turn at the specified power for 160ms
    if(i==10){__delay_ms(150);} //turn with 10 power for another 120ms
    }
}

//function to make the robot go straight
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR)
{
    (*mL).direction=1;
    (*mR).direction=1;
    (*mL).power=50;
    (*mR).power=50;
    setMotorPWM(mL);
    setMotorPWM(mR);
    __delay_ms(500);
}
void TimedfullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR, unsigned int time)
{
    seconds = 0;
    while (seconds<time)
    {
    (*mL).direction=1;
    (*mR).direction=1;
    (*mL).power=25;
    (*mR).power=22;
    setMotorPWM(mL);
    setMotorPWM(mR);
    }
}
void increment_seconds()
{ //main counter function
    seconds += 1; // increment by the second
}