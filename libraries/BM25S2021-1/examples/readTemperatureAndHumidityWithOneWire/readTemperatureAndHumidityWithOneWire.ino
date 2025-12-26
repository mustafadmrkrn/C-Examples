/*****************************************************************
File:        readTemperatureAndHumidityWithOneWire
Description: repeatedly obtain the temperature and humidity value 
             through OneWire and display the value in the serial port.
******************************************************************/
#include "BM25S2021-1.h"

BM25S2021_1 BMht(8);//select PIN8 as OneWire DataPin

void setup() {
    BMht.begin();
    Serial.begin(9600);             
}

void loop() {
    Serial.print("Humidity : ");
    Serial.print(BMht.readHumidity());
    Serial.print(" % ");
    
    Serial.print("Temperature : ");
    Serial.print(BMht.readTemperature());
    Serial.println(" °C ");
    delay(2000); 
}
