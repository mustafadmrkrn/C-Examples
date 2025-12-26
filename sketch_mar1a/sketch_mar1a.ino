#include <Wire.h>
#include <DHT.h>
#include <MPU6050.h>

#define redLed 8
#define greenLed 10
#define yellowLed 9
#define DHTPIN 3  // DHT11'in bağlı olduğu pin
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
MPU6050 mpu;
int tempdeger = 0;
int analogpin = A0;
int sure = 50;

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  Serial.begin(115200);
  dht.begin();
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 bağlantı başarısız!");
  }
}

void loop() {
  tempdeger = analogRead(analogpin);
  delay(sure);

  float tempC = dht.readTemperature();
  Serial.print("DHT11 Sicaklik: ");
  Serial.print(tempC);
  Serial.print(" C, ");

  if (tempC < 25) {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
  } else if (tempC >= 25 && tempC <= 30) {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, LOW);
  } else {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, HIGH);
  }


 int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    Serial.print("MPU6050 Accel: ");
    Serial.print(" X="); Serial.print(ax);
    Serial.print(" Y="); Serial.print(ay);
    Serial.print(" Z="); Serial.println(az);
    Serial.print("MPU6050 Gyro: ");
    Serial.print(" X="); Serial.print(gx);
    Serial.print(" Y="); Serial.print(gy);
    Serial.print(" Z="); Serial.println(gz);

  delay(500);
}