//*********************************************************
//
// buttonPresses.h
//
// Support for checking the up,left,right,down button state
//
// Group FitnessWedGroup03
//
// author Erica Shipley(23108940), Nicholas Ranum(53147503), Yaxian Yu(25468326)
//
// Last modified: 29.05.2020
//*********************************************************

#ifndef BUTTONPRESSES_H_
#define BUTTONPRESSES_H_

#include <stdint.h>
#include <stdbool.h>


// *******************************************************
//upPushed: function designed to check if the up button
//is pushed or released, it returns the button state
int upPushed(int upState);

// *******************************************************
//leftRightPushed: function returns the current state of the left button and
//right button
//
int leftRightPushed(int currentState);

// *******************************************************
//longDownPress: function returns the state of the down button, it represents 'not pressed', 'short pressed' or 'long pressed' if it returns 0, 2 or 1 respectively.
int longDownPress(void);

//int downPushed(void);

#endif /* BUTTONPRESSES_H_ */
