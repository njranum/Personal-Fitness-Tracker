//*********************************************************
//
// Main.c
//
// Main module for running the program
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
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "acc.h"
#include "i2c_driver.h"
#include "buttons4.h"
#include "math.h"
#include "Accelerometer.h"
#include "Display.h"
#include "switches.h"
#include "buttonPresses.h"
#include "circBufT.h"

//********************************************************
// Constants
//********************************************************
#define BUF_SIZE 5
#define SYSTICK_RATE_HZ 100
#define DISPLAYTICK_RATE_HZ 4
#define ACCELTICK_RATE_HZ 15
#define BUTTONTICK_RATE_HZ 10

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_inBufferx; // Buffer of size BUF_SIZE integers (sample values)
static circBuf_t g_inBuffery;
static circBuf_t g_inBufferz;
volatile uint8_t displayTick = false;
volatile uint8_t accelTick = false;
volatile uint8_t buttonTick = false;

/*******************************************
 *      Local prototypes
 *******************************************/
void initClock(void);
void initDisplay(void);
void displayUpdate(char *str1, char *str2, int16_t num, uint8_t charLine);
void initAccl(void);
void initSysTick(void);
vector3_t getAcclData(void);
void SysTickIntHandler(void);

void initClock(void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
    SYSCTL_XTAL_16MHZ);
}

void SysTickIntHandler(void)
{

    static uint8_t displayCount = 0;
    static uint8_t accelCount = 0;
    static uint8_t buttonCount = 0;

    const uint8_t ticksPerDisplay = SYSTICK_RATE_HZ / DISPLAYTICK_RATE_HZ;
    const uint8_t ticksPerAccel = SYSTICK_RATE_HZ / ACCELTICK_RATE_HZ;
    const uint8_t ticksPerButton = SYSTICK_RATE_HZ / BUTTONTICK_RATE_HZ;

    if (++displayCount >= ticksPerDisplay)
    {                       // Signal a slow tick
        displayCount = 0;
        displayTick = true;
    }
    if (++accelCount >= ticksPerAccel)
    {
        accelCount = 0;
        accelTick = true;
    }
    if (++buttonCount >= ticksPerButton) {
        buttonCount = 0;
        buttonTick = true;
    }
}

void initSysTick(void)
{
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SYSTICK_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

int16_t accelerationNorm(void) {
    // Takes the most recent data from the accelerometer, writes it to the
    // buffer and calculates the mean value of the buffer.
    // Returns the norm of the mean value for the x, y and z directions.
    vector3_t acceleration_raw;

    int16_t accel_x = 0;
    int16_t accel_y = 0;
    int16_t accel_z = 0;
    int16_t mean_x = 0;
    int16_t mean_y = 0;
    int16_t mean_z = 0;
    int i;
    //Calculates the norm of the accelerometer data.
    acceleration_raw = getAcclData();
    writeCircBuf(&g_inBufferx, acceleration_raw.x);
    writeCircBuf(&g_inBuffery, acceleration_raw.y);
    writeCircBuf(&g_inBufferz, acceleration_raw.z);

    accel_x = 0;
    accel_y = 0;
    accel_z = 0;

    for (i = 0; i < BUF_SIZE; i++)
    {
        accel_x = accel_x + readCircBuf(&g_inBufferx);
        accel_y = accel_y + readCircBuf(&g_inBuffery);
        accel_z = accel_z + readCircBuf(&g_inBufferz);
    }
    mean_x = (2 * accel_x + BUF_SIZE) / 2 / BUF_SIZE;
    mean_y = (2 * accel_y + BUF_SIZE) / 2 / BUF_SIZE;
    mean_z = (2 * accel_z + BUF_SIZE) / 2 / BUF_SIZE;

    int16_t accel_norm = sqrt(
            mean_x * mean_x + mean_y * mean_y + mean_z * mean_z);

    return accel_norm;

}



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~       MAIN        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(void)
{
    // initializations
    initClock();
    initAccl();
    initDisplay();
    initButtons();
    initSwitches();
    initSysTick();

    initCircBuf(&g_inBufferx, BUF_SIZE);
    initCircBuf(&g_inBuffery, BUF_SIZE);
    initCircBuf(&g_inBufferz, BUF_SIZE);

    // Variables
    int16_t accel_norm;
    int step = 0;
    int flag = 0;
    int16_t distance = 0;


    int upPressed = 0;
    int leftRightState = 0;
    bool testMode;
    int testFlag = 1;
    int longDown = 0;
    int miles = 0;

    char distance_buffer[17];

    OLEDStringDraw("Fitness Project", 0, 0);

    while (1)
    {

        if (buttonTick) { // The buttons are polled at a rate of 10Hz.
            upPressed = upPushed(upPressed);
            leftRightState = leftRightPushed(leftRightState);
            testMode = checkSwitch(LSW);
            buttonTick = false;
            longDown = longDownPress();
        }

        if (accelTick) // The norm acceleration is calculated and the steps updated at approx 50Hz.
        {
            accel_norm = accelerationNorm();
            if ((accel_norm > 300) && (flag == 0)) {
                flag = 1;
                step++;
            } else {
                flag = 0;
            }
            accelTick = false;
        }

        if (displayTick) // The display is updated at a rate of approx 4Hz.
        {
            distance = 9 * step;
            miles = distance * 0.6214;

            // Checks if the long press on down button is performed. If so resets step count
            // unless in test mode.
            if ((longDown == 1) & (testMode == 0)) {
                step = 0;
            } else if ((longDown == 2) & (testMode == 1))  {
                step -= 500;
                if (step < 0) {
                    step = 0;
                }
            }

            if (testMode == 1) // Program enters test mode as SW1 is up.
            {
                if ((upPressed == 1) & (testFlag == 1)) {
                    step += 100;
                    testFlag = 0;
                }
                if ((upPressed == 0) & (testFlag == 0)) {
                    step += 100;
                    testFlag = 1;
                }
                if (leftRightState == 0)  { // Display the step count in test mode.
                    displayUpdate("TEST STEP", "", step, 2);
                }
                else {  // Display the distance in test mode
                    OLEDStringDraw("                ", 0, 2);
                    usnprintf(distance_buffer, sizeof(distance_buffer),"TEST KM: %d.%04d", distance / 10000, distance % 10000);
                    OLEDStringDraw(distance_buffer, 0, 2);
                }
            }
            else if (leftRightState == 0) // Display the step count in regular mode.
            {
                displayUpdate("STEP COUNT", "", step, 2);
            }
            else { // Display the distance in regular mode.
                if (upPressed == 0) // Display distance in kilometers.
                {
                    usnprintf(distance_buffer, sizeof(distance_buffer), "Km: %d.%04d", distance / 10000, distance % 10000);
                } else  { //Display distance in miles.
                    usnprintf(distance_buffer, sizeof(distance_buffer), "Miles : %d.%04d", miles / 10000, miles % 10000);
                }
            OLEDStringDraw("                ", 0, 2);
            OLEDStringDraw(distance_buffer, 0, 2);
            }
        displayTick = false;

        }
    }
}
