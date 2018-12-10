/* Authors: Sam Kirk and Ben Burger
 * Assignment: EGR 326 LCD Commands
 * Professor: Prof. Krug
 * Date: 11/04/18
 * Notes:
 */



/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/*Custom Libraries */
#include "ST7735.h"
#include "RTC.h"
#include "Flash.h"

/* Global Variables */

/* Strings used for time and date info */
char lcd_seconds[2];
char lcd_minutes[2];
char lcd_hours[2];
char lcd_day[1];
char lcd_date[2];
char lcd_month[2];
char lcd_year[2];
char lcd_temp[4];

void parseTimeDate(char string[]);

void DisplayOutsideTemp(float OutTemp){
   char formattedOutTemp[9];

    snprintf(formattedOutTemp, sizeof formattedOutTemp, "%6.3fC ", OutTemp);

    ST7735_DrawString(4, 9, formattedOutTemp, ST7735_Color565(0xFF, 0xFF, 0xFF), 2, 0);

    ST7735_DrawString(1, 13, "Press # to exit", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

}

void ClearDisplay(void){
    ST7735_FillScreen(0);
    //ST7735_FillRect(0, 150, 128, 10, ST7735_Color565(0xFF, 0xFF, 0xFF));
}

void ClearLowerDisplay(void){
    ST7735_FillRect(0, 80, 128, 60, 0);
}

void PrintProxWarning(void){
    ST7735_DrawString(0, 14, "  Collision Warning", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
}

void ClearProxWarning(void){
    ST7735_DrawString(0, 14, "                   ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
}

void PrintTempWarning(void){
    ST7735_DrawString(0, 15, " Temperature Warning", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
}

void ClearTempWarning(void){
    ST7735_DrawString(0, 15, "                      ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
}


void UpdateDisplay(uint16_t speed, char *TimeDate, uint8_t DisplayStyle){

    char formattedTimeDate[21];
    char formattedSpeed[9];
    char formattedTemp[9];
    char justTime[10];

    // Converting and displaying the time and date
    // strcpy(TimeDate, returnTimeDateInfo() );
    parseTimeDate(TimeDate);
    snprintf(formattedTimeDate, sizeof formattedTimeDate, "%s:%s:%s %s/%s/20%s ", lcd_hours, lcd_minutes, lcd_seconds, lcd_month, lcd_date, lcd_year);

    strncpy(lcd_temp, TimeDate + 13, 5);  // this had to be moved here for some reason

    // Converting and displaying the temperature
    snprintf(formattedTemp, sizeof formattedTemp, "%sC ", lcd_temp);

    // Converting and displaying the speed
    snprintf(formattedSpeed, sizeof formattedSpeed, " %dmph    ", speed);

    //Emphasis on Speed
    if(DisplayStyle == 0){


        ST7735_DrawString(7, 3, formattedTemp, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);


        ST7735_DrawString(1, 1, formattedTimeDate, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);


        ST7735_DrawString(4, 6, formattedSpeed, ST7735_Color565(0xFF, 0xFF, 0xFF), 2, 0);

    }

    //Emphasis on Temp
    else if(DisplayStyle == 1){

        ST7735_DrawString(7, 3, formattedSpeed, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);


        ST7735_DrawString(1, 1, formattedTimeDate, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);


        ST7735_DrawString(4, 6, formattedTemp, ST7735_Color565(0xFF, 0xFF, 0xFF), 2, 0);

    }

    //Emphasis on Time
    else if(DisplayStyle == 2){

        ST7735_DrawString(7, 3, formattedTemp, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);


        ST7735_DrawString(1, 1, formattedSpeed, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

        strncpy(justTime, formattedTimeDate, 9);


        ST7735_DrawString(4, 6, justTime, ST7735_Color565(0xFF, 0xFF, 0xFF), 2, 0);
        Timer32_enableInterrupt(TIMER32_0_BASE);
    }
}

void IdleDisplay(void){
    ST7735_DrawString(1, 12, "Press the * key for", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(6, 13, "the menu.", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
}

void MenuDisplay(void){
    ST7735_DrawString(1, 8, "1) Set Time", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 9, "2) Check Alarm Log", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 10,"3) Check Speed Log", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 11, "4) Outside Temp", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 12, "5) Exit              ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 13, "                         ", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

}

void parseTimeDate(char string[]) {
    strncpy(lcd_seconds, string, 2);
    strncpy(lcd_minutes, string + 2, 2);
    strncpy(lcd_hours, string + 4, 2);
    strncpy(lcd_day, string + 6, 1);
    strncpy(lcd_date, string + 7, 2);
    strncpy(lcd_month, string + 9, 2);
    strncpy(lcd_year, string + 11, 2);
}


void DisplayTime(char time[]){

    ST7735_DrawString(1, 9, time, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
}

void DisplayAlarmLog(void){
    //char Log[65];       // string of time and date log

    // strcpy(Log, ReadFromFlash(1));  // store current log in flash as string

    FlashDisplay( ReadFromFlash(1) );

    ST7735_DrawString(1, 13, "Press # to exit", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

}

void DisplaySpeedLog(void){
    //char Log[65];       // string of time and date log

    //strcpy(Log, ReadFromFlash(2));  // store current log in flash as string

    FlashDisplay(ReadFromFlash(2));

    ST7735_DrawString(1, 13, "Press # to exit", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

}



