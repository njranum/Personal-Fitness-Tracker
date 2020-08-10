// *********************************************************
//
// switches.c
//
// Support for two switches on the Tiva/Orbit
// The switches are SW1 and Sw2
//
// Group FitnessWedGroup03
//
// author Erica Shipley(23108940), Nicholas Ranum(53147503), Yaxian Yu(25468326)
//
// Last modified: 29.05.2020
// *********************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)
#include "switches.h"

// *******************************************************
// Globals to module
// *******************************************************
static bool sw_state[NUM_SWI];
static uint8_t sw_count[NUM_SWI];

// *******************************************************
//initSwitches:Initialise the variables associated with the set of switch
// defined by the constants in the switches.h file.
void initSwitches(void){
    // LSW slide switch
    SysCtlPeripheralEnable (LSW_PERIPH);
    GPIOPinTypeGPIOInput (LSW_PORT_BASE, LSW_PIN);
    GPIOPadConfigSet (LSW_PORT_BASE, LSW_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

    // RSW slide switch
    SysCtlPeripheralEnable (RSW_PERIPH);
    GPIOPinTypeGPIOInput (RSW_PORT_BASE, RSW_PIN);
    GPIOPadConfigSet (RSW_PORT_BASE, RSW_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

    sw_state[0] = 0;
    sw_state[1] = 0;

}

// *******************************************************
// updateSwitch: It polls switch once and update variable
// associated with the switch if necessary.It is efficient to be part of an ISR.
// Debounce algorithm: A state machine is associated with each switch.
// A state change occurs only after NUM_BUT_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_BUT_POLLS according to the polling rate.
void updateSwitch(void)
{
    bool sw_value[NUM_SWI];
    int i;

    // Read the pins; true means HIGH, false means LOW
    sw_value[RSW] = (GPIOPinRead (LSW_PORT_BASE, LSW_PIN) == LSW_PIN);
    sw_value[LSW] = (GPIOPinRead (RSW_PORT_BASE, RSW_PIN) == RSW_PIN);
    // Iterate through the buttons, updating button variables as required
    for (i = 0; i < NUM_SWI; i++)
    {
        if (sw_value[i] != sw_state[i])
        {
            sw_count[i]++;
            if (sw_count[i] >= NUM_BUT_POLLS)
            {
                sw_state[i] = sw_value[i];
                sw_count[i] = 0;
            }
        }
        else{
            sw_count[i] = 0;
        }
    }
}

// *******************************************************
//checkSwitch: function returns the new switch state if the
//switch state has changes since the last call

bool checkSwitch (uint8_t switchName)
{
    updateSwitch();
    if (sw_state[switchName] == 1)
        return true;
    else
        return false;

}

