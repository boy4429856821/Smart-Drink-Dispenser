/*ECE230 - FINAL PROJECT - Tyler Townsend, Coleman Weaver
 * Program Description:
 * This is the C code for our final project: Smart Drink Dispenser
 * This program takes in a sequence of drink in the form of an array and dispenses
 * said drink via servo sequencing by indexing through various loops to produce the desired
 * drink. This program contains a few modes in which the machine will loop until
 * human input is added. This happens when the machine waits for user to input a 
 * 4 digit array for their drink, and when the servos are dispensing.
*/

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
//#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR21V   // Brown-out Reset Selection bit (Brown-out Reset set to 2.1V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)


#include <xc.h>
#include <pic.h>
#include "lcd4bits.h"

//create variables
int highTicks = 0; //create high and low ticks for outputting through loud speaker
int lowTicks = 0;
int line1 = 0x00; //hex value to jump to line 1
int line2 = 0x40; //hex value to jump to line 2
int debounce = 300; //good button debounce
unsigned char drink[4] = {0, 0, 0, 0}; //initialize drink array with 0's
char mode = 0; //set mode variable to 0 
char innermode = 0; //set innermode variable to 0
int l = 0; 

//Call functions to initialize them
void test_Servo();
void get_Servo1();
void get_Servo2();
void get_Servo3();
void get_Servo4();
void intialize_Everything();
void drinkDispensingSound();
void __interrupt() interrupt_handler(void);


//"Final Countdown Music Playing"
void main(void) {
    intialize_Everything(); //initializes all the pins we are using
    lcd_init(); //initialize the LCD module
    lcd_clear(); //clear LCD before starting for good habit
    
    
    //main program
    mode = 0; //set first mode to zero
    while(1){
        //only displays "Enter Drink" if mode is equal to zero
        if(mode == 0 ){
            lcd_clear();
            lcd_goto(line1);
            lcd_puts("  Enter Drink  ");
        }
        //first while loop helps to store value at position 0 of the drink array
        while(mode == 0){
            int j = -1;
            for(int i = 0; i < 4; i++){
                j++;
                innermode = 0;
                if(line2 > 0x44){
                    line2 = 0x40;
                }
                /*Innermode while loop waits for 1 of the 5 buttons on the interface panel
                 to be pressed. RB0, 1, 2, 3 each dispense drinks 1, 2, 3, 4 respectively.
                 RB4 acts like a backspace button for the array. If you mistype a drink,
                 you can simply backspace by pressing RB4*/
                while(innermode == 0){
                    if(RB0 == 0){       //if the first button is pressed
                        DelayMs(debounce); // button debounce
                        char number = 1; //define which number 1 for drink one to store in array
                        drink[j] = number; //stores that number in the array
                        lcd_goto(line2); //goes to line 2 of LCD
                        lcd_putch(number+0x30); // adds the character to display
                        line2 = line2 + 0x01; //increments to next character in LCD
                        innermode = 1; // breaks from this while loop and proceeds
                        // to the next character in the drink array
                        // This process for RB0 repeats for RB1, 2, and 3
                    }   
                    //process repeats
                    if(RB1 == 0){
                        DelayMs(debounce);
                        char number = 2;
                        lcd_goto(line2);
                        drink[j] = number;
                        lcd_putch(number +0x30);
                        line2 = line2 + 0x01;
                        innermode = 1;
                    }
                    //process repeats
                    if(RB2 == 0){
                        DelayMs(debounce);
                        char number = 3;
                        lcd_goto(line2);
                        drink[j] = number;
                        lcd_putch(number + 0x30);
                        line2 = line2 + 0x01;
                        innermode = 1;
                    }
                    //process repeats
                    if(RB3 == 0){
                        DelayMs(debounce);
                        char number = 4;
                        lcd_goto(line2);
                        drink[j] = number;
                        lcd_putch(number + 0x30);
                        line2 = line2 + 0x01;
                        innermode = 1;
                    }
                    if(RB4 == 0){         //backspace functionality
                        DelayMs(debounce); // button debounce
                        lcd_goto(line2-0x01); //goes to previous character in LCD
                        line2 = line2 - 0x01; //sets line2 previous character in LCD
                        char number = 0; // will reset that spot in drink array to 0
                        //if drink is at the zero position, store drink[0] as the new value
                        if (j == 0){
                            drink[j] = number;
                        } //if at any other spot, go to previous index
                        else{
                             drink[j-1] = number;  //stores zero into previous spot
                             j = j-1;               //decrement where you are at in the array
                             i = i-1;               //decrement where you are at in the for loop
                        }
                        lcd_putch(""); //           //leaves a blank space (making you deleted previous choice)
                    }
                    //if the array is full, break out of mode and get ready for dispensing
                    if(line2 == 0x44){
                        innermode = 1;
                        mode = 1;
                    } 
                }
            } 
        }
        //wait for any of the 4 button presses to begin dispensing 
        if(RB0 == 0 || RB1 == 0 || RB2 == 0 || RB3 == 0){
            DelayMs(750);
            line2 = 0x40;
            lcd_clear();
            lcd_goto(line2);
            lcd_puts(" Dispensing... ");
            DelayMs(750);
            //actual dispensing using the servos
            l = 0;
            //index through the drink array checking each number. This for loop
            //activates the correct servo depending on the value in each of the 4
            //indexes of the drink array
            for(int i = 0; i < 4; i++){
                l++;
                DelayMs(500); // delay between each drink
                if(drink[i] == 1){
                    get_Servo1();
                }
                if(drink[i] == 2){
                    get_Servo2();
                }
                if(drink[i] == 3){
                    get_Servo3();
                }
                if(drink[i] == 4){
                    get_Servo4();
                }
            }
            mode = 0;
        }
    }
}

