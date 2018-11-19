/* Authors: Sam Kirk and Ben Burger
 * Assignment: EGR 326 Proximity Sensor Library
 * Professor: Prof. Krug
 * Date: 10/26/18
 * Notes:
 */


/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/*Custom Libraries */
#include "Prox.h"

/* Function Prototypes */
void InitProx();
void PulseProx(void);
uint8_t CheckProx(uint32_t meas1, uint32_t meas2);


/* const to setup TIMER A mode */
const Timer_A_CaptureModeConfig Prox_captureModeConfig =
{
 TIMER_A_CAPTURECOMPARE_REGISTER_1,             // CC Register 1
 TIMER_A_CAPTUREMODE_RISING_AND_FALLING_EDGE,   // Rising Edge and falling
 TIMER_A_CAPTURE_INPUTSELECT_CCIxA,             // CCIxA Input Select
 TIMER_A_CAPTURE_SYNCHRONOUS,                   // Synchronized Capture
 TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE,       // Enable interrupt
 TIMER_A_OUTPUTMODE_OUTBITVALUE                 // Output bit value
};

/* const to setup clock source */
const Timer_A_ContinuousModeConfig Prox_continuousModeConfig =
{
 TIMER_A_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
 TIMER_A_CLOCKSOURCE_DIVIDER_1,      // SMCLK/1 = 3 MHz
 TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Timer ISR
 TIMER_A_SKIP_CLEAR                  // Skip Clear Counter
};

void PulseProx(void){

    /* turn off P5.0 for 10 us, starts a prox read, P5.0 is inverter */
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
    __delay_cycles(480);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);

}
uint8_t CheckProx(uint32_t meas1, uint32_t meas2){
    float distance;
    uint8_t ProxFlag;
    uint32_t time;

    if(meas2 < meas1) {
        time = meas2 + 65535 - meas1;
    }
    else {
        time = meas2 - meas1;
    }

    distance = ( ((float)time / 3) / 58 );

    if (distance < 15) {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
        ProxFlag = 1;
    }
    else {
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
        ProxFlag = 0;
    }

    //distance = distance * 39.3701;                  // convert from meters to inches

    return ProxFlag;


}
void InitProx(void){

    /* setting up P2.4 as input value for Timer A0 */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);

    /* set up P5.0 to toggle the trigger on the prox. sensor */
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);


    /*Onboard LED setup */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    /* Configuring Capture Mode */
    Timer_A_initCapture(TIMER_A0_BASE, &Prox_captureModeConfig);

    /* Configuring Continuous Mode */
    Timer_A_configureContinuousMode(TIMER_A0_BASE,&Prox_continuousModeConfig);

    /* Enabling interrupts */
    Interrupt_enableInterrupt(INT_TA0_N);


    /* start Timer A0 */
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);


}




