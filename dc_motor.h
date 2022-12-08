#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

//function prototypes
void initDCmotorsPWM(int PWMperiod); // function to setup PWM
void setMotorPWM(DC_motor *m);
void stop(DC_motor *mL, DC_motor *mR);
void turnLeft(DC_motor *mL, DC_motor *mR);
void turnRight(DC_motor *mL, DC_motor *mR);
void turnLeftLong(DC_motor *mL, DC_motor *mR);
void turnRightLong(DC_motor *mL, DC_motor *mR);
void fullSpeedAhead(DC_motor *mL, DC_motor *mR);
void TimedfullSpeedAhead(DC_motor *mL, DC_motor *mR, unsigned int);
void moveBack(DC_motor *mL, DC_motor *mR, unsigned int);
void Calibrate(DC_motor *mL, DC_motor *mR);
void RobotMovement(unsigned int, struct DC_motor *motorL, struct DC_motor *motorR);
void increment_seconds(void);
void add_seconds_to_list(void);
void return_back(struct DC_motor *motorL, struct DC_motor *motorR);
#endif
