
/* initialize keypad, use P4.0 - P4.6 */
void initKeypad(void);

/* scans keypad for input, if none then NULL is returned */
char scanKeypad(void);

/* returns key pressed */
char getKey (void);


