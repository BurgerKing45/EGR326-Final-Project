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

void ClearDisplay(void){
    ST7735_FillScreen(0);
    ST7735_FillRect(0, 150, 128, 10, ST7735_Color565(0xFF, 0xFF, 0xFF));
}

void UpdateDisplay(uint16_t speed){

    char TimeDate[19];
    char formattedTimeDate[21];
    char formattedSpeed[6];
    char formattedTemp[8];

    // Converting and displaying the time and date
    strcpy(TimeDate, returnTimeDateInfo() );
    parseTimeDate(TimeDate);
    snprintf(formattedTimeDate, sizeof formattedTimeDate, "%s:%s:%s %s/%s/20%s ", lcd_hours, lcd_minutes, lcd_seconds, lcd_month, lcd_date, lcd_year);

    ST7735_DrawString(1, 1, formattedTimeDate, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

    strncpy(lcd_temp, TimeDate + 13, 5);  // this had to be moved here for some reason

    // Converting and displaying the temperature
    snprintf(formattedTemp, sizeof formattedTemp, "%sF", lcd_temp);
    ST7735_DrawString(7, 3, formattedTemp, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

    // Converting and displaying the speed
    snprintf(formattedSpeed, sizeof formattedSpeed, "%dmph", speed);
    ST7735_DrawString(6, 6, formattedSpeed, ST7735_Color565(0xFF, 0xFF, 0xFF), 2, 0);


}

void IdleDisplay(void){
    ST7735_DrawString(1, 12, "Press the * key for", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(6, 13, "the menu.", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
}

void MenuDisplay(void){
    ST7735_DrawString(1, 8, "1) Set Time", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 9, "2) Check Alarm Log", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 10,"3) Check Speed Log", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 11, "4) Exit", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
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

}

void DisplaySpeedLog(void){
    //char Log[65];       // string of time and date log

    //strcpy(Log, ReadFromFlash(2));  // store current log in flash as string

    FlashDisplay(ReadFromFlash(2));

}


void ExitDisplay(void){
    ST7735_DrawString(1, 14, "Press # to exit", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
}

