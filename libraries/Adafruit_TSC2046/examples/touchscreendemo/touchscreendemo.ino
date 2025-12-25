#include <Adafruit_TSC2046.h>

/* To use this library you will also need the [Adafruit BusIO]
   (https://www.arduino.cc/reference/en/libraries/adafruit-busio/)
   library and include it in your libraries folder.
   Instructions for how to install libraries in the Arduino IDE can be found at
   https://docs.arduino.cc/software/ide-v2/tutorials/ide-v2-installing-a-library

   Connections
   ===========
   Connect board SCK to breakout SCK.
   Connect board CIPO (MISO) to breakout MISO.
   Connect board COPI (MOSI) to breakout MOSI.
   Connect board SS (CS) to breakout CS
     You can instead use a different digital pin, but you must pass the pin
     number as the third argument to Adafruit_TSC2046::begin().
   IF you have a board with 5V-logic:
     Connect board 5V to breakout Vin
   IF you have a board with 3.3V-logic:
     Connect board 3V3 to breakout Vin
   Connect board GND to breakout GND

   Measurements
   ============
   Set your multimeter to measure resistance, place one probe on the pin-hole
   labled "X-" on the TSC2046 breakout, and the other probe on the pin-hole
   labled "X+". Your multimeter should show you a number in ohms (Ω), the unit
   for resistance. Pass that number as the first argument to
   Adafruit_TSC2046::begin().

   This example uses a 400Ω X-resistance.

 */


Adafruit_TSC2046 touchscreen;
#define TSC_CS         10  // chip select pin
#define TS_RESISTANCE 400  // For a 400Ω resistance across X- and X+.

void setup() {
  Serial.begin(115200);
  // Wait till serial connection opens for native USB devices
  while (!Serial) {
    delay(10);
  }

  Serial.println("Adafruit TSC2046 touchscreen demo");

  touchscreen.begin(TSC_CS, &SPI, TS_RESISTANCE);
  touchscreen.enableInterrupts(true);
}


// Prints the three coordinates in nice columns.
void displayTouchPoint(TSPoint point) {
  Serial.print(point.x);
  Serial.print("    ");
  Serial.print(point.y);
  Serial.print("    ");

  // Z measures the pressure; the value for Z *decreases* as the physical
  // pressure *increases*.
  Serial.print(point.z, 2);
  Serial.print(" ohms\n");
}

void loop() {
  delay(50); // Add delay to avoid overloading the serial monitor

  // Check that the touchscreen is being touched at all before getting
  // coordinates. Otherwise the coordinate values we get will be garbage.
  if (touchscreen.isTouched()) {
    TSPoint point = touchscreen.getPoint();
    displayTouchPoint(point);
  } else {
    // We can also monitor the ambient temperature around the chip,
    // in case you want to do something like check for overheating.
    // So if the touchscreen isn't being touched right now,
    // let's measure the temperature and print it over serial.
    float tempC = touchscreen.readTemperatureC();
    Serial.print(tempC);
    Serial.print(" C\t\t");

    Serial.print("Aux: ");
    Serial.print(touchscreen.readAuxiliaryVoltage());
    Serial.print(" V\t");

    Serial.print("Bat: ");
    Serial.print(touchscreen.readBatteryVoltage());
    Serial.println(" V\n");

  }

}
