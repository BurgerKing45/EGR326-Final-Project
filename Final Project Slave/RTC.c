/* driverLib includes */
#include "driverlib.h"

/* standard includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "Keypad.h"
#include "RTC.h"

#define RTC_ADDRESS 0x68    // RTC address from data sheet

/* variables for user keypad input */
char seconds[2];
char minutes[2];
char hours[2];
char day[1];
char date[2];
char month[2];
char year[2];
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

/* configuration for I2C communication */
const eUSCI_I2C_MasterConfig RTCi2cConfig =
{
 EUSCI_B_I2C_CLOCKSOURCE_SMCLK,     // SMCLK clock source
 12000000,                          // SMCLK = 12 MHz
 EUSCI_B_I2C_SET_DATA_RATE_400KBPS, // desired I2C clock of 400 kHz
 0,                                 // no byte counter threshold
 EUSCI_B_I2C_NO_AUTO_STOP           // no auto stop
};


/* initializes I2C module */
void initRTC(void) {

    // select Port 6 for I2C
    // set Pin 4, 5 to input Primary Module Function,
    // P6.4 is UCB1SDA, P6.5 is UCB1SCL
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
                                                   GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);

    // Initializing I2C Master
    MAP_I2C_initMaster(EUSCI_B1_BASE, &RTCi2cConfig);

    // Specify slave address
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, RTC_ADDRESS);

    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Enable I2C Module to start operations
    MAP_I2C_enableModule(EUSCI_B1_BASE);
}

/* prompts user to enter time and date info */
void promptUser(void) {

    promptSeconds();
    promptMinutes();
    promptHours();
    promptDay();
    promptDate();
    promptMonth();
    promptYear();

    convertAllToBCD();
    writeToRTC();
}

/* prompt user to enter seconds */
void promptSeconds(void) {
    if (seconds[0] == NULL) {
        printf("please enter seconds (0-59) \n");

        seconds[0] = getKey();
        seconds[1] = getKey();

        if (seconds[0] < '0' || seconds[0] > '5' || seconds[1] < '0' || seconds[1] > '9') {
            printf("invalid number \n");
            seconds[0] = NULL;
            seconds[1] = NULL;
            promptSeconds();
        }
        else {
            printf("seconds enter: %s \n", seconds);
            return;
        }
    }
}

/* prompt user to enter minutes*/
void promptMinutes(void) {
    if (minutes[0] == NULL) {
        printf("please enter minutes (0-59) \n");

        minutes[0] = getKey();
        minutes[1] = getKey();

        if (minutes[0] < '0' || minutes[0] > '5' || minutes[1] < '0' || minutes[1] > '9') {
            printf("invalid number \n");
            minutes[0] = NULL;
            minutes[1] = NULL;
            promptMinutes();
        }
        else {
            printf("minutes enter: %s \n", minutes);
            return;
        }
    }
}

/* prompt user to enter hours */
void promptHours(void) {
    if (hours[0] == NULL) {
        printf("please enter hours (1-24) \n");

        hours[0] = getKey();
        hours[1] = getKey();

        if (hours[0] < '0' || hours[0] > '2' || hours[1] < '0' || hours[1] > '9' || (hours[0] == '0' && hours[1] == '0') || (hours[0] == '2' && (hours[1] < '0' || hours[1] > '4'))) {
            printf("invalid number \n");
            hours[0] = NULL;
            hours[1] = NULL;
            promptHours();
        }
        else {
            printf("hours enter: %s \n", hours);
            return;
        }
    }
}

/* prompt user to enter day */
void promptDay(void) {
    if (day[0] == NULL) {
        printf("please enter day (1-7, Sunday is 1) \n");

        day[0] = getKey();

        if (day[0] < '1' || day[0] > '7') {
            printf("invalid number \n");
            day[0] = NULL;
            promptDay();
        }
        else {
            printf("day enter: %s \n", day);
            return;
        }
    }
}

/* prompt user to enter date */
void promptDate(void) {
    if (date[0] == NULL) {
        printf("please enter date (1-31) \n");

        date[0] = getKey();
        date[1] = getKey();

        if (date[0] < '0' || date[0] > '3' || date[1] < '0' || date[1] > '9' || (date[0] == '0' && date[1] == '0') || (date[0] == '3' && (date[1] < '0' || date[1] > '1'))) {
            printf("invalid number \n");
            date[0] = NULL;
            date[1] = NULL;
            promptDate();
        }
        else {
            printf("date enter: %s \n", date);
            return;
        }
    }
}

/* prompt user to enter month */
void promptMonth(void) {
    if (month[0] == NULL) {
        printf("please enter month (1-12) \n");

        month[0] = getKey();
        month[1] = getKey();

        if (month[0] < '0' || month[0] > '1' || month[1] < '0' || month[1] > '9' || (month[0] == '0' && month[1] == '0') || (month[0] == '1' && (month[1] < '0' || month[1] > '2'))) {
            printf("invalid number \n");
            month[0] = NULL;
            month[1] = NULL;
            promptMonth();
        }
        else {
            printf("month enter: %s \n", month);
            return;
        }
    }
}

/* prompt user to enter year */
void promptYear(void) {
    if (year[0] == NULL) {
        printf("please enter year (0-99) \n");

        year[0] = getKey();
        year[1] = getKey();

        if (year[0] < '0' || year[0] > '9' || year[1] < '0' || year[1] > '9') {
            printf("invalid number \n");
            year[0] = NULL;
            year[1] = NULL;
            promptYear();
        }
        else {
            printf("year enter: %s \n", year);
            return;
        }
    }
}

