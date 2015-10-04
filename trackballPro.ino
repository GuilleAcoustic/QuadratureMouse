/* ================================================================================
   Author  : GuilleAcoustic
   Date    : 2015-05-22
   Revision: V1.0
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
   ================================================================================ */

// =================================================================================
// Type definition
// =================================================================================
typedef struct
{
  int8_t  coordinate = 0;
  uint8_t index      = 0;
} ENCODER_;

// =================================================================================
// Constant definition
// =================================================================================
const int8_t lookupTable[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1,  1,  0};

// =================================================================================
// Volatile variables
// =================================================================================
volatile ENCODER_ xAxis;
volatile ENCODER_ yAxis;
volatile boolean leftButton   = false;
volatile boolean middleButton = false;
volatile boolean rightButton  = false;

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
  if (!(PINB & 0b1000) && !leftButton)
  {
    Mouse.press(MOUSE_LEFT);
    leftButton = true;
  }
  else
  {
    if ((PINB & 0b1000) && leftButton)
    {
      Mouse.release(MOUSE_LEFT);
      leftButton = false;
    }
  }

  // ---------------------------------
  // Right mouse button state update
  // ---------------------------------  
  if (!(PINB & 0b0100) && !rightButton)
  {
    Mouse.press(MOUSE_RIGHT);
    rightButton = true;
  }
  else
  {
    if ((PINB & 0b0100) && rightButton)
    {
      Mouse.release(MOUSE_RIGHT);
      rightButton = false;
    }
  }
  
  // ---------------------------------
  // Middle mouse button state update
  // ---------------------------------
  if (!(PINB & 0b0010) && !middleButton)
  {
    Mouse.press(MOUSE_MIDDLE);
    middleButton = true;
  }
  else
  {
    if ((PINB & 0b0010) && middleButton)
    {
      Mouse.release(MOUSE_MIDDLE);
      middleButton = false;
    }
  }

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
