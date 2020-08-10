#ifndef switches_h
#define switches_h
//*********************************************************
//
// switches.h
//
// Support for two switches on the Tiva/Orbit
// The switches are SW1 and Sw2
//
// Group FitnessWedGroup03
//
// author Erica Shipley(23108940), Nicholas Ranum(53147503), Yaxian Yu(25468326)
//
// Last modified: 29.05.2020
//*********************************************************

#include <stdint.h>
#include <stdbool.h>

// *******************************************************
// Constants
// *******************************************************
enum swtichName {RSW = 0, LSW, NUM_SWS};
#define NUM_SWI 2
#define NUM_BUT_POLLS 3
// Debounce algorithm: A state machine is associated with each switch.
// A state change occurs only after NUM_BUT_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_BUT_POLLS according to the polling rate.

// SW1
#define RSW_PERIPH SYSCTL_PERIPH_GPIOA
#define RSW_PORT_BASE GPIO_PORTA_BASE
#define RSW_PIN GPIO_PIN_7

// SW2
#define LSW_PERIPH SYSCTL_PERIPH_GPIOA
#define LSW_PORT_BASE GPIO_PORTA_BASE
#define LSW_PIN GPIO_PIN_6


// *******************************************************
//initSwitches:Initialise the variables associated with the set of switch
//
void initSwitches(void);

// *******************************************************
//updateSwitch: It polls switch once and update variable
//associated with the switch if necessary.It is efficient to be part of an ISR.
void updateSwitch(void);

// *******************************************************
//checkSwitch: function returns the new switch state if the
//button state has changes since the last call

bool checkSwitch (uint8_t switchName);

#endif /* switches_h */
