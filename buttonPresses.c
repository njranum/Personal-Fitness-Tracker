//*********************************************************
//
// buttonPresses.c
//
// Support for checking the up,left,right,down button state
//
// Group FitnessWedGroup03
//
// author Erica Shipley(23108940), Nicholas Ranum(53147503), Yaxian Yu(25468326)
//
// Last modified: 29.05.2020
//*********************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "driverlib/adc.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "i2c_driver.h"
#include "buttons4.h"
#include "math.h"
#include "Display.h"
#include <stdbool.h>

// *******************************************************
//upPushed: function designed to check if the up button
//is pushed or released, it returns the button state
int upPushed(int upState) {
    int switcher = upState;
    updateButtons();
    uint8_t butStateUp;
    butStateUp = checkButton (UP);
    switch(butStateUp)
    {
    case PUSHED:
        if (switcher == 0) {
            switcher = 1;
        } else {
            switcher = 0;
        }
    case RELEASED:
        break;
    }
    return switcher;
}

// *******************************************************
//leftRightPushed: function returns the current state of the left button and
//right button
int leftRightPushed(int currentState) {
    int switcher = currentState;
    updateButtons ();   // Poll the buttons
    // check state of each button and display if a change is detected
    uint8_t butStateLeft;
    butStateLeft = checkButton (LEFT);
    uint8_t butStateRight;
    butStateRight = checkButton (RIGHT);

    switch (butStateLeft)
    {
        case PUSHED:
            if (switcher == 0) {
                switcher = 1;
                      } else {
                          switcher = 0;
                      }
            break;
        case RELEASED:
            break;
        // Do nothing if state is NO_CHANGE
    }
    switch (butStateRight)
    {
        case PUSHED:
            if (switcher == 0) {
                switcher = 1;
            } else {
                switcher = 0;
                break;
            }
        case RELEASED:
            break;
        // Do nothing if state is NO_CHANGE
    }
    return switcher;
}

// *******************************************************
//longDownPress: function returns the state of the down button, it represents 'not pressed', 'short pressed' or 'long pressed' if it returns 0, 2 or 1 respectively.
int longDownPress(void) {
    updateButtons();
    int pressed = checkButton (DOWN);

    uint32_t count = 0;
    int longPress = 0;

    while(pressed == PUSHED) {
        SysCtlDelay (SysCtlClockGet () / 30);

        updateButtons();
        int state = checkButton (DOWN);
            if (count >= 12) {
                longPress = 1;
                pressed = 0;
            } else {
                longPress = 2;
            }
        if (state == RELEASED) {
            pressed = 0;
        }
        count ++;
    }
    return longPress;
}


