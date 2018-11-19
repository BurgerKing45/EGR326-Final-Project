
/* Function Prototypes */
void PromptUser(void);
void WriteToFlash( char* InfoToWrite, uint8_t WhichLog);
char* ReadFromFlash(uint8_t WhichLog);
void parseString(char string[13]);
void FlashDisplay(char Log[]);
