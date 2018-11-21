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

#include "UART.h"

const eUSCI_UART_Config uartConfig =
{
 EUSCI_A_UART_CLOCKSOURCE_ACLK,          // SMCLK Clock Source
 3,                                     // BRDIV = 78
 0,                                       // UCxBRF = 2
 3,                                       // UCxBRS = 0
 EUSCI_A_UART_NO_PARITY,                  // No Parity
 EUSCI_A_UART_LSB_FIRST,                  // LSB First
 EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
 EUSCI_A_UART_MODE,                       // UART mode
 EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

void InitUART(void){
    /* Selecting P1.2 and P1.3 in UART mode */
        MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

        /* Configuring UART Module */
        MAP_UART_initModule(EUSCI_A1_BASE, &uartConfig);

        /* Enable UART module */
        MAP_UART_enableModule(EUSCI_A1_BASE);

        /* Enabling interrupts */
        MAP_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        MAP_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
        MAP_Interrupt_enableInterrupt(INT_EUSCIA1);

}
