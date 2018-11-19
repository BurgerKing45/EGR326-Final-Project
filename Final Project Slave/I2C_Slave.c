/* Authors: Sam Kirk and Ben Burger
 * Assignment: EGR 326 Final Project
 * Professor: Prof. Krug
 * Date: 11/04/18
 * Notes: I2C code for the slave MSP
 */

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


/* Set Slave Address */
#define SLAVE_ADDRESS 0x48

/* Initialize I2C on P1.6 and P1.7 */
void initSlaveI2C(void) {
    // select Port 1 for I2C
    // set Pin 6, 7 to input Primary Module Function,
    // P1.6 is UCB1SDA, P1.7 is UCB1SCL
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);


    MAP_I2C_initSlave(EUSCI_B0_BASE, SLAVE_ADDRESS, EUSCI_B_I2C_OWN_ADDRESS_OFFSET0, EUSCI_B_I2C_OWN_ADDRESS_ENABLE);

    // Enable I2C Module to start operations
    MAP_I2C_enableModule(EUSCI_B0_BASE);

    // Set Slave in receive mode
    MAP_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);

    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);

    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    MAP_Interrupt_enableInterrupt(INT_EUSCIB0);
}

///* Send a single byte to the master MSP. */
//void slaveSendSingleByte(uint8_t byte) {
//
//    /* Wait until ready to transmit. */
//    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
//    while( MAP_I2C_masterIsStopSent (EUSCI_B0_BASE) == EUSCI_B_I2C_SENDING_STOP);
//
//    /* Send byte. */
//    MAP_I2C_masterSendSingleByte(EUSCI_B0_BASE, byte);
//}
