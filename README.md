# PanicBrakeLights [Version 1.0]
=========================
A module to add automatic strobing panic (emergency) brake lights to a motorcycle for better visibility while stopping under unusual circumstances.

![alt text][pic1]
[pic1]: ./img/finalinstall.jpg "Finished product as installed on the motorcycle."

**Repository Contents**
=========================
* **/img** - Image files for the README file
* **/hardware** - Circuit design
* **/libraries** - Libraries used in the code
* **/src** - Project code (*.ino file for Arduino)
* **/LICENSE** - The user license file (open)
* **/README.md** - Project guide (this document)

**Requirements and Materials**
=========================

**Libraries required:**
* **Adafruit_NeoPixel.h** - to control LED strips
* **Wire.h** - for serial communication
* **Adafruit_Sensor.h** - for generic Adafruit sensor functionality
* **Adafruit_ADXL343.h** - for Adafruit ADXL343 accelerometer functionality
* **EEPROM.h** - for storing accelerometer data for thresholding

**Materials required:**
* Arduino-based microcontroller: Adafruit Metro Mini is used in my implementation (Product ID 2590)
* Adafruit ADXL343 Triple-Axis Accelerometer (Product ID 4097)
* Weather-resistant adhesive-backed LED strips: strips of WS2812B individually addressable LED modules will be used--they are individually addressable and can be cut to any length (as small as one LED element)
* Small breadboard
* Assorted bulk wire, jumper wires, and shrink tube
* Soldering station and soldering supplies such as solder, flux, desolding tools/wick, and a "third hand"


**Build Instructions**
=========================
Detailed directions on the build process will go here.


**Usage**
=========================
Once the hardware is assembled, perform the following operations to get it up and running:

* Install the Arduino IDE to program the Arduino-based microcontroller
* Install the requisite libraries listed in the /libraries folder
* Install the code listed in /src to the Arduino-based microcontroller
* Threshold the accelerometer to a level suitable for your needs


Credits
=========================
The inspiration for the project came from Instructables.com user *kristeller62* and their **Passive Brake Lights** project (https://www.instructables.com/id/passive-brake-light/).
