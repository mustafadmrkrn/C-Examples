#define MQ2pin (0)
float sensorValue; 
void setup()
{
  Serial.begin(9600); // sets the serial port to 9600
  Serial.println("Gas sensor warming up!");
  delay(2000); // allow the MQ-6 to warm up
}
void loop()
{
  sensorValue = analogRead(MQ2pin); // read analog input pin 0
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  if(sensorValue > 200)
  {
    Serial.print(" | Smoke detected!");
  }
  Serial.println("");
  delay(200); // wait 2s for next reading
}