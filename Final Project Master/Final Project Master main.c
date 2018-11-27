/* Authors: Sam Kirk and Ben Burger
 * Assignment: EGR 326 Final Project
 * Professor: Prof. Krug
 * Date: 11/04/18
 * Notes: code for the master MSP
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
#include "Stepper.h"
#include "Flash.h"
#include "LCD.h"
#include "UART.h"
#include "NeedForSpeed.h"
#include "Prox.h"

//Macros
#define Startup   0
#define Idle      1
#define Menu      2
#define TimeEnter 3
#define AlarmLog  4
#define SpeedLog  5

/* Global Variables */
uint8_t state = 0;
uint8_t MenuFlag = 0;

uint8_t rcv_byte;
uint8_t StateChanged = 0;
uint8_t rcvflag;

//Speed related
uint16_t speed_count = 0;
uint16_t speed = 0;
uint16_t prev_speed = 0;

//Prox Related
uint32_t meas1 = 0;
uint32_t meas2 = 0;
uint8_t ProxFlag = 0;
uint8_t ProxMeasured = 0;

/* Time, date and temp returnted from the RTC */
char TimeDate[19];

uint8_t DisplayStyle = 0;

uint8_t TimeOut = 0;

/* variables for user keypad input */
char m_seconds[2];
char m_minutes[2];
char m_hours[2];
char m_date[2];
char m_month[2];
char m_year[2];
char day[1];
char actual_day[9];


/* BCD variables, send to RTC*/
uint8_t bcdSeconds;
uint8_t bcdMinutes;
uint8_t bcdHours;
uint8_t bcdDay;
uint8_t bcdDate;
uint8_t bcdMonth;
uint8_t bcdYear;
uint8_t TempInt;
uint8_t TempFrac;

char UserTime[13] = "";

void promptTime(void);
void promptSeconds(void);
void promptMinutes(void);
void promptHours(void);
void promptDay(void);
void promptDate(void);
void promptMonth(void);
void promptYear(void);

void CheckRTC(void);
void CheckSpeed(void);
void CheckTemp(void);

void InitButton(void);

char user_entry[] = "xx:xx:xx xx/xx/20xx";


int main(void) {

    /* Stop Watchdog  */
    MAP_WDT_A_holdTimer();

    while(1) {

        switch(state){

        /* Start-Up State */
        case Startup :

            /* Initializations */
            clockInit48MHzXTL();
            ST7735_InitR(INITR_REDTAB);
            initRTC();
            InitUART();
            InitBacklight();     // check for clock issues now that we are running at 48 MHz
            InitStepper();
            InitProx();
            InitButton();




            InitTimer32(3000000);
            InitHall();
            InitSysTick(12000000);



            //Init Display
            ClearDisplay();

            MAP_Interrupt_enableMaster();



            // MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);

            // StopBuzzer();  // stop buzzer



            //MAP_UART_transmitData(EUSCI_A1_BASE, 1);

            state = 1;   // go to Idle State
            break;

            /* Idle State */
        case Idle :

            StateChanged = 0;

            //Idle Display
            Timer32_haltTimer(TIMER32_0_BASE);

          //  InitLCD_Delay(1500000);

            ClearLowerDisplay();
            IdleDisplay();
            InitTimer32(3000000);

            //Request keypad info from slave


            while(!StateChanged){

                if(rcv_byte == '*'){
                    state = Menu;
                    StateChanged = 1;
                    rcv_byte = 0;
                    //MAP_UART_transmitData(EUSCI_A1_BASE, 1);


                }

            }

            break;

            //Menu State
        case Menu :

            StateChanged = 0;

            //Clear Display and print menu
            // ClearLowerDisplay();
            Timer32_haltTimer(TIMER32_0_BASE);


            MenuDisplay();
            InitTimer32(3000000);


            while(!StateChanged){

                if(rcv_byte == '1'){
                    state = TimeEnter;
                    StateChanged = 1;
                    rcv_byte = 0;
                }

                else if(rcv_byte == '2'){
                    state = AlarmLog;
                    StateChanged = 1;
                    rcv_byte = 0;
                }
                else if(rcv_byte == '3'){
                    state = SpeedLog;
                    StateChanged = 1;
                    rcv_byte = 0;
                }
                else if(rcv_byte == '4'){
                    state = Idle;
                    StateChanged = 1;
                    rcv_byte = 0;

                }


            }
            break;

        case TimeEnter :

            StateChanged = 0;

            //Time Enter Display
            Timer32_haltTimer(TIMER32_0_BASE);

            ClearLowerDisplay();
            InitTimer32(3000000);
            TimeOut = 0;

            promptTime();

            writeToRTC();

            user_entry[0] = 'x';
            user_entry[1] = 'x';
            user_entry[3] = 'x';
            user_entry[4] = 'x';
            user_entry[6] = 'x';
            user_entry[7] = 'x';
            user_entry[9] = 'x';
            user_entry[10] = 'x';
            user_entry[12] = 'x';
            user_entry[13] = 'x';
            user_entry[17] = 'x';
            user_entry[18] = 'x';

            m_seconds[0] = NULL;
            m_seconds[1] = NULL;
            m_minutes[0] = NULL;
            m_minutes[1] = NULL;
            m_hours[0] = NULL;
            m_hours[1] = NULL;
            m_date[0] = NULL;
            m_date[1] = NULL;
            m_month[0] = NULL;
            m_month[1] = NULL;
            m_year[0] = NULL;
            m_year[1] = NULL;

            state = Idle;

            break;

        case AlarmLog :

            StateChanged = 0;
            //ClearLowerDisplay();


            Timer32_haltTimer(TIMER32_0_BASE);

            DisplayAlarmLog();
            InitTimer32(3000000);

            while(!StateChanged){

                if(rcv_byte == '#'){
                    state = Idle;
                    StateChanged = 1;
                    rcv_byte = 0;
                }


            }

            break;

        case SpeedLog :

            StateChanged = 0;
            //ClearLowerDisplay();



            Timer32_haltTimer(TIMER32_0_BASE);

            DisplaySpeedLog();
            InitTimer32(3000000);

            while(!StateChanged){

                if(rcv_byte == '#'){
                    state = Idle;
                    StateChanged = 1;
                    rcv_byte = 0;
                }


            }

            break;

        default :

            break;
        }
    }
}