/* writes user entered date and time to RTC */
void writeToRTC(void) {

    /* Set Master in transmit mode */
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    // set pointer to beginning of RTC registers
    MAP_I2C_masterSendMultiByteStart(EUSCI_B1_BASE, 0x00);

    // write to time and day registers
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdSeconds);
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdMinutes);
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdHours);
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdDay);
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdDate);
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, bcdMonth);
    MAP_I2C_masterSendMultiByteFinish(EUSCI_B1_BASE, bcdYear);
}

/* reads  date and time from RTC */
void readFromRTC(void) {

    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    // set pointer to beginning of RTC registers
    MAP_I2C_masterSendSingleByte(EUSCI_B1_BASE, 0x00);

    // Wait for bus release
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    // Set Master in receive mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_MODE);

    // Wait for bus release, ready to receive
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    // read from RTC registers (pointer auto increments after each read)
    bcdSeconds = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    bcdMinutes = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    bcdHours = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    bcdDay = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    bcdDate = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    bcdMonth = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    bcdYear = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);


}

void ReadTemp(void)
{
    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);

    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    // set pointer to beginning of RTC temp register
    MAP_I2C_masterSendSingleByte(EUSCI_B1_BASE, 0x11);

    // Wait for bus release
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    // Set Master in receive mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_MODE);

    // Wait for bus release, ready to receive
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    // read from RTC registers (pointer auto increments after each read)
    TempInt = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    TempFrac = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);

}

//Converts the data from the two temperature registers to a single float
float convertTemp() {
    float final_temp;
    final_temp = ( (TempInt << 2) + (TempFrac >> 6) ) * 0.25;

    return final_temp;

}

/* convert two characters to BCD value */
uint8_t convertCharToBCD(char a, char b) {
    uint8_t a1 = ((a - 48) & 0x0F) << 4;
    uint8_t b1 = (b - 48) & 0x0F;
    return a1 + b1;
}

/* converts all character values to BCD values */
void convertAllToBCD (void) {
    bcdSeconds = convertCharToBCD(seconds[0], seconds[1]);
    bcdMinutes = convertCharToBCD(minutes[0], minutes[1]);
    bcdHours = convertCharToBCD(hours[0], hours[1]);
    bcdDay = convertCharToBCD('0', day[0] );
    bcdDate = convertCharToBCD(date[0], date[1]);

    bcdMonth = convertCharToBCD(month[0], month[1]);
    bcdYear = convertCharToBCD(year[0], year[1]);
}

/* convert BCD value to char values */
void convertBCDToChar(char (*ch)[2], uint8_t bcd) {
    (*ch)[1] = (bcd & 0x0F) + 48;
    (*ch)[0] = ((bcd & 0xF0) >> 4) + 48;
    return;

}

//Converts the BCD value for day to char.
void convertDayToChar(char (*ch)[1], uint8_t bcd) {
    (*ch)[0] = (bcd & 0x0F) + 48;
    return;

}

/* converts all BCD values to char values */
void convertAllToChar (void) {
    convertBCDToChar(&seconds, bcdSeconds);
    convertBCDToChar(&minutes, bcdMinutes);
    convertBCDToChar(&hours, bcdHours);
    convertDayToChar(&day, bcdDay);
    convertBCDToChar(&date, bcdDate);
    convertBCDToChar(&month, bcdMonth);
    convertBCDToChar(&year, bcdYear);
}


/* waits for user input to display date and time */
void waitForUser(void) {
    char userInput;

    printf("press the * key for display options \n");

    while (1) {
        userInput = getKey();
        if (userInput == '*') {
            printf("Press 1 for the day, month, and year \nPress 2 for the hour, minute, and second \nPress 3 for the temperature \n");
            displayDateTime();
        }
    }
}


/* display current date and time on RTC */
void displayDateTime(void) {
    float temp;
    char userInput;
    readFromRTC();      // first read is bad for some reason
    readFromRTC();
    ReadTemp();
    temp = convertTemp();
    convertAllToChar();
    convertToDay();


    userInput = getKey();

    if(userInput == '1'){
        printf("%s/%s/20%s %s \n", month, date, year, actual_day);

    }
    else if(userInput == '2'){
        printf("%s:%s:%s \n", hours, minutes, seconds);

    }
    else if(userInput == '3') {
        printf("%6.2f degrees Celsius \n", temp);
    }

    printf("press the * key for display options \n");
}


/* Covert the char representation of day to the day of the week */
void convertToDay(void)
{
    switch( (day[0] - 48) )
    {
    case 1:
        strcpy(actual_day,"Sunday");
        break;
    case 2:
        strcpy(actual_day,"Monday");
        break;
    case 3:
        strcpy(actual_day,"Tuesday");
        break;
    case 4:
        strcpy(actual_day,"Wednesday");
        break;
    case 5:
        strcpy(actual_day,"Thursday");
        break;
    case 6:
        strcpy(actual_day,"Friday");

    case 7:
        strcpy(actual_day,"Saturday");
        break;
    }
}


char* returnTimeDateInfo(void) {
    float temp;
    char userInput;
    char s[13] ="";

    readFromRTC();      // first read is bad for some reason
    readFromRTC();
    ReadTemp();          // this keeps data line high for some reason
    temp = convertTemp();
    convertAllToChar();
    convertToDay();


    strcat(s,seconds);
    strcat(s,minutes);
    strcat(s,hours);
    strcat(s,day);
    strcat(s,date);
    strcat(s,month);
    strcat(s,year);

    return s;

}

