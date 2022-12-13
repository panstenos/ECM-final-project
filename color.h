#ifndef _color_H
#define _color_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

unsigned int get_color_code(void);
unsigned int get_wall_detection(void);
void set_wall_detection(unsigned int);

void set_led_color(unsigned int);
void calibrate_white(void);
void calibrate_black(void);

#endif