void SysTick_Handler(void)
{
    speed = Convert_Speed(speed_count);
    speed_count = 0;
    InitSysTick(12000000);
}
/* Timer32 ISR for LCD Display */
void T32_INT2_IRQHandler(void){
    MAP_Timer32_clearInterruptFlag(TIMER32_1_BASE);

    //Renable the main interrupt
  //  InitTimer32(3000000);


}
/* Timer32 ISR */
void T32_INT1_IRQHandler(void){
    MAP_Timer32_clearInterruptFlag(TIMER32_0_BASE);

    TimeOut = 1;

    CheckRTC();
    UpdateDisplay(speed, TimeDate, DisplayStyle);
    Update_Speedometer(speed);
    CheckSpeed();
    //Checking the prox. Disable speed checking

    SysTick_disableModule();
    Interrupt_disableInterrupt(INT_PORT3);
    PulseProx();


    MAP_ADC14_toggleConversionTrigger();



}

/* interrupt handler for Timer A0 */
void TA0_N_IRQHandler(void){
    int rising = 0;
    if (TIMER_A0->CCTL[1] & BIT0) { // Timer A0.1 was the cause. This is setup as a capture
        Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);  // clear timer_A interrupt flag

        if(P2IN & BIT4) rising=1; else rising=0; // check for rising or falling edge on input
        if(rising) {
            meas1 = Timer_A_getCaptureCompareCount(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1); // read timer_A value

            //            MAP_Timer_A_stopTimer(TIMER_A0_BASE);
            //
            //            TA0R = 0;
            //            /* start Timer A0 */
            //            Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_CONTINUOUS_MODE);


        }
        else {
            meas2 = Timer_A_getCaptureCompareCount(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1); // read timer_A value
            ProxFlag = CheckProx(meas1, meas2);

            if(ProxFlag == 1)
            {
                MAP_UART_transmitData(EUSCI_A1_BASE, 3);
            }
            else if (ProxFlag == 0)
            {
                MAP_UART_transmitData(EUSCI_A1_BASE, 4);
            }

            ProxMeasured = 1;
            InitSysTick(12000000);
            Interrupt_enableInterrupt(INT_PORT3);
        }
    }
}
/* GPIO ISR */
void PORT3_IRQHandler(void)
{
    uint16_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P3, status);

    if(status & GPIO_PIN7)
    {
        speed_count = speed_count + 1;
    }


}
/* EUSCI A1 UART ISR - Echoes data back to PC host */
void EUSCIA1_IRQHandler(void)
{
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A1_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A1_BASE, status);



    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        rcv_byte = MAP_UART_receiveData(EUSCI_A1_BASE);


    }

}




//Configuration Parameters for PWM on Timer A.
//SMCK source, 3MHz, clock divider of 1 for a 3MHz TimerA clock. For a frequency of 1kHz, set Timer A period to
//3000. Than a 50% duty cycle would be 1500.
//Output mode = reset/set.
Timer_A_PWMConfig BacklightpwmConfig =
{
 TIMER_A_CLOCKSOURCE_SMCLK,
 TIMER_A_CLOCKSOURCE_DIVIDER_1,
 3000000,
 TIMER_A_CAPTURECOMPARE_REGISTER_3,
 TIMER_A_OUTPUTMODE_RESET_SET,
 1500,
};


