
void Left90Turn() {
  digitalWrite(RightLed, HIGH);
  digitalWrite(LeftLed, LOW);
  digitalWrite(L_Mdir, HIGH);
  analogWrite(L_Mpwm, TurnSpeedL - TolL);
  digitalWrite(R_Mdir, HIGH);
  analogWrite(R_Mpwm, TurnSpeedR - TolR);
  EncoderStep();
  counterLCM = counterL / -(CountLVal);
  counterRCM = counterR / CountRVal;
  if (counterLCM == 50) {
    TurnSpeedL = 0;
    Left90L = 0;
  }
  if (counterRCM == 50) {
    TurnSpeedR = 0;
    Left90R = 0;
  }

  /*
    Serial.print("Direction: ");
    Serial.print(currentDirR);
    Serial.print(" | Counter: ");
    Serial.println(counterR);
  */
}
void ForwOneStep() {
  digitalWrite(RightLed, HIGH);
  digitalWrite(LeftLed, HIGH);
  digitalWrite(L_Mdir, LOW);
  analogWrite(L_Mpwm, TurnSpeedL - TolL);
  digitalWrite(R_Mdir, HIGH);
  analogWrite(R_Mpwm, TurnSpeedR - TolR);
  EncoderStep();
  counterLCM = counterL / CountLVal;
  counterRCM = counterR / CountRVal;
  if (counterLCM == 100) {
    TurnSpeedL = 0;
    ForwL = 1;
  }
  if (counterRCM == 100) {
    TurnSpeedR = 0;
    ForwR = 1;
  }
}
void ForwMiniStep() {
  digitalWrite(RightLed, HIGH);
  digitalWrite(LeftLed, LOW);
  digitalWrite(L_Mdir, LOW);
  analogWrite(L_Mpwm, TurnSpeedL - TolL);
  digitalWrite(R_Mdir, HIGH);
  analogWrite(R_Mpwm, TurnSpeedR - TolR);
  EncoderStep();
  counterLCM = counterL / CountLVal;
  counterRCM = counterR / CountRVal;
  if (counterLCM == 50) {
    TurnSpeedL = 0;
    ForwMiniL = 1;
  }
  if (counterRCM == 50) {
    TurnSpeedR = 0;
    ForwMiniR = 1;
  }
}
void Right90Turn() {
  digitalWrite(RightLed, LOW);
  digitalWrite(LeftLed, HIGH);
  digitalWrite(L_Mdir, LOW);
  analogWrite(L_Mpwm, TurnSpeedL - TolL);
  digitalWrite(R_Mdir, LOW);
  analogWrite(R_Mpwm, TurnSpeedR - TolR);
  EncoderStep();
  counterLCM = counterL / CountLVal;
  counterRCM = counterR / -(CountRVal);
  if (counterLCM == 50 ) {
    TurnSpeedL = 0;
    Right90L = 1;
  }
  if (counterRCM == 50) {
    TurnSpeedR = 0;
    Right90R = 1;
  }

}
void MotorStop() {
  digitalWrite(L_Mdir, LOW);
  analogWrite(L_Mpwm, 0);
  digitalWrite(R_Mdir, HIGH);
  analogWrite(R_Mpwm, 0);
  delay(1);
}
void AutoStop() {
  MSensDeg();
  digitalWrite(7, LOW);
  analogWrite(9, MLStop  - (M_PWMdeg));
  digitalWrite(8, HIGH);
  analogWrite(10, MRStop - (M_PWMdeg));

}
