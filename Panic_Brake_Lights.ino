/*
 * Panic Brake Lights
 * 
 * Purpose: To automatically activate strobing lights, affixed to the
 *    rear of a motorcycle, under situations of great braking force.
 *    
 * Setup:
 * 
 * Created  November  8, 2019
 * Modified November 17, 2019
 * Modified November 23, 2019
 * 
 * By Ayden Ell
 * 
 */

// Adafruit NeoPixel best practices (for most reliable operation):
// - Add 1000 uF 6.3V CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//     connect GROUND (-) first, then +, then data.
// - If your microcontroller and NeoPixels are powered from two different 
//     sources, there must be a ground connection between the two.

#include <Adafruit_NeoPixel.h>  // to control LED strips
#include <Wire.h>               // to use serial communication
#include <Adafruit_Sensor.h>    // for generic Adafruit sensor functionality
#include <Adafruit_ADXL343.h>   // for Adafruit ADXL343 accelerometer functionality

// Constants to reference LED strips
const int LEDL_PIN = 5;
const int LEDR_PIN = 6;
const int LEDL_COUNT = 3;
const int LEDR_COUNT = 3;

// Other constants
const float THRESHOLD = -3;
const int BRIGHTNESS = 127;   // LED strip brightness (range 0 - 255)
const int SHORT_BLINK = 50;   // duration for the LED strip OFF cycle
const int LONG_BLINK = 200;   // duration for the LED strip ON cycle
const int CYCLES = 50;        // number of strobe cycles on triggered event
const int EXTRA_TIME = 5000;  // duration to continue strobing after braking ceases
const int ELEMENT_DELAY = 1;  // default delay between LED strip elements activating
const int DELAY = 10;         // default loop delay

// 32-bit (HEX) colours (to be used by NeoPixel elements)
const uint32_t BLACK = 0x000000;
const uint32_t RED = 0xFF0000;
const uint32_t GREEN = 0x00FF00;
const uint32_t BLUE = 0x0000FF;
const uint32_t WHITE = 0xFFFFFF;
const uint32_t FUCHSIA = 0xFF00FF;
const uint32_t AQUA = 0x00FFFF;
const uint32_t YELLOW = 0xFFFF00;

// Declare NeoPixel objects
Adafruit_NeoPixel stripL(LEDL_COUNT, LEDL_PIN); // outer left strip
Adafruit_NeoPixel stripR(LEDR_COUNT, LEDR_PIN); // outer right strip

// Declare sensor objects
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);
sensors_event_t event;

void setup()
{
  Serial.begin(9600);
  accel.begin();

  // Set accelerometer range to 
  // accel.setRange(ADXL343_RANGE_16_G);
  // accel.setRange(ADXL343_RANGE_8_G);
  // accel.setRange(ADXL343_RANGE_4_G);
  accel.setRange(ADXL343_RANGE_2_G);
  
  // Left outer strip setup
  stripL.begin();                   // Initialize NeoPixel strip object
  stripL.show();                    // Initialize to "off" (showing black)
  stripL.setBrightness(BRIGHTNESS); // Set strip brightness (max 255)

  // Right outer strip setup
  stripR.begin();                   // Initialize NeoPixel strip object
  stripR.show();                    // Initialize to "off" (showing black)
  stripR.setBrightness(BRIGHTNESS); // Set strip brightness (max 255)
}

void loop() 
{
  
  accel.getEvent(&event);       // get sensor event
  float X = event.acceleration.x; // get new X acceleration
  float Y = event.acceleration.y; // get new Y acceleration
  float Z = event.acceleration.z; // get new Z acceleration

  Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
  delay(100);

  if(X <= THRESHOLD)
    for (int i = 0; i < CYCLES; i++)
      Strobe(stripL, stripR, RED);
  
  //Wipe(stripL,RED,10);
  //colorWipe(RED,100);
  //delay(100);
  //stripL.clear();
  //stripL.show();
  //delay(50);
  //brightnessTest(stripL, RED);
}

void brightnessTest(Adafruit_NeoPixel & strip, uint32_t colour)
{
  for(int i = 0; i < strip.numPixels(); i++)
    strip.setPixelColor(i, colour);

  for(int i = 1; i <= 5; i++)
  {
    strip.setBrightness(i * 50);
    strip.show();
    delay(500);
  }
  for(int i = 4; i > 1; i--)
  {
    strip.setBrightness(i * 50);
    strip.show();
    delay(500);
  }
}

/*  Wipe
 *  Purpose:  To sequentially fill a LED strip one element at a time with a delay.
 *  Parameters:
 *    <1> strip - the strip object to fill, passed by reference
 *    <2> colour - the hex colour with which to fill the strip
 *    <3> wait - the wait time between elements (in milliseconds)
 *  Returns: void
 */
void Wipe(Adafruit_NeoPixel & strip, uint32_t colour, int wait)
{
  for(int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, colour);
    strip.show();
    delay(wait);
  }
}

/*  Strobe
 *  Purpose: To strobe both left and right LED strips.
 *  Parameters:
 *    <1> strip1 - the first strip object to fill, passed by reference
 *    <2> strip2 - the second strip object to fill, passed by reference
 *  Returns: void
 */
void Strobe(Adafruit_NeoPixel & strip1, Adafruit_NeoPixel & strip2, uint32_t colour)
{
  // Fill both strips with given colour
  for(int i = 0; i < strip1.numPixels(); i++)
  {
    strip1.setPixelColor(i, colour);
    strip1.show();
    strip2.setPixelColor(i, colour);
    strip2.show();
    delay(ELEMENT_DELAY);
  }
  delay(LONG_BLINK);
  
  // Clear both strips for short duration
  strip1.clear();
  strip1.show();
  strip2.clear();
  strip2.show();
  delay(SHORT_BLINK);
}
