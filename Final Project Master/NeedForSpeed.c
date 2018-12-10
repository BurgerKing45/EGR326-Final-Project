/* Authors: Sam Kirk and Ben Burger
 * Assignment: EGR 326 Hall Effect Library
 * Professor: Prof. Krug
 * Date: 11/03/18
 * Notes:
 * the presence of a magnet.
 */

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "NeedForSpeed.h"

void InitHall(void){

    /* setting up P3.7 */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN7);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN7);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN7);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P3, GPIO_PIN7,GPIO_HIGH_TO_LOW_TRANSITION);
    MAP_Interrupt_enableInterrupt(INT_PORT3);


}


/* Configuring SysTick to trigger at 1500000 (MCLK is 3MHz so this will make
 * it toggle every 0.5s) */
void InitSysTick(uint32_t delay){

    MAP_SysTick_disableModule();
    MAP_SysTick_setPeriod(delay);
    MAP_SysTick_enableModule();
    MAP_SysTick_enableInterrupt();
}

uint16_t Convert_Speed(uint16_t total_counts){
    int16_t speed;

    speed = 2 * total_counts;


    return speed;

}
