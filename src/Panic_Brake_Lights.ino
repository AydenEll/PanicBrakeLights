/*
 * Panic Brake Lights
 * 
 * Purpose: To automatically activate strobing lights, affixed to the rear of a
 *          motorcycle, under situations of greater than normal braking force.
 *    
 * Required Hardware: 
 *        - Adafruit Metro Mini controller board is used
 *        - Adafruit ADXL343 Analog Devices accelerometer breakout board
 *        - LM2596 DC to DC buck converter to step down 12V supply from motorcycle
 *            to 5V for Metro Mini and LED strips
 *        - Strips of WS2812B LED modules to make up left and right side lights
 *            - each elements can be individually addressed
 *            - e.g. Adafruit NeoPixel strips
 * Setup:
 *        - Metro Mini Pin 5 to 560ohm resistor then to left LED strip data pin
 *        - Metro Mini Pin 6 to 560ohm resistor then to right LED strip data pin
 *        - Metro Mini Pin A5 (SCL) to ADXL343 clock pin
 *        - Metro Mini Pin A4 (SDA) to ADXL343 data pin
 *        - Metro Mini +5V out to Vin on ADXL343
 *        - Metro Mini GND to GND on ADXL343
 *        - +5V supply to Metro Mini Vin and +5V on left and right LED strip
 *        - GND from power supply shared to GND on Metro Mini and each LED strip
 * 
 * Created  November  8, 2019 (basic NeoPixel functionality tested)
 * Modified November 17, 2019 (NeoPixel refinements)
 * Modified November 23, 2019 (incorporated accelerometer code)
 * Modified November 30, 2019 (first setup on actual hardware and more refinements)
 * Modified December  4, 2019 (updated Strobe function constants; removed Wipe function)
 * Modified December  5, 2019 (helper functions for thresholding; removed BrightnessTest function)
 * 
 * By Ayden Ell
 * 
 */

// Import libraries for LEDs, sensor, and serial communication
#include <Adafruit_NeoPixel.h>  // to control LED strips
#include <Wire.h>               // to use serial communication
#include <Adafruit_Sensor.h>    // for generic Adafruit sensor functionality
#include <Adafruit_ADXL343.h>   // for Adafruit ADXL343 accelerometer functionality
#include <EEPROM.h>             // for storing accelerometer data for thresholding

// Constants to reference LED strips
const int LEDL_PIN = 5;   // Left LED strip pin
const int LEDR_PIN = 6;   // Right LED strip pin
const int LEDL_COUNT = 24; // Left LED count
const int LEDR_COUNT = 24; // Right LED count

// Other constants
//*** Proper threshold still needs to be determined for accelerometer X axis ***//
const double THRESHOLD = -3.0; // Accelerometer threshold to trigger strobing effect
const uint32_t RED = 0xFF0000; // HEX colour red to be used by NeoPixel elements
const int BRIGHTNESS = 127;   // LED strip brightness (range 0 - 255)
const int DELAY = 10;         // default loop delay
const int CYCLES = 20;        // number of strobe cycles on triggered event (500ms each)
const int SHORT_BLINK = 50;   // duration for the LED strip OFF cycle
const int LONG_BLINK = 202;   // duration for the LED strip ON cycle
const int ELEMENT_DELAY = 2;  // default delay between LED strip elements activating
// Note: LONG_BLINK + (24 * ELEMENT_DELAY) should total 250ms
//       e.g. if ELEMENT_DELAY == 1, then LONG_BLINK = 226
//            else if ELEMENT_DELAY == 2, then LONG_BLINK = 202

// Global variable to store EEPROM memory location "pointer"
//  to aid in thresholding (used by helper functions)
int memoryLocation = 2;

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

  // Set accelerometer range to +- 2G
  accel.setRange(ADXL343_RANGE_2_G);
  
  // Left outer strip setup
  stripL.begin();                   // Initialize NeoPixel strip object
  stripL.show();                    // Initialize to "off" (showing black)
  stripL.setBrightness(BRIGHTNESS); // Set strip brightness (max 255)

  // Right outer strip setup
  stripR.begin();                   // Initialize NeoPixel strip object
  stripR.show();                    // Initialize to "off" (showing black)
  stripR.setBrightness(BRIGHTNESS); // Set strip brightness (max 255)

  // Read memory location (stored at 0) from EEPROM and save in MEMORY_LOCATION
  EEPROM.get(0, memoryLocation);
}

