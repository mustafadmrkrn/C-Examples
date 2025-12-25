#include <Servo.h>

Servo myServo;
const int joystickX = A0;

void setup() {
  myServo.attach(9);  // Servo motor sinyal pini D9
  Serial.begin(9600);
}

void loop() {
  int xValue = analogRead(joystickX);  // 0 - 1023 arası
  int angle = map(xValue, 0, 1023, 0, 180);  // Joystick değerini 0-180 dereceye çevir

  myServo.write(angle);  // Servo motoru döndür
  Serial.print("Joystick: ");
  Serial.print(xValue);
  Serial.print(" -> Servo Açısı: ");
  Serial.println(angle);

  delay(15);  // Servo tepkisini rahatlat
}