void ADC14_IRQHandler(void)
{

    uint16_t Duty_Cycle;
    float cur_ADCResult;
    uint64_t status;

    //Get the status of the ADC interrupt
    status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);

    //If the interrupt occurred on ADC instance 0, than get the result
    if(status & ADC_INT0)
    {
        //ADC results

        float normalized_ADCRes;

        cur_ADCResult = MAP_ADC14_getResult(ADC_MEM0);  //Get ADC conversion result

        Duty_Cycle = 0.0091 * cur_ADCResult + 4.6514;

        //Mess with this to change brightness
        BacklightpwmConfig.dutyCycle = 6000 * Duty_Cycle *.01;
        MAP_Timer_A_generatePWM(TIMER_A2_BASE, &BacklightpwmConfig);
        normalized_ADCRes = (cur_ADCResult * 3.3) / 16384;    //Normalize that result given Vref = 3.3V and 14 bit ADC


    }

}

void CheckSpeed(void){
    char SpeedTimeDate[13];

            if( (speed >= 85) && (prev_speed < 85) ){
                MAP_UART_transmitData(EUSCI_A1_BASE, 1);

                strncpy(SpeedTimeDate, TimeDate, 13);

                WriteToFlash(SpeedTimeDate, 2);
            }
            else if( (speed < 85) && (prev_speed >= 85) ) {
                MAP_UART_transmitData(EUSCI_A1_BASE, 2);
            }

            prev_speed = speed;

}

void CheckTemp(void){
    char TempTimeDate[13];

                if( (speed >= 85) && (prev_speed < 85) ){
                    MAP_UART_transmitData(EUSCI_A1_BASE, 1);

                    strncpy(TempTimeDate, TimeDate, 13);

                    WriteToFlash(TempTimeDate, 1);
                }
                else if( (speed < 85) && (prev_speed >= 85) ) {
                    MAP_UART_transmitData(EUSCI_A1_BASE, 2);
                }

                prev_speed = speed;
}

void CheckRTC(void){

    strcpy(TimeDate, returnTimeDateInfo() );
}

/* prompts user to enter time and date info */
void promptTime(void) {
    promptSeconds();
    promptMinutes();
    promptHours();
    promptDate();
    promptMonth();
    promptYear();
    //    promptDay();
}

/* prompt user to enter seconds */
void promptSeconds(void) {
    if (m_seconds[0] == NULL) {
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 9, "enter seconds (0-59)    ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 10, "              ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);



        while(m_seconds[0] == NULL){
            m_seconds[0] = rcv_byte;
        };
        user_entry[6] = m_seconds[0];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        rcv_byte = NULL;


        while(m_seconds[1] == NULL){
            m_seconds[1] = rcv_byte;
        };
        user_entry[7] = m_seconds[1];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        if (m_seconds[0] < '0' || m_seconds[0] > '5' || m_seconds[1] < '0' || m_seconds[1] > '9') {
            ST7735_DrawString(1, 10, "invalid number", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            __delay_cycles(48000000);  // delay for 1 second
            m_seconds[0] = NULL;
            m_seconds[1] = NULL;
            user_entry[6] = 'x';
            user_entry[7] = 'x';
            promptSeconds();
        }
        else {
            return;
        }
    }
}

/* prompt user to enter minutes*/
void promptMinutes(void) {
    if (m_minutes[0] == NULL) {
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 9, "enter minutes (0-59)    ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 10, "              ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);



        while(m_minutes[0] == NULL){
            m_minutes[0] = rcv_byte;
        };
        user_entry[3] = m_minutes[0];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        rcv_byte = NULL;


        while(m_minutes[1] == NULL){
            m_minutes[1] = rcv_byte;
        };
        user_entry[4] = m_minutes[1];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        if (m_minutes[0] < '0' || m_minutes[0] > '5' || m_minutes[1] < '0' || m_minutes[1] > '9') {
            ST7735_DrawString(1, 10, "invalid number", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            __delay_cycles(48000000);  // delay for 1 second
            m_minutes[0] = NULL;
            m_minutes[1] = NULL;
            user_entry[3] = 'x';
            user_entry[4] = 'x';
            promptMinutes();
        }
        else {
            return;
        }
    }
}

