/* driverLib includes */
#include "driverlib.h"

/* standard includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "Keypad.h"
#include "RTC.h"
#include "Final Project Master main.h"

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
 3000000,                          // SMCLK = 12 MHz
 EUSCI_B_I2C_SET_DATA_RATE_100KBPS, // desired I2C clock of 400 kHz
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


/* writes user entered date and time to RTC */
void writeToRTC(void) {

    convertAllToBCD();

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
    bcdSeconds = convertCharToBCD(m_seconds[0], m_seconds[1]);
    bcdMinutes = convertCharToBCD(m_minutes[0], m_minutes[1]);
    bcdHours = convertCharToBCD(m_hours[0], m_hours[1]);
    bcdDay = convertCharToBCD('0', '1');
    bcdDate = convertCharToBCD(m_date[0], m_date[1]);
    bcdMonth = convertCharToBCD(m_month[0], m_month[1]);
    bcdYear = convertCharToBCD(m_year[0], m_year[1]);
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
    char s[19] ="";
    char stringTemp[6];

    readFromRTC();      // first read is bad for some reason
    readFromRTC();
    ReadTemp();          // this keeps data line high for some reason
    temp = convertTemp();
    snprintf(stringTemp, sizeof stringTemp, "%f", temp);
    convertAllToChar();
    convertToDay();


    strcat(s,seconds);
    strcat(s,minutes);
    strcat(s,hours);
    strcat(s,day);
    strcat(s,date);
    strcat(s,month);
    strcat(s,year);
    strcat(s,stringTemp);

    return s;
}

