#include <Servo.h>

Servo myServo;

const int joyX = A0;
const int servoPin = 9;
const int buzzerPin = 8;

void setup() {
  myServo.attach(servoPin);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int xValue = analogRead(joyX);
  int angle = map(xValue, 0, 1023, 0, 180);

  myServo.write(angle);
  delay(15);

  // Buzzer sınır açı kontrolü
  if (angle <= 10 || angle >= 170) {
    digitalWrite(buzzerPin, HIGH); // Buzzer çalsın
  } else {
    digitalWrite(buzzerPin, LOW);  // Sessiz olsun
  }

  Serial.print("X: ");
  Serial.print(xValue);
  Serial.print(" -> Açı: ");
  Serial.println(angle);
}
