#include <xc.h>
#include "dc_motor.h"

int seconds = 0; // 0.1 second counter from the Timer0
int movement_list[100]; //tracks the movement of the buggy
int index = 0; //current index of the movement list
int state = 0; // 0 for going to the white; 1 for returning to origin

int blockahead = 16;
int blockback = 16;
int halfblock = 5;
int twothirds = 11;
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

//stop function
void stop(struct DC_motor *mL, struct DC_motor *mR)
{
    (*mL).power=0; //power down the motors
    (*mR).power=0;
    setMotorPWM(mL);
    setMotorPWM(mR);
    __delay_ms(500);
}

//turn left 
void turnLeft(DC_motor *mL, DC_motor *mR)
{
    int k = 0;
    for (k=0;k<2;k++) //repeats 2 45 degree turning
    {
        int i;
        for(i=41;i<56;i+=5){ //gradually increase the power
        (*mL).direction=1; //backward direction
        (*mR).direction=0; //forward direction
        (*mL).power=i;   
        (*mR).power=i;
        setMotorPWM(mL);
        setMotorPWM(mR); 
        __delay_ms(58); //turn at the specified power for 58ms
        }
        for(i=46;i>31;i-=5){ //gradually decrease the power
        (*mL).direction=1;
        (*mR).direction=0;
        (*mL).power=i;   
        (*mR).power=i;
        setMotorPWM(mL);
        setMotorPWM(mR); 
        __delay_ms(57); //turn at the specified power for 57ms
        }
    }
}
//turn right function
void turnRight(struct DC_motor *mL, struct DC_motor *mR)
{
    int k;
    for (k=0;k<2;k++) //repeats 2 45 degree turning
    {
        int i;
        for(i=23;i<38;i+=5){ //gradually increase the power
        (*mL).direction=0;
        (*mR).direction=1;
        (*mL).power=i;   
        (*mR).power=i;
        setMotorPWM(mL);
        setMotorPWM(mR); 
        __delay_ms(107); //turn at the specified power for 107ms
        }
        for(i=28;i>13;i-=5){ //gradually decrease the power
        (*mL).direction=0;
        (*mR).direction=1;
        (*mL).power=i;   
        (*mR).power=i;
        setMotorPWM(mL);
        setMotorPWM(mR); 
        __delay_ms(105); //turn at the specified power for 105ms
        }
    }
}

void turnLeftLong(struct DC_motor *mL, struct DC_motor *mR)
{ 
    int k = 0;
    for (k=0;k<3;k++) //repeats 3 45 degree turning
    {
        int i;
        for(i=41;i<56;i+=5){
        (*mL).direction=1;
        (*mR).direction=0;
        (*mL).power=i;   
        (*mR).power=i;
        setMotorPWM(mL);
        setMotorPWM(mR); 
        __delay_ms(58); //turn at the specified power for 58ms
        }
        for(i=46;i>31;i-=5){
        (*mL).direction=1;
        (*mR).direction=0;
        (*mL).power=i;   
        (*mR).power=i;
        setMotorPWM(mL);
        setMotorPWM(mR); 
        __delay_ms(57); //turn at the specified power for 57ms
        }
    }
}

void turnRightLong(struct DC_motor *mL, struct DC_motor *mR)
{  
    int k;
    for (k=0;k<3;k++) 
    {
        int i;
        for(i=23;i<38;i+=5){
        (*mL).direction=0;
        (*mR).direction=1;
        (*mL).power=i;   
        (*mR).power=i;
        setMotorPWM(mL);
        setMotorPWM(mR); 
        __delay_ms(107); //turn at the specified power for 107ms
        }
        for(i=28;i>13;i-=5){
        (*mL).direction=0;
        (*mR).direction=1;
        (*mL).power=i;   
        (*mR).power=i;
        setMotorPWM(mL);
        setMotorPWM(mR); 
        __delay_ms(105); //turn at the specified power for 105ms
        }
    }
}
//function to make the robot go straight
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR)
{
    seconds = 0; // reset the timer
    (*mL).direction=0;
    (*mR).direction=0;
    (*mL).power=68;
    (*mR).power=68;
    setMotorPWM(mL);
    setMotorPWM(mR);
}
// function to move straight for a specified time period
void TimedfullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR, unsigned int time)
{
    seconds = 0;
    while (seconds<time)
    {
        // used for returning back to the entrance
        (*mL).direction=0;
        (*mR).direction=0;
        (*mL).power=68;
        (*mR).power=68;
        setMotorPWM(mL);
        setMotorPWM(mR);
        }
}

// function to move back for a specified time period
void moveBack(struct DC_motor *mL, struct DC_motor *mR, unsigned int time)
{
    seconds = 0; // reset the seconds timer
    while (seconds<time) // run as much as ordered
    {
        (*mL).direction=1;
        (*mR).direction=1;
        (*mL).power=70;
        (*mR).power=68; 
        setMotorPWM(mL);
        setMotorPWM(mR);
    }

}

void increment_seconds()
{ //main counter function
    seconds += 1; // increment by the second
}

