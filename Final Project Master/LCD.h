

void UpdateDisplay(uint16_t speed, char *TimeDate, uint8_t DisplayStyle);
void parseTimeDate(char string[]);
void ClearDisplay(void);
void MenuDisplay(void);
void DisplayTime(char time[] );
void DisplayAlarmLog(void);
void DisplaySpeedLog(void);
void IdleDisplay(void);
void ClearLowerDisplay(void);

void PrintProxWarning(void);
void ClearProxWarning(void);
void PrintTempWarning(void);
void ClearTempWarning(void);
void DisplayOutsideTemp(float OutTemp);