void loop() 
{
  // Get new accelerometer sensor event and acceleration for each axis
  accel.getEvent(&event);
  double X = event.acceleration.x;
  double Y = event.acceleration.y;
  double Z = event.acceleration.z;

  // Three useful helper functions to debug and threshold the accelerometer
  //  can be enabled and disabled as necessary (turn all off in final build)
  //PrintAcceleration(X, Y, Z);
  StoreAcceleration(X);
  //PrintEEPROM();

  // If negative X acceleration is less than the threshold (breaking hard),
  //  activate strobing lights (each strobe cycle takes 500ms)
  if(X <= THRESHOLD)
    for (int i = 0; i < CYCLES; i++)
      Strobe(stripL, stripR, RED);

  delay(DELAY*25); //temporarily increased to 250ms for thresholding
}

/*  PrintAcceleration
 *  Purpose: Print accelerometer data to serial console 
 *    for thresholding and debugging
 *  Parameters:
 *    <1> x - the X-axis acceleration
 *    <2> x - the Y-axis acceleration
 *    <3> x - the Z-axis acceleration
 *  Returns: void
 */
void PrintAcceleration(double x, double y, double z)
{
  Serial.print("X: "); Serial.print(x);
  Serial.print("  Y: "); Serial.print(y);
  Serial.print("  Z: "); Serial.print(z);
  Serial.println("  (m/s^2)");
}

/*  StoreAcceleration
 *  Purpose: Store acceleration data into EEPROM if it is of interest 
 *    but only stores acceleration values less than -1.0 and if EEPROM not full
 *  Parameters:
 *    <1> X - value to be stored
 *  Returns: void
 */
void StoreAcceleration(double value)
{
  // If negative acceleration is less than -1.0, store value in memory,
  //   increment memory location pointer, and store new pointer at address 0
  if((value < -1.0) && (memoryLocation < 1000)) // 1024 bytes of EEPROM
  {
    EEPROM.put(memoryLocation, value); // store "value" at memoryLocation
    memoryLocation += 8; // increment location by 8 bytes (for double)
    EEPROM.put(0, memoryLocation); // store new memory location at 0
  }
}

/*  PrintEEPROM
 *  Purpose: Prints contents of EEPROM and resets memory pointer back to beginning (2)
 *  Parameters: n/a
 *  Returns: void
 */
void PrintEEPROM()
{
  // Print accelerometer data to the serial console
  for(int i = 2; i <= memoryLocation - 8; i += 8)
  {
    float temp;
    EEPROM.get(i, temp);
    Serial.print("X: ");
    Serial.print(temp);
    Serial.print("m/s^2 at location ");
    Serial.println(i);
  }
  // Reset memory location back to the start of the EEPROM
  memoryLocation = 2;
  EEPROM.put(0, memoryLocation);
}

/*  Strobe
 *  Purpose: To strobe both left and right LED strips; total time per cycle is 500ms.
 *  Parameters:
 *    <1> strip1 - the first strip object to fill, passed by reference
 *    <2> strip2 - the second strip object to fill, passed by reference
 *  Returns: void
 */
void Strobe(Adafruit_NeoPixel & strip1, Adafruit_NeoPixel & strip2, uint32_t colour)
{  
  // Fill both strips with given colour with small delay (looks animated)
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

  // Quickly flash the lights twice (kind of a "chirping" effect)
  for (int i = 0; i < 2; i++)
  {
    // Fill both strips with given colour without delay between elements
    for(int j = 0; i < strip1.numPixels(); i++)
    {
      strip1.setPixelColor(i, colour);
      strip1.show();
      strip2.setPixelColor(i, colour);
      strip2.show();
    }
    delay(SHORT_BLINK);

    // Clear both strips for short duration
    strip1.clear();
    strip1.show();
    strip2.clear();
    strip2.show();
    delay(SHORT_BLINK);
  }
}