// color 0-8 detecting color
void RobotMovement(unsigned int color, DC_motor *motorL, DC_motor *motorR)
{
    moveBack(motorL, motorR, halfblock); // move back to the centre of the block 
    stop(motorL,motorR); //built in delay 1 s
    
    //RED + R90     r   -1
    if(color == 0){
        turnRight(motorL, motorR);
        movement_list[index] = -1; // trace that step and add to the list 
        index++;
    }
    //GRE + L90     l   -2
    if(color == 1){
        turnLeft(motorL, motorR);
        movement_list[index] = -2; // trace that step and add to the list 
        index++;
    }
    //BLU + 180     u   -1 x2
    if(color == 2){
        int i;
        for (i=0;i<2;i++){
            turnRight(motorL, motorR);
            movement_list[index] = -1; // trace that step and add to the list 
            index++;
        }
    }
    //YEL + B1R90   -1 , r *
    if(color == 3){
        //move back + turn right
        moveBack(motorL, motorR, blockback); // move back a block
        stop(motorL, motorR);
        turnRight(motorL, motorR);
        //if the last element of the list is integer greater or equal to a block
        if (movement_list[index-1] > halfblock)
        {
        //  subtract 1 block 
            movement_list[index-1] -= twothirds;
        //  add r to the list
            movement_list[index] = -1;
            index += 1;
        }else{
            //if the last element of the list is r
            if (movement_list[index-1] == -1)
            {
            //  change it to l
                movement_list[index-1] = -2;
            }
            //if the last element of the list is l
            else if (movement_list[index-1] == -2)
            {
            //  change it to r
                movement_list[index-1] = -1;
            }
            //if the last element of the list is long r
            else if (movement_list[index-1] == -3)
            {
            //  change it to long l
                movement_list[index-1] = -4;
            }            
            //if the last element of the list is long l
            else if (movement_list[index-1] == -4)
            {
            //  change it to long r
                movement_list[index-1] = -3;
            }            
            //  add 0l to the list
            movement_list[index] = blockahead; // add a block's length    
            index += 1;
            //  add l to the list
            movement_list[index] = -2;
            index += 1;
        }
    }
    //PIN + B1L90   -1 , l *
    
    if(color == 4){
        //move back + turn left
        moveBack(motorL, motorR, blockback); // move back a block
        stop(motorL, motorR);
        turnLeft(motorL, motorR);        
        //if the last element of the list is integer greater or equal to a block
        if (movement_list[index-1] > halfblock)
        {
        //  subtract 01 block 
            movement_list[index-1] -= twothirds;
        //  add l to the list
            movement_list[index] = -2;
            index += 1;
        }else{
            //if the last element of the list is r
            if (movement_list[index-1] == -1)
            {
            //  change it to l
                movement_list[index-1] = -2;
            }
            //if the last element of the list is l
            else if (movement_list[index-1] == -2)
            {
            //  change it to r
                movement_list[index-1] = -1;
            }
            //if the last element of the list is long r
            else if (movement_list[index-1] == -3)
            {
            //  change it to long l
                movement_list[index-1] = -4;
            }            
            //if the last element of the list is long l
            else if (movement_list[index-1] == -4)
            {
            //  change it to long r
                movement_list[index-1] = -3;
            } 
        //  add 0l to the list
        movement_list[index] = blockahead; // add a block's length    
        index += 1;
        //  add r to the list
        movement_list[index] = -1;
        index += 1;
        } 
    }
    //ORA + R135    R
    if(color == 5){
        turnRightLong(motorL, motorR);
        movement_list[index] = -3;
        index += 1;
    }
    //LIB + L135    L
    if(color == 6){
        turnLeftLong(motorL, motorR);
        movement_list[index] = -4;
        index += 1;
    }    
    //WHITE
    if(color == 7){
        int i;
        for (i=0;i<2;i++)
        {
            turnRight(motorL, motorR); // u-turn
        }
        // change the state to 1
        state = 1;
    }
    // BLACK OR NO COLOR
    if (color > 7)
    {
        TimedfullSpeedAhead(motorL, motorR, halfblock); // move ahead to get a better look at the color
    }
    
}

int get_state(void)
{
    return state; // return the state
}


void add_seconds_to_list(void)
{
    if (seconds > twothirds) // set some threshold here for what is considered to be a block 
    {
        movement_list[index] = seconds - halfblock; //import the seconds to the list remove half block and add to the list 
        index++; // increase the index value for the next value
    }
}

void return_back(struct DC_motor *motorL, struct DC_motor *motorR)
{
    while (index > 0){
        if (movement_list[index-1] == -1){turnLeft(motorL, motorR);}
        else if (movement_list[index-1] == -2){turnRight(motorL, motorR);}
        else if (movement_list[index-1] == -3){turnLeftLong(motorL, motorR);}
        else if (movement_list[index-1] == -4){turnRightLong(motorL, motorR);}
        else if (movement_list[index-1] > 0 || movement_list[index-2] > -3) // when moving forward and next move is a short turn
        {
            TimedfullSpeedAhead(motorL, motorR, movement_list[index-1]); //move ahead
            __delay_ms(200);
            TimedfullSpeedAhead(motorL, motorR, halfblock);
            moveBack(motorL, motorR, halfblock);
        }
        stop(motorL,motorR); // stop and add seconds movement to the list
        index -= 1;
    }
    if (index == 0){state = 2;}
}