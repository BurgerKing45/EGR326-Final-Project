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
#include "UART.h"
#include "LEDs_PBs.h"

#include "msp.h"
#include "CTS_Layer.h"

/* Global Variables*/
char KeyPressed = NULL;
uint8_t KeyFlag = 0;
uint8_t TransmissionFlag = 0;

uint8_t LeftFlag = 0;
uint8_t RightFlag = 0;

int main(void) {

    /* Stop Watchdog */
    MAP_WDT_A_holdTimer();

    /* Initializations */
    clockInit48MHzXTL();
    InitUART();
    initKeypad();
    InitTurnSignalButtons();
    InitTimerA();


    P1DIR |= BIT0;
    P1OUT &= ~BIT0;




    TI_CAPT_Init_Baseline(&my_button);



    TI_CAPT_Update_Baseline(&my_button, 5);

    MAP_Interrupt_enableMaster();

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);


    while(1){


        KeyPressed = getKey();

        if( KeyPressed != NULL) {
            MAP_UART_transmitData(EUSCI_A1_BASE, KeyPressed);
            KeyPressed = NULL;

        }
    }

}



/* EUSCI A1 UART ISR - Echoes data back to PC host */
void EUSCIA1_IRQHandler(void)
{
    uint8_t rcv_byte = 0;
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A1_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A1_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        rcv_byte = MAP_UART_receiveData(EUSCI_A1_BASE);

        //Overheat Condition
        if(rcv_byte == 1){
            StartBuzzer(1);

        }
        //Overheat over condition
        else if(rcv_byte == 2){
            StopBuzzer();

        }
        //Prox triggered condition
        else if(rcv_byte == 3){
            StartBuzzer(10);
            StartWarning();

        }
        //End prox triggered condition
        else if(rcv_byte == 4){
            StopBuzzer();
            StopWarning();
        }

    }

}

void PORT6_IRQHandler(void){
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P6);
    GPIO_clearInterruptFlag(GPIO_PORT_P6, status);

    //Turn on left turn signal
    if(status & GPIO_PIN4){

        if(LeftFlag == 0){
            LeftFlag = 1;
            StartLeft();
        }
        else{
            LeftFlag = 0;
            StopLeft();
        }


    }

    //Turn on right turn signal
    if(status & GPIO_PIN5){

        if(RightFlag == 0){
            RightFlag = 1;
            StartRight();
        }
        else{
            RightFlag = 0;
            StopRight();
        }
    }

}

///* Interrupt Handler to Receive Data via I2C on P1.6 */
//void EUSCIB0_IRQHandler(void) {
//    uint32_t status = I2C_getEnabledInterruptStatus(EUSCI_B0_BASE);
//    uint8_t rxdata;
//
//    MAP_I2C_clearInterruptFlag(EUSCI_B0_BASE, status);  // clear the receive interrupt
//
//    if (status & EUSCI_B_I2C_RECEIVE_INTERRUPT0) {
//
//        rxdata = MAP_I2C_slaveGetData(EUSCI_B0_BASE);    // receives the data
//
//        if (rxdata == 0) {
//            //
//        }
//        if (rxdata == 1) {
//            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
//
//        }
//        if (rxdata == 2) {
//            MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
//
//        }
//        if (rxdata == 3) {
//            // turn off chime 1
//        }
//        if (rxdata == 4) {
//            // turn off chime 2
//        }
//    }
//
//    if (status & EUSCI_B_I2C_TRANSMIT_INTERRUPT0){
//        TransmissionFlag = 1;
//        //if( KeyPressed != NULL ){
//
//            //MAP_I2C_slavePutData(EUSCI_B0_BASE, KeyPressed);
//            //KeyPressed = NULL;
//        //}
//    }
//
//
//}












