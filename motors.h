/*
 * motors.h
 *
 * Created: 9/24/2013 9:31:01 PM
 *  Author: Nick
 */ 


#ifndef MOTORS_H_
#define MOTORS_H_

void set_motors(uint8_t duty_cycle, uint8_t dir);
void TC_init();

#endif /* MOTORS_H_ */