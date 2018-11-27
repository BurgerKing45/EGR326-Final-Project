
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "BuzzerAndClock.h"


//Configuration Parameters for PWM on Timer A.
Timer_A_PWMConfig pwmConfigA0 =
{
 TIMER_A_CLOCKSOURCE_ACLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 32678,
 TIMER_A_CAPTURECOMPARE_REGISTER_1,
 TIMER_A_OUTPUTMODE_RESET_SET,
 16339
};

Timer_A_PWMConfig pwmConfigA3 =
{
 TIMER_A_CLOCKSOURCE_ACLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 8,
 TIMER_A_CAPTURECOMPARE_REGISTER_1,
 TIMER_A_OUTPUTMODE_RESET_SET,
 4
};

void InitLCD_Delay(uint32_t period){

    MAP_Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_16, TIMER32_32BIT,
                           TIMER32_PERIODIC_MODE);
    MAP_Timer32_setCount(TIMER32_1_BASE, period);
    MAP_Interrupt_enableInterrupt(TIMER32_1_INTERRUPT);

    MAP_Timer32_enableInterrupt(TIMER32_1_BASE);
    MAP_Timer32_startTimer(TIMER32_1_BASE, true);
}
void InitTimer32(uint32_t period){

    MAP_Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_16, TIMER32_32BIT,
                           TIMER32_PERIODIC_MODE);
    MAP_Timer32_setCount(TIMER32_0_BASE, period);
    MAP_Interrupt_enableInterrupt(TIMER32_0_INTERRUPT);

    MAP_Timer32_enableInterrupt(TIMER32_0_BASE);
    MAP_Timer32_startTimer(TIMER32_0_BASE, false);
}
void StartBuzzer(uint32_t Freq){


    /* Configuring GPIO 10.4 and 2.4 as peripheral output for PWM */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P10, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);


    //Configuring Timer_A3 to have a frequency of 4000Hz.
    MAP_Timer_A_generatePWM(TIMER_A3_BASE, &pwmConfigA3);


    //Frequency of Timer_A0 dependent on which alarm is triggered.
    //Different frequencies for different unique alarms
    pwmConfigA0.timerPeriod = 32678 / Freq;
    pwmConfigA0.dutyCycle = (32678 / Freq ) * 0.5;

    MAP_Timer_A_generatePWM(TIMER_A0_BASE, &pwmConfigA0);

}
void StopBuzzer(void){

    MAP_Timer_A_stopTimer(TIMER_A0_BASE);
    MAP_Timer_A_stopTimer(TIMER_A3_BASE);
}

/* sets the clock module to use the external 48 MHz crystal
 * Then MClK = 48MHz, SMCLK = 3MHz ACLK = 32.678 kHz
 */
void clockInit48MHzXTL(void) {
    uint32_t SMCLKfreq, MCLKfreq, ALCKfreq; //For checcking clock speed

    /* Configuring pins for peripheral/crystal usage */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
                                                    GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32768,48000000); // enables getMCLK, getSMCLK to know externally set frequencies
    /* Starting HFXT in non-bypass mode without a timeout. Before we start
     * we have to change VCORE to 1 to support the 48MHz frequency */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_startHFXT(false); // false means that there are no timeouts set, will return when stable
    /* Initializing MCLK to HFXT (effectively 48MHz) */
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // set SMCLK to 3 MHz
    MAP_CS_initClockSignal(CS_SMCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_16);
    // set ACLK to 32.678KHz
    MAP_CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //     MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    //     MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    //     MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_4);
    SMCLKfreq=MAP_CS_getSMCLK(); // get SMCLK value to verify it was set to 3 MHz
    MCLKfreq=MAP_CS_getMCLK(); // get MCLK value and verify it was set to 48MHz
    ALCKfreq = MAP_CS_getACLK(); // get MCLK value and verify it was set to 32.678 kHz
}


