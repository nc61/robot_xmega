/*
 * motors.h
 *
 * Created: 9/24/2013 9:31:01 PM
 *  Author: Nick
 */ 


#ifndef MOTORS_H_
#define MOTORS_H_


//This function is used when the user wants to control an individual motor
void set_motor(char motor, uint8_t duty_cycle, uint8_t dir);

//This function initializes a counter for PWM and a counter for acceleration
void TC_init();

//Sets the right motor forward and left motor backward at equal duty cycle
void pivot_left(uint8_t duty_cycle);

//Sets the left motor forward and right motor backward at equal duty cycle
void pivot_right(uint8_t duty_cycle);

//Stops the motors
void stop_motors();

#endif /* MOTORS_H_ */