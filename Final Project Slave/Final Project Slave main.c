/* Authors: Sam Kirk and Ben Burger
 * Assignment: EGR 326 Final Project
 * Professor: Prof. Krug
 * Date: 11/04/18
 * Notes: code for the slave MSP
 */


/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/*Custom Libraries */
#include "RTC.h"
#include "Keypad.h"
#include "ST7735.h"
#include "Backlight.h"
#include "BuzzerAndClock.h"
#include "I2C_Slave.h"

/* Global Variables*/
char KeyPressed = NULL;
uint8_t KeyFlag = 0;

int main(void) {

    /* Stop Watchdog */
    MAP_WDT_A_holdTimer();

    /* Initializations */
    clockInit48MHzXTL();
    initSlaveI2C();
    initKeypad();

    MAP_Interrupt_enableMaster();

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);


    while(1){


        KeyPressed = getKey();
    }

}

/* Interrupt Handler to Receive Data via I2C on P1.6 */
void EUSCIB0_IRQHandler(void) {
    uint32_t status = I2C_getEnabledInterruptStatus(EUSCI_B0_BASE);
    uint8_t rxdata;

    MAP_I2C_clearInterruptFlag(EUSCI_B0_BASE, status);  // clear the receive interrupt

    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0) {

        rxdata = MAP_I2C_slaveGetData(EUSCI_B0_BASE);    // receives the data

        if (rxdata == 0) {
            //
        }
        if (rxdata == 1) {
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);

        }
        if (rxdata == 2) {
            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);

        }
        if (rxdata == 3) {
            // turn off chime 1
        }
        if (rxdata == 4) {
            // turn off chime 2
        }
    }

    if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0){
        MAP_I2C_slavePutData(EUSCI_B0_BASE, KeyPressed);
        KeyPressed = NULL;
    }


}












