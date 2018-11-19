/* Authors: Sam Kirk and Ben Burger
 * Assignment: EGR 326 Final Project
 * Professor: Prof. Krug
 * Date: 11/04/18
 * Notes: I2C code for the master MSP
 */


/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* Set Slave Address */
#define SLAVE_ADDRESS 0x48

/* Configuration for I2C Communication */
const eUSCI_I2C_MasterConfig i2cConfig =
{
 EUSCI_B_I2C_CLOCKSOURCE_SMCLK,     // SMCLK clock source
 12000000,                          // SMCLK = 12 MHz
 EUSCI_B_I2C_SET_DATA_RATE_400KBPS, // desired I2C clock of 400 kHz
 0,                                 // no byte counter threshold
 EUSCI_B_I2C_NO_AUTO_STOP           // no auto stop
};



/* Initializes I2C Module */
void initMasterI2C(void) {

    // select Port 1 for I2C
    // set Pin 6, 7 to input Primary Module Function,
    // P1.6 is UCB1SDA, P1.7 is UCB1SCL
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    // Initializing I2C Master
    MAP_I2C_initMaster(EUSCI_B0_BASE, &i2cConfig);

    // Enable I2C Module to start operations
    MAP_I2C_enableModule(EUSCI_B0_BASE);

    // Specify slave address
    MAP_I2C_setSlaveAddress(EUSCI_B0_BASE, SLAVE_ADDRESS);


    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_NAK_INTERRUPT);

    MAP_Interrupt_enableInterrupt(INT_EUSCIB0);
}

/* Send a single byte to the slave MSP. */
void masterSendSingleByte(uint8_t byte) {

    /* Wait until ready to transmit. */
    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
    while( MAP_I2C_masterIsStopSent (EUSCI_B0_BASE) == EUSCI_B_I2C_SENDING_STOP);

    /* Send byte. */
    MAP_I2C_masterSendSingleByte(EUSCI_B0_BASE, byte);

    while( MAP_I2C_masterIsStopSent (EUSCI_B0_BASE) == EUSCI_B_I2C_SENDING_STOP);
}
void masterRecieveSingleByte(void){
    MAP_I2C_disableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_TRANSMIT_INTERRUPT0);
    MAP_I2C_setMode(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    MAP_I2C_masterReceiveStart(EUSCI_B0_BASE);
    MAP_I2C_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0);
}
