
/*
 * This function will set the piezo buzzer to operate at a specific frequency
 * Inputs: The desired frequency of the buzzer in Hz.
 * Output: A really annoying alarm from the buzzer
 * Note: P2.4 TimerA0 and P10.5 TimerA3 used. Overall frequency of buzzer set to
 * 4000Hz, because that's loud.
 */
void StartBuzzer(uint32_t Freq);

/*
 * This function shuts the buzzer up
 */
void StopBuzzer(void);

/*
 * This function initializes the external oscillators, and three clocks.
 * MCLK = 48MHz
 * SMCLK = 12MHz
 * ACLK = 32678Hz. No idea why, but I'm using it.
 */
void clockInit48MHzXTL(void);
