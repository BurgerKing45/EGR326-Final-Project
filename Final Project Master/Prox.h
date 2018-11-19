
/*Initializes the proximity sensor on P2.4 with Timer A0.
 * P5.0 set as GPIO for trigger pin
 */
void InitProx();

/* Pulses the trigger pin for 10us.
 */
void PulseProx(void);

/* Given the global variables for measurement from the prox ISR handler,
 * returns a flag, 1 if object within 15cm, 0 if no objects detected
 * within 15cm.
 */
uint8_t CheckProx(uint32_t meas1, uint32_t meas2);
