
void SensorDeg() {
  digitalWrite(SensPin, HIGH);
  R_ReelDeg = analogRead(RSens);
  M_ReelDeg = analogRead(MSens);
  L_ReelDeg = analogRead(LSens);

  QTRStop(); // QTRStop Function
  
  /*
    Serial.print("Reel R Sens: ");
    Serial.println(R_ReelDeg);
  */
  // Right Wall
  if (R_ReelDeg > R_WallRange ) {
    R_Deg = (R_SensRange - R_WallRange) / 2 ;
    R_Deg = R_Deg + R_WallRange;
    R_PWMC = R_ReelDeg - R_Deg;
    R_PWMC = map(R_PWMC, R_PWM_N, R_PWM_P, 30, -30);
    if (R_PWMC > 0) {
      digitalWrite(RightLed, HIGH);
    } else {
      digitalWrite(RightLed, LOW);
    }
    if (L_ReelDeg > L_WallRange) {
      digitalWrite(LeftLed, LOW);
    } else {
      digitalWrite(LeftLed, HIGH);
    }
    // Front Wall
    if (M_ReelDeg > M_WallRange ) {
      R_PWMC = 0;
      M_Deg = (M_SensRange - M_WallRange) / 2;
      M_Deg = M_Deg + M_WallRange;
      M_PWMC = M_ReelDeg - M_Deg;
      M_PWMC = map(M_PWMC, M_PWM_N, M_PWM_P, 0, 100);
      M_PWMC = M_PWMC - 15;
      if (M_PWMC > 100) {
        M_PWMdeg = 100;
        //*86455656
      } else {
        M_PWMdeg = M_PWMC;
      }
      if (M_PWMdeg <= 110 && M_PWMdeg >= 90) {

        //

        Left90L = 1;
        Left90R = 1;
      }
      // Serial.print("Reel M Sens: ");
      //  Serial.println(M_PWMdeg);
    }
    // Left Wall
    NoWall = 0;
  } else {
    NoWall = 1;
  }



}

void MSensDeg() {
  digitalWrite(SensPin, HIGH);
  R_ReelDeg = analogRead(RSens);
  M_ReelDeg = analogRead(MSens);
  L_ReelDeg = analogRead(LSens);
  if (M_ReelDeg > M_WallRange ) {
    M_Deg = (M_SensRange - M_WallRange) / 2;
    M_Deg = M_Deg + M_WallRange;
    M_PWMC = M_ReelDeg - M_Deg;
    M_PWMC = map(M_PWMC, M_PWM_N, M_PWM_P, 0, 100);
    M_PWMC = M_PWMC - 15;
    if (M_PWMC > 100) {
      M_PWMdeg = 100;
      Finish = 1;
      //*86458765245655656
    } else {
      M_PWMdeg = M_PWMC;
    }

  }

}

void QTRStop() {

  int qtrValue = analogRead(QtrSens);

  // Her renkte çalışması için geniş aralık belirleme
  if (qtrValue > 300 && qtrValue < 600) { 
    Finish = 1;
    Serial.print("QTR Sensör Değeri: ");
    Serial.println(qtrValue);
    
    // Motoru durdur
    while (Finish == 1) {
      MotorStop();
      FinishLed();
    }
  }


}
