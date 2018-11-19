
void writeToRTC(void);
void readFromRTC(void);
void initRTC(void);
uint8_t convertCharToBCD(char a, char b);
void convertAllToBCD(void);
void convertBCDToChar(char (*ch)[2], uint8_t bcd);
void convertAllToChar(void);
void displayDateTime(void);
void convertToDay(void);
void convertDayToChar(char (*ch)[1], uint8_t bcd);
void ReadTemp(void);
float convertTempBCDToChar(void);
char* returnTimeDateInfo(void);
