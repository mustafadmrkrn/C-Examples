/*****************************************************************
File:        readTemperatureAndHumidityWithIIC
Description: Power on to obtain the equipment information once, 
             including (SN/PID/VER), and then repeatedly obtain 
             the temperature and humidity value through IIC and 
             display the value in the serial port.
******************************************************************/
#include "BM25S2021-1.h"
BM25S2021_1 BMht(&Wire);

void setup() 
{
    BMht.begin(); 
    Serial.begin(9600);   
    Serial.println("=======DeviceInfo(HEX)========= ");
    Serial.print("SN : ");
    Serial.println(BMht.getSN(), HEX);
    Serial.print("PID : ");
    Serial.println(BMht.getPID(), HEX);
    Serial.print("VER : ");
    Serial.println(BMht.getFWVer(), HEX);  
    Serial.println("=============================== ");       
    delay(2000);
}

void loop() 
{
    Serial.print("Humidity : ");
    Serial.print(BMht.readHumidity());
    Serial.print(" %    ");
    Serial.print("Temperature : ");
    Serial.print(BMht.readTemperature());
    Serial.println(" °C ");
    delay(2000);
}
