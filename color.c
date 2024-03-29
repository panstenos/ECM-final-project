#include <xc.h>
#include "color.h"
#include "i2c.h"

unsigned int wall_detection_mode = 0; // 0 : WallDetection; 1 : no Wall detection
//definition of RGB structure
struct RGB_val {
	unsigned long R;
	unsigned long G;
	unsigned long B;
};

struct RGB_val RGB;

static struct RGB_val Red_rule = {72,2,2};
static struct RGB_val Green_rule = {25,54,8};
static struct RGB_val Blue_rule = {3,9,36};
static struct RGB_val Yellow_rule = {93,76,20};
static struct RGB_val Pink_rule = {77,60,64};
static struct RGB_val Orange_rule = {92,39,28};
static struct RGB_val Lightblue_rule = {38,72,84};
static struct RGB_val White_rule = {100,100,100};
static struct RGB_val Black_rule = {0,0,0};

static struct RGB_val Color_rules[9];
struct RGB_val White_setup = {10492,6997,1904};
struct RGB_val Black_setup = {1028,587,161};
unsigned int wall_coef = 15;

unsigned int clear1 = 0;
unsigned int clear2 = 0;

void set_wall_detection_mode(unsigned int mode){ // Set the wall detection mode and reset clear values
    wall_detection_mode = mode;
    clear1 = 0;
    clear2 = 0;
}

void color_click_init(void) // Initialise all the registers and variables
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
    
    TRISFbits.TRISF7 = 0;
    TRISGbits.TRISG1 = 0;
    TRISAbits.TRISA4 = 0;
    
    Color_rules[0] = Red_rule; //Initialise all the colors into the color list
    Color_rules[1] = Green_rule;
    Color_rules[2] = Blue_rule;
    Color_rules[3] = Yellow_rule;
    Color_rules[4] = Pink_rule;
    Color_rules[5] = Orange_rule;
    Color_rules[6] = Lightblue_rule;
    Color_rules[7] = White_rule;
    Color_rules[8] = Black_rule;
    
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin
    
    //Calibration code
    while (PORTFbits.RF3 && PORTFbits.RF2); //Wait until the button F3 or F2 is pushed
    if(!PORTFbits.RF2){ // Skip calibration if F2 is pushed
        return;
    }
    calibrate_white();     //Calibrate white value
    while (PORTFbits.RF3); //Wait until the button F3 is pushed
    calibrate_black();     // Calibrate black values
    while (PORTFbits.RF3); //Wait until the button F3 is pushed

}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int get_color_distance(struct RGB_val color1, struct RGB_val color2){ // Get distance between 2 colors in R3 space (using Euclidian Norm) squared
    unsigned int R_offset = color1.R >= color2.R ? color1.R - color2.R : color2.R - color1.R;
    unsigned int G_offset = color1.G >= color2.G ? color1.G - color2.G : color2.G - color1.G;
    unsigned int B_offset = color1.B >= color2.B ? color1.B - color2.B : color2.B - color1.B;

    return R_offset*R_offset + G_offset*G_offset + B_offset*B_offset;
}

void set_led_color(unsigned int color){
    //Set the led color, the color is encoded on 3 bits,
    //the order from high bit to low bit is red, green and blue 
    
    if(color & 0b100){ //Set the red color
        LATGbits.LATG1 = 1;
    }else{
        LATGbits.LATG1 = 0;
    }
    if(color & 0b010){ //Set the green color
        LATAbits.LATA4 = 1;
    }else{
        LATAbits.LATA4 = 0;
    }
    if(color & 0b001){ //Set the blue color
        LATFbits.LATF7 = 1;
    }else{
        LATFbits.LATF7 = 0;
    }
}


unsigned int wait_time = 220; // Wait time between 2 colors measurements
unsigned int get_color_code(){ 
    // Returns an int representing the detected color
    //Red -> 0 | Green -> 1 | Blue -> 2 | Yellow -> 3 | Pink -> 4 | Orange -> 5 | Lightblue -> 6 | White -> 7 | Black -> 8
    
    set_led_color(0b100);
    __delay_ms(wait_time);
    RGB.R = color_read_Red();
    set_led_color(0b010);
    __delay_ms(wait_time);
    RGB.G = color_read_Green();
    set_led_color(0b001);
    __delay_ms(wait_time);
    RGB.B = color_read_Blue();
    set_led_color(0b000);
    
    struct RGB_val NormalizedRGB; //Normalise the colors values according the calibration
    NormalizedRGB.R = RGB.R  >= Black_setup.R ? (RGB.R - Black_setup.R)*100/(White_setup.R - Black_setup.R) : 0;
    NormalizedRGB.G = RGB.G  >= Black_setup.G ? (RGB.G - Black_setup.G)*100/(White_setup.G - Black_setup.G) : 0;
    NormalizedRGB.B = RGB.B  >= Black_setup.B ? (RGB.B - Black_setup.B)*100/(White_setup.B - Black_setup.B) : 0;
    
    unsigned int min_value = get_color_distance(NormalizedRGB,Color_rules[0]);
    unsigned int min_index = 0;
    for(unsigned int i = 1;i < 9; i++){ //Check for minimal distance between measured color and stored color values
        unsigned int value = get_color_distance(NormalizedRGB,Color_rules[i]);
        if(value < min_value){
            min_index = i;
            min_value = value;
        }
    }
    return min_index; //Return the color index


}

void calibrate_black(){ //Measure red, green and blue colors for the black color. And clear for the wall detection
    set_led_color(0b100);
    __delay_ms(wait_time);
    Black_setup.R = color_read_Red();
    set_led_color(0b010);
    __delay_ms(wait_time);
    Black_setup.G = color_read_Green();
    set_led_color(0b001);
    __delay_ms(wait_time);
    Black_setup.B = color_read_Blue();
    set_led_color(0b000);
    __delay_ms(wait_time);
    unsigned int clear1 = color_read_Clear(); //Clear value with led turned off
    set_led_color(0b111);
    __delay_ms(wait_time);
    unsigned int clear2 = color_read_Clear(); //Clear value with led turned on
    set_led_color(0b000);
    wall_coef = clear2/clear1/5; //Get the wall detection coefficient
}

void calibrate_white(){ //Measure red, green and blue colors for the white color.
    set_led_color(0b100);
    __delay_ms(wait_time);
    White_setup.R = color_read_Red();
    set_led_color(0b010);
    __delay_ms(wait_time);
    White_setup.G = color_read_Green();
    set_led_color(0b001);
    __delay_ms(wait_time);
    White_setup.B = color_read_Blue();
    set_led_color(0b000);
}



unsigned int get_wall_detection(){ // Check if their is a wall ahead
    if(clear1 == 0 || clear2 == 0){return 0;}
    return clear2 >= clear1*wall_coef; //If light intensity with a flash is way higher than without there is a wall
}

void set_wall_detection(unsigned int mode){ //Called by interrup.C
    //Set the values for clear1 and clear2
    if(wall_detection_mode == 0){
        return;
    }
    if(mode == 0){
        clear1 = color_read_Clear(); //Clear value with led turned off
        set_led_color(0b111);
    }else if(mode == 1){
        clear2 = color_read_Clear(); //Clear value with led turned on
        set_led_color(0b000);
    }
} 