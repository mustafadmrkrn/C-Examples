void BlinkLed() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(SensPin, LOW);
    digitalWrite(RightLed, LOW);
    digitalWrite(LeftLed, LOW);
    delay(20);
    digitalWrite(SensPin, HIGH);
    digitalWrite(RightLed, HIGH);
    digitalWrite(LeftLed, HIGH);
    delay(20);
  }
}
void FinishLed() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(RightLed, HIGH);
    digitalWrite(LeftLed, LOW);
    delay(20);
    digitalWrite(RightLed, LOW);
    digitalWrite(LeftLed, HIGH);
    delay(20);
  }
}

void FlashLed() {
  for (int i = 0; i < 12; i++) {
    digitalWrite(RightLed, HIGH);
    digitalWrite(LeftLed, LOW);
    delay(20);
    digitalWrite(RightLed, LOW);
    digitalWrite(LeftLed, HIGH);
    delay(20);
  }
  digitalWrite(RightLed, LOW);
  digitalWrite(LeftLed, LOW);
}


void WaitLed() {
  for (int j = 255; j > 0; j--) {
    if (analogRead(Sws) > 800) {
      Start = 1;
      BlinkLed();
    }
    analogWrite(RightLed, j);
    analogWrite(LeftLed, j);
    delay(2);
  }
  for (int i = 0; i < 255; i++) {
    if (analogRead(Sws) > 800) {
      Start = 1;
      BlinkLed();
    }
    analogWrite(RightLed, i);
    analogWrite(LeftLed, i);
    delay(2);
  }
}
void MSensFlash() {
  for (int i = 0; i < 12; i++) {
    digitalWrite(RightLed, HIGH);
    digitalWrite(LeftLed, LOW);
    delay(20);
    digitalWrite(RightLed, LOW);
    digitalWrite(LeftLed, HIGH);
    delay(20);
  }

}