/* prompt user to enter hours */
void promptHours(void) {
    if (m_hours[0] == NULL) {
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 9, "enter hours (1-24)    ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 10, "              ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);



        while(m_hours[0] == NULL){
            m_hours[0] = rcv_byte;
        };
        user_entry[0] = m_hours[0];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        rcv_byte = NULL;


        while(m_hours[1] == NULL){
            m_hours[1] = rcv_byte;
        };
        user_entry[1] = m_hours[1];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        if (m_hours[0] < '0' || m_hours[0] > '2' || m_hours[1] < '0' || m_hours[1] > '9' || (m_hours[0] == '0' && m_hours[1] == '0') || (m_hours[0] == '2' && (m_hours[1] < '0' || m_hours[1] > '4'))) {

            ST7735_DrawString(1, 10, "invalid number", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            __delay_cycles(48000000);  // delay for 1 second
            m_hours[0] = NULL;
            m_hours[1] = NULL;
            user_entry[0] = 'x';
            user_entry[1] = 'x';
            promptHours();
        }
        else {
            return;
        }
    }
}

/* prompt user to enter date */
void promptDate(void) {
    if (m_date[0] == NULL) {
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 9, "enter date (1-31)    ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 10, "              ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);



        while(m_date[0] == NULL){
            m_date[0] = rcv_byte;
        };
        user_entry[12] = m_date[0];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        rcv_byte = NULL;


        while(m_date[1] == NULL){
            m_date[1] = rcv_byte;
        };
        user_entry[13] = m_date[1];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        if (m_date[0] < '0' || m_date[0] > '3' || m_date[1] < '0' || m_date[1] > '9' || (m_date[0] == '0' && m_date[1] == '0') || (m_date[0] == '3' && (m_date[1] < '0' || m_date[1] > '1'))) {

            ST7735_DrawString(1, 10, "invalid number", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            __delay_cycles(48000000);  // delay for 1 second
            m_date[0] = NULL;
            m_date[1] = NULL;
            user_entry[12] = 'x';
            user_entry[13] = 'x';
            promptDate();
        }
        else {
            return;
        }
    }
}

/* prompt user to enter month */
void promptMonth(void) {
    if (m_month[0] == NULL) {
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 9, "enter month (1-12)    ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 10, "              ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);



        while(m_month[0] == NULL){
            m_month[0] = rcv_byte;
        };
        user_entry[9] = m_month[0];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        rcv_byte = NULL;


        while(m_month[1] == NULL){
            m_month[1] = rcv_byte;
        };
        user_entry[10] = m_month[1];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        if (m_month[0] < '0' || m_month[0] > '1' || m_month[1] < '0' || m_month[1] > '9' || (m_month[0] == '0' && m_month[1] == '0') || (m_month[0] == '1' && (m_month[1] < '0' || m_month[1] > '2'))) {

            ST7735_DrawString(1, 10, "invalid number", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            __delay_cycles(48000000);  // delay for 1 second
            m_month[0] = NULL;
            m_month[1] = NULL;
            user_entry[9] = 'x';
            user_entry[10] = 'x';
            promptMonth();
        }
        else {
            return;
        }
    }
}

/* prompt user to enter year */
void promptYear(void) {
    if (m_year[0] == NULL) {
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 9, "enter year (0-99)    ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
        ST7735_DrawString(1, 10, "              ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);



        while(m_year[0] == NULL){
            m_year[0] = rcv_byte;
        };
        user_entry[17] = m_year[0];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        rcv_byte = NULL;


        while(m_year[1] == NULL){
            m_year[1] = rcv_byte;
        };
        user_entry[18] = m_year[1];
        ST7735_DrawString(1, 8, user_entry, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        if (m_year[0] < '0' || m_year[0] > '9' || m_year[1] < '0' || m_year[1] > '9') {

            ST7735_DrawString(1, 10, "invalid number", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            __delay_cycles(48000000);  // delay for 1 second
            m_year[0] = NULL;
            m_year[1] = NULL;
            user_entry[17] = 'x';
            user_entry[18] = 'x';
            promptYear();
        }
        else {
            return;
        }
    }
}

//Push button interrupt on P1.1 or P1.4. When interrupt is triggered, sends data via I2C SDA line on port 1
void PORT1_IRQHandler(void){
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    if(status & GPIO_PIN1){

        DisplayStyle = DisplayStyle + 1;

        if(DisplayStyle == 3) DisplayStyle = 0;


    }

}

void InitButton(void){
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1 );
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 );
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 );
    MAP_Interrupt_enableInterrupt(INT_PORT1);
}

///* prompt user to enter day */
//void promptDay(void) {
//    if (day[0] == NULL) {
//        printf("please enter day (1-7, Sunday is 1) \n");
//
//        day[0] = getKey();
//
//        if (day[0] < '1' || day[0] > '7') {
//            printf("invalid number \n");
//            day[0] = NULL;
//            promptDay();
//        }
//        else {
//            printf("day enter: %s \n", day);
//            return;
//        }
//    }
//}


