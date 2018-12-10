/* Authors: Sam Kirk and Ben Burger
 * Assignment: EGR 326 Stepper Motor Library
 * Professor: Prof. Krug
 * Date: 11/08/18
 * Notes:
 */
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Custom Includes */
#include "Stepper.h"

//Macros
#define An     GPIO_PIN0
#define A     GPIO_PIN1
#define Bn     GPIO_PIN2
#define B     GPIO_PIN3

//Functions
void stepper_move(uint16_t steps, uint8_t direction);
void Update_Speedometer(uint16_t speed);
void InitStepper(void);

//Global Variables


/*Updates the speedometer */
void Update_Speedometer(uint16_t speed){
    uint16_t speed_dif;
    uint16_t steps;
    static uint16_t CurrentSpeed = 0;

    if(CurrentSpeed <= speed){
        speed_dif = speed - CurrentSpeed;
        steps = speed_dif * 3 / 4;
        stepper_move(steps, 0);
        CurrentSpeed = CurrentSpeed + speed_dif;
    }
    else if(CurrentSpeed > speed){
        speed_dif = CurrentSpeed - speed;
        steps = speed_dif * 3 / 4;
        stepper_move(steps, 1);
        CurrentSpeed = CurrentSpeed - speed_dif;
    }



}


/*Initializes and zeros the speedometer */
void InitStepper(void){
    /*Set GPIO pins */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3);

    /* Zero the speedometer */
    stepper_move(100, 1);

}

//Stepper controller function
void stepper_move(uint16_t steps, uint8_t direction){
    uint16_t i;

    //Logic from Prof. Krug's slides to drive stepper motor forward or reverse
    if(direction){

        for(i = 0; i < steps; i++){


            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, A | B);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, An | Bn);
            __delay_cycles(60000);

            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, An | B);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, A | Bn);
            __delay_cycles(60000);

            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, An| Bn);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, A | B);
            __delay_cycles(60000);

            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, A | Bn);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, An | B);
            __delay_cycles(60000);


        }
    }

    if(!direction){

        for(i = 0; i < steps; i++){

            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, A | Bn);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, An | B);
            __delay_cycles(60000);

            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, An| Bn);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, A | B);
            __delay_cycles(60000);

            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, An | B);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, A | Bn);
            __delay_cycles(60000);

            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P7, A | B);
            MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P7, An | Bn);
            __delay_cycles(60000);



        }
    }

}

