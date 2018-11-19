
/*
 * Updates the stepper motor. Inputs: Current speed.
 */
void Update_Speedometer(uint16_t speed);

/*Intializes the stepper motor and resets it to 0
 *
 */
void InitStepper(void);

/*Stepper controller function. Inputs: Number of steps and direction
 * Output: stepper motor moves
*/
void stepper_move(uint16_t steps, uint8_t direction);
