
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "keypad.h"

#include "msp.h"
#include "CTS_Layer.h"



#include "Final Project Slave.h"


/* initialize keypad, use P4.0 - P4.6 */
void initKeypad(void) {

    // set P4.0 - P4.6 as inputs with pull up resistors
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN0);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN1);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN2);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN3);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN4);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN5);
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN6);

}


/* scans keypad for input, if none then NULL is returned */
char scanKeypad(void)
{
    int row, col;

    const char column_select[] = {0x10, 0x20, 0x40};


    for (col = 0; col < 3; col++)
    {
        P4->DIR &= ~0xF0;               //  disable all columns
        P4->DIR |= column_select[col];   // enable one column at a time
        P4->OUT &= ~column_select[col];  //  drive the active column low
        __delay_cycles(10);             //  wait for signal to settle
        row = P4->IN& 0x0F;             //  read all rows
        P4->OUT |= column_select[col];   //  drive the active column high
        if  (row != 0x0F) break;        //  if one of the input is low,
        // some key is pressed
    }


    P4->OUT |= 0xF0;  //  drive all columns high before disable
    P4->DIR &= ~0xF0;  //  disable all columns

    if(col == 3 ) return NULL;  //  if we get here, no key is pressed
    //  gets here when one of the columns has key pressed,
    // check which row it is

    if(row == 0x0E) {                       // key in row 0
        if ( col + 1 == 1)  return '1';
        if ( col + 1 == 2)  return '2';
        if ( col + 1 == 3)  return '3';
    }
    if(row == 0x0D) {                       // key in row 1
        if ( 3 + col + 1 == 4)  return '4';
        if ( 3 + col + 1 == 5)  return '5';
        if ( 3 + col + 1 == 6)  return '6';
    }
    if(row == 0x0B) {                   // key in row 2
        if ( 6 + col + 1 == 7)   return '7';
        if ( 6 + col + 1 == 8)   return '8';
        if ( 6 + col + 1 == 9)   return '9';
    }
    if(row == 0x07) {                   // key in row 3
        if ( 9 + col + 1 == 10)  return '*';
        if ( 9 + col + 1 == 11)  return '0';
        if ( 9 + col + 1 == 12)  return '#';
    }
    return NULL;   // just to be safe
}

/* returns key pressed */
char getKey (void)
{
    //Variables for key returned from getKey, previous key from getKey and confirmed key
    char key;
    static char prev_key = NULL;
    char confirmed_key;
    static uint8_t CapFlag = 0;
    static uint8_t CapLatch = 0;

    //Continue this loop until keypad data is received
    while(1)
    {


        //key is returned from getKey
        key = scanKeypad();
        //Detect rising edge of key press. Only accept entry of a pressed key if the
        //previous value from getKey was NULL, which occurs when no key is pressed. This
        //prevents the same key press from being recorded multiple times

        IntFlag = 0;
        if( TI_CAPT_Button(&my_button) ){
            //P1OUT |= BIT0;
            CapLatch = 1;
           // CapFlag = 1;
            if(IntFlag == 0){
            key = 12;
            }
        }
        else{
            //MAP_UART_transmitData(EUSCI_A1_BASE, 13);
            //CapLatch = 0;
            if(IntFlag == 0){
            //P1OUT &= ~BIT0;
            }
            //CapFlag = 2;
           // key = 13;
        }
//
//        if( !(P6IN & BIT4) ){
//
//            if(LeftFlag == 0){
//                LeftFlag = 1;
//                StartLeft();
//            }
//            else{
//                LeftFlag = 0;
//                StopLeft();
//            }
//
//
//        }
//
//        //Turn on right turn signal
//        if( !(P6IN & BIT5) ){
//
//            if(RightFlag == 0){
//                RightFlag = 1;
//                StartRight();
//            }
//            else{
//                RightFlag = 0;
//                StopRight();
//            }
//        }

        if (key != NULL && prev_key == NULL) {
            confirmed_key = key;
            prev_key = confirmed_key;
            return confirmed_key;
        }
        prev_key = key; //Set previous key as key, to detect rising edge of key press.

//        if(CapFlag == 1){
//            CapFlag = 0;
//            MAP_UART_transmitData(EUSCI_A1_BASE, 12);
//            //return 12;
//        }
//        else if(CapFlag == 2){
//            CapFlag = 0;
//            MAP_UART_transmitData(EUSCI_A1_BASE, 13);
//        }
    }
}

