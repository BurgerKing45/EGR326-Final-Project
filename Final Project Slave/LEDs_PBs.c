
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "LEDs_PBs.h"

Timer_A_PWMConfig pwmConfigRight =
{
 TIMER_A_CLOCKSOURCE_ACLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 32678,
 TIMER_A_CAPTURECOMPARE_REGISTER_3,
 TIMER_A_OUTPUTMODE_RESET_SET,
 16339
};

Timer_A_PWMConfig pwmConfigLeft =
{
 TIMER_A_CLOCKSOURCE_ACLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 32678,
 TIMER_A_CAPTURECOMPARE_REGISTER_4,
 TIMER_A_OUTPUTMODE_RESET_SET,
 16339
};

Timer_A_PWMConfig pwmConfigWarning =
{
 TIMER_A_CLOCKSOURCE_ACLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 32678,
 TIMER_A_CAPTURECOMPARE_REGISTER_1,
 TIMER_A_OUTPUTMODE_RESET_SET,
 16339
};


void InitTurnSignalButtons(void){
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5 );
    MAP_GPIO_enableInterrupt(GPIO_PORT_P6, GPIO_PIN4 | GPIO_PIN5 );
    MAP_Interrupt_enableInterrupt(INT_PORT6);
}

void InitTimerA(void){
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P7, GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN7 , GPIO_PRIMARY_MODULE_FUNCTION);


}

void StartLeft(void){
    pwmConfigLeft.dutyCycle = 16339;
    MAP_Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfigLeft);
}

void StartRight(void){
    pwmConfigRight.dutyCycle = 16339;
    MAP_Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfigRight);
}

void StartWarning(void){
    pwmConfigWarning.dutyCycle = 16339;
    MAP_Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfigWarning);
}

void StopLeft(void){
    pwmConfigLeft.dutyCycle = 0;

    MAP_Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfigLeft);
}

void StopRight(void){
    pwmConfigRight.dutyCycle = 0;

    MAP_Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfigRight);
}

void StopWarning(void){
    pwmConfigWarning.dutyCycle = 0;

    MAP_Timer_A_generatePWM(TIMER_A1_BASE, &pwmConfigWarning);
}


