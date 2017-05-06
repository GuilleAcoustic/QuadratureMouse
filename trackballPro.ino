#include <Mouse.h>

/* ================================================================================
   Author  : GuilleAcoustic
   Date    : 2015-05-22
   Revision: V1.1
   Purpose : Opto-mechanical trackball firmware
   --------------------------------------------------------------------------------
   Wiring informations: Sparkfun Pro micro (Atmega32u4)
   --------------------------------------------------------------------------------
     - Red    : Gnd                          |   Pin: Gnd
     - Orange : Vcc (+5V)                    |   Pin: Vcc
     - Yellow : X axis encoder / channel A   |   Pin: PD3 - (INT0)
     - Green  : X axis encoder / channel B   |   Pin: PD2 - (INT1)
     - Blue   : Y axis encoder / channel A   |   Pin: PD0 - (INT2)
     - Violet : Y axis encoder / channel B   |   Pin: PD1 - (INT3)
     - Grey   : Switch 1                     |   Pin: PB3
     - White  : Switch 2                     |   Pin: PB2
     - Black  : Switch 3                     |   Pin: PB1
   --------------------------------------------------------------------------------
   Latest additions:
     - 2016-01-28: Software switch debouncing
   ================================================================================ */

// =================================================================================
// Type definition
// =================================================================================

#ifndef DEBOUNCE_THREASHOLD
#define DEBOUNCE_THREASHOLD 50
#endif

// =================================================================================
// Type definition
// =================================================================================
typedef struct ENCODER_
{
    int8_t  coordinate;
    uint8_t index;
} ENCODER_;

typedef struct BUTTON_
{
    boolean state;
    boolean needUpdate;
    char    button;
    byte    bitmask;
    long    lastDebounceTime;
} BUTTON_;

// =================================================================================
// Constant definition
// =================================================================================
const int8_t lookupTable[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1,  1,  0};

// =================================================================================
// Volatile variables
// =================================================================================
volatile ENCODER_ xAxis = {0, 0};
volatile ENCODER_ yAxis = {0, 0};

// =================================================================================
// Global variables
// =================================================================================
BUTTON_ leftButton   = {false, false, MOUSE_LEFT,   0b1000, 0};
BUTTON_ middleButton = {false, false, MOUSE_MIDDLE, 0b0010, 0};
BUTTON_ rightButton  = {false, false, MOUSE_RIGHT,  0b0100, 0};

// =================================================================================
// Setup function
// =================================================================================
void setup()
{
    // Attach interruption to encoders channels
    attachInterrupt(0, ISR_HANDLER_X, CHANGE);
    attachInterrupt(1, ISR_HANDLER_X, CHANGE);
    attachInterrupt(2, ISR_HANDLER_Y, CHANGE);
    attachInterrupt(3, ISR_HANDLER_Y, CHANGE);

    // Start the mouse function
    Mouse.begin();
}

// =================================================================================
// Main program loop
// =================================================================================
void loop()
{
    // Update mouse coordinates
    if (xAxis.coordinate != 0 || yAxis.coordinate != 0)
    {
        Mouse.move(xAxis.coordinate, yAxis.coordinate);
        xAxis.coordinate = 0;
        yAxis.coordinate = 0;
    }

    // ---------------------------------
    // Left mouse button state update
    // ---------------------------------
    ReadButton(leftButton);
    UpdateButton(leftButton);

    // ---------------------------------
    // Right mouse button state update
    // ---------------------------------
    ReadButton(rightButton);
    UpdateButton(rightButton);

    // ---------------------------------
    // Middle mouse button state update
    // ---------------------------------
    ReadButton(middleButton);
    UpdateButton(middleButton);

    // Wait a little before next update
    delay(10);
}

// =================================================================================
// Interrupt handlers
// =================================================================================
void ISR_HANDLER_X()
{
    // Build the LUT index from previous and new data
    xAxis.index       = (xAxis.index << 2) | ((PIND & 0b00000011) >> 0);
    xAxis.coordinate += lookupTable[xAxis.index & 0b00001111];
}

void ISR_HANDLER_Y()
{
    // Build the LUT index from previous and new data
    yAxis.index       = (yAxis.index << 2) | ((PIND & 0b00001100) >> 2);
    yAxis.coordinate += lookupTable[yAxis.index & 0b00001111];
}

// =================================================================================
// Functions
// =================================================================================
void ReadButton(BUTTON_& button)
{
    // Variables
    long    currentime  = millis();
    boolean switchState = !(PINB & button.bitmask);
    boolean debounced   = (currentime - button.lastDebounceTime > DEBOUNCE_THREASHOLD);

    // Button state acquisition
    if ((switchState != button.state) && debounced)
    {
        button.lastDebounceTime = currentime;
        button.state            = switchState;
        button.needUpdate       = true;
    }
}

void UpdateButton(BUTTON_& button)
{
    if (button.needUpdate)
    {
        (button.state) ? Mouse.press(button.button) : Mouse.release(button.button);
        button.needUpdate = false;
    }
}
