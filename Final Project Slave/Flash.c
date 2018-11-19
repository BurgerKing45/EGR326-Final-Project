/* Authors: Sam Kirk and Ben Burger
 * Assignment: EGR 326 Memory Library
 * Professor: Prof. Krug
 * Date: 11/05/18
 * Notes:
 */


/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/* Custom libraries for the keypad, RTC, and LCD */
#include "Keypad.h"
#include "RTC.h"
#include "ST7735.h"
#include "Flash.h"

/* Strings used for time and date info */
char seconds[2];
char minutes[2];
char hours[2];
char day[1];
char date[2];
char month[2];
char year[2];


#define CALIBRATION_START 0x000200000  // memory address for flash memory
uint8_t* addr_pointer;    // pointer to address in flash for reading/writing



//int main(void)
//{
//
//    MAP_WDT_A_holdTimer();  // stop watchdog timer
//
//    clockInit48MHzXTL();    // setting MCLK to 48MHz for faster clock speed
//
//    initKeypad();                   // initialize keypad pins
//    initRTC();                      // initialize I2C communication for RTC
//    ST7735_InitR(INITR_REDTAB);     // initialize the LCD
//
//    PromptUser();
//}

/* Waits for user input to display date and time. */
void PromptUser(void) {
    char userInput;
    char TimeDate[13];  // current date and time string
    char Log[65];       // string of time and date log

    /* Prompt the user over the LCD */
    ST7735_FillScreen(0);
    ST7735_DrawString(1, 4, "press * key for", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 5, "current date & time", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 7, "press # key for", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
    ST7735_DrawString(1, 8, "date & time log", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

    while(1) {

        userInput = getKey();       // wait for user input

        if (userInput == '*') {

            /* get current time and date info by reading RTC, store as string */
            strcpy(TimeDate, returnTimeDateInfo() );
            parseString(TimeDate);      // parse the tine and date string

            /* format time and date info, print to LCD, prompt user */
            char formattedTime[20];
            snprintf(formattedTime, sizeof formattedTime, "%s:%s:%s %s/%s/20%s", hours, minutes, seconds, month, date, year);
            ST7735_FillScreen(0);
            ST7735_DrawString(1, 4, formattedTime, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            ST7735_DrawString(1, 6, "press * key for", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            ST7735_DrawString(1, 7, "current date & time", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            ST7735_DrawString(1, 9, "press # key for", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            ST7735_DrawString(1, 10, "date & time log", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

            WriteToFlash(TimeDate);     // write current date and time to log in flash
            userInput = NULL;

        }

        if (userInput == '#') {

            strcpy(Log, ReadFromFlash());  // store current log in flash as string

            /* parse first entry in the log, display on LCD */
            parseString(Log);
            char entry1[20];
            snprintf(entry1, sizeof entry1, "%s:%s:%s %s/%s/20%s", hours, minutes, seconds, month, date, year);
            ST7735_FillScreen(0);
            ST7735_DrawString(1, 1, entry1, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

            /* parse second entry in the log, display on LCD */
            parseString(Log + 13);
            char entry2[20];
            snprintf(entry2, sizeof entry2, "%s:%s:%s %s/%s/20%s", hours, minutes, seconds, month, date, year);
            ST7735_DrawString(1, 2, entry2, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

            /* parse third entry in the log, display on LCD */
            parseString(Log + 26);
            char entry3[20];
            snprintf(entry3, sizeof entry3, "%s:%s:%s %s/%s/20%s", hours, minutes, seconds, month, date, year);
            ST7735_DrawString(1, 3, entry3, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

            /* parse fourth entry in the log, display on LCD */
            parseString(Log + 39);
            char entry4[20];
            snprintf(entry4, sizeof entry4, "%s:%s:%s %s/%s/20%s", hours, minutes, seconds, month, date, year);
            ST7735_DrawString(1, 4, entry4, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

            /* parse fifth entry in the log, display on LCD */
            parseString(Log + 52);
            char entry5[20];
            snprintf(entry5, sizeof entry5, "%s:%s:%s %s/%s/20%s", hours, minutes, seconds, month, date, year);
            ST7735_DrawString(1, 5, entry5, ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

            /* prompt user */
            ST7735_DrawString(1, 7, "press * key for", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            ST7735_DrawString(1, 8, "current date & time", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            ST7735_DrawString(1, 10, "press # key for", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);
            ST7735_DrawString(1, 11, "date & time log", ST7735_Color565(0xFF, 0xFF, 0xFF), 1, 0);

            userInput = NULL;
        }
    }
}

/* Reads time and date log from flash memory. */
char* ReadFromFlash(void){
    uint8_t i; // current byte index in flash
    uint8_t FlashData[65];  // string to store flash data

    /* set pointer to beginning of sector, skip first 4 bytes */
    addr_pointer = CALIBRATION_START + 4;

    /* read 65 bytes from the sector */
    for(i=0; i<65; i++) {
        FlashData[i] = *addr_pointer++;
    }

    return FlashData;
}

/* Updates the flash with the new current time and date entry */
void WriteToFlash( char* InfoToWrite){
    uint8_t i;      // current byte index in flash
    uint8_t currentFlash[65];   // string to store flash data

    /* set pointer to beginning of sector, skip first 4 bytes */
    addr_pointer = CALIBRATION_START + 4;

    /* read current 65 bytes from the sector */
    for(i=0; i<65; i++) {
        currentFlash[i] = *addr_pointer++;
    }

    /* Unprotecting Info Bank 0, Sector 0 */
    MAP_FlashCtl_unprotectSector(FLASH_INFO_MEMORY_SPACE_BANK0,FLASH_SECTOR0);

    /* Erase the flash sector starting at beginning of sector. */
    while(!MAP_FlashCtl_eraseSector(CALIBRATION_START));

    /* Program the flash with the current time and date info. */
    while (!MAP_FlashCtl_programMemory(InfoToWrite, (void*) CALIBRATION_START + 4, 13 ));

    /* Program the rest of the flash with the previous 4 log entries. */
    while (!MAP_FlashCtl_programMemory(currentFlash, (void*) CALIBRATION_START+17, 52 ));

    /* Setting the sector back to protected */
    MAP_FlashCtl_protectSector(FLASH_INFO_MEMORY_SPACE_BANK0,FLASH_SECTOR0);
}



/* Parses a string into the separate time and date strings. */
void parseString(char string[]) {
    strncpy(seconds, string, 2);
    strncpy(minutes, string + 2, 2);
    strncpy(hours, string + 4, 2);
    strncpy(day, string + 6, 1);
    strncpy(date, string + 7, 2);
    strncpy(month, string + 9, 2);
    strncpy(year, string + 11, 2);
}