void intialize_Everything(){
    TRISC2 = 0; //make RC2 output to the speaker
    TRISD0 = 0; TRISD1 = 0; TRISC5 = 0; TRISC6 = 0; //Servo motor outputs
    TRISB = 0b11111111; //make port b all inputs
    ANSEL = 0; //disable analog
    ANSELH = 0; // ^^^
    nRBPU = 0; //enable pull up resistors
    TMR1GE = 0; //Disable the gate enable bit
    TMR1ON = 1; // Enable TIMER1
    // Select internal clock whose frequency is Fosc/4, Fosc = 8 MHz
    TMR1CS = 0;
    T1CKPS1 = 0;
    T1CKPS0 = 0; // 1:1 pre-scale
    
    CCP1M3 = 0;
    CCP1M2 = 0;
    CCP1M1 = 0;
    CCP1M0 = 0; // set to default mode. Don't want CCP1 running until we call servos
    CCP1IF = 0; // Clear the CCP1IF interrupt flag
    CCP1IE = 1; // Enable Compare Interrupt from CCP1
    PEIE = 1; // Enable all peripheral interrupts
    GIE = 1; // Globally Enable all interrupts
}

//interrupt handler on toggle mode on compare and capture
// takes in highTicks and uses it to calculate lowTicks and toggle a PWM wave for the servo
void __interrupt() interrupt_handler(void) {
    if (CCP1IF == 1) {
            CCPR1 = CCPR1 + highTicks;
            CCP1IF = 0;
    }
}
// compare and no effect
// set pin high, set pin low

/*get_Servo# activates each servo by sending a PWM signal on the correct digital
 pin. This PWM signal is generated by turning the output on the pin on and off and
 waiting a precise delay to produce a square wave signal. We first go to line 2, and print 
 the drink that is being served.
 We sound the speaker between each servo action:
 These actions include plugging, releasing, and plugging again. By first plugging, we 
 make sure that the holes are plugged to start with before releasing. This also helps when
 we first run the machine.
 Each get_Servo# function runs the same for each servo, just with different indices.
 * 
 Variable int i was calibrated through thorough testing. We timed each opening to dispense
 the right amount of liquid for each servo. Each i value is different because each tube dispensed
 at a different amount of pressure. The rate at which the liquid dispensed also was determined
 by how much was in the container at the time.*/
void get_Servo1(){
    line2 = 0x40; // set the LCD to line 2
    lcd_clear(); //clear LCD
    lcd_goto(line2); //go to line 2
    lcd_puts("1"); // display drink 1
    drinkDispensingSound(); //sounds the speaker
    //closes the servo
    for(int i = 0; i < 30; i++){
        RD1 = 1;
        DelayMs(2.5);
        RD1 = 0;
        DelayMs(17.5);
    }
    drinkDispensingSound(); //sounds the speaker
    //opens the servo
    for(int i = 0; i < 375; i++){
        RD1 = 1;
        DelayMs(0.5);
        RD1 = 0;
        DelayMs(19.5);
    }
    drinkDispensingSound(); //sounds the speaker
    //closes the servo
    for(int i = 0; i < 30; i++){
        RD1 = 1;
        DelayMs(2.5);
        RD1 = 0;
        DelayMs(17.5);
    }     
}
//this process above repeats for Servo2, 3, and 4.
void get_Servo2(){
    line2 = 0x40;
    lcd_clear();
    lcd_goto(line2);
    lcd_puts("2");
    drinkDispensingSound();
    for(int i = 0; i < 30; i++){
        RD0 = 1;
        DelayMs(0.5);
        RD0 = 0;
        DelayMs(19.5);
    }
    drinkDispensingSound();
    for(int i = 0; i < 250; i++){
        RD0 = 1;
        DelayMs(2.5);
        RD0 = 0;
        DelayMs(17.5);
    }
    drinkDispensingSound();
    for(int i = 0; i < 30; i++){
        RD0 = 1;
        DelayMs(0.5);
        RD0 = 0;
        DelayMs(19.5);
    }    
}
void get_Servo3(){
    line2 = 0x40;
    lcd_clear();
    lcd_goto(line2);
    lcd_puts("3");
    drinkDispensingSound();
    for(int i = 0; i < 30; i++){
        RC5 = 1;
        DelayMs(2.5);
        RC5 = 0;
        DelayMs(17.5);
    }
    drinkDispensingSound();
    for(int i = 0; i < 280; i++){
        RC5 = 1;
        DelayMs(0.5);
        RC5 = 0;
        DelayMs(19.5);
    }
    drinkDispensingSound();
    for(int i = 0; i < 30; i++){
        RC5 = 1;
        DelayMs(2.5);
        RC5 = 0;
        DelayMs(17.5);
    }
}
void get_Servo4(){
    line2 = 0x40;
    lcd_clear();
    lcd_goto(line2);
    lcd_puts("4");
    drinkDispensingSound();
    for(int i = 0; i < 30; i++){
        RC6 = 1;
        DelayMs(2.5);
        RC6 = 0;
        DelayMs(17.5);
    }
    drinkDispensingSound();
    for(int i = 0; i < 250; i++){
        RC6 = 1;
        DelayMs(0.5);
        RC6 = 0;
        DelayMs(19.5);
    }
    drinkDispensingSound();
    for(int i = 0; i < 30; i++){
        RC6 = 1;
        DelayMs(2.5);
        RC6 = 0;
        DelayMs(17.5);
    }
}

//sounds the speaker by turning on the interrupt and generating a square wave
//interrupt for 400ms delay.
//after delay, turn off speaker
void drinkDispensingSound(){
    CCP1M1 = 1;
    highTicks = 1912; 
    DelayMs(400);
    CCP1M1 = 0;
}