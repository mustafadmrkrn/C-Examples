
void EncoderStep() {


  currentStateL_CLK = digitalRead(L_CLK);
  currentStateR_CLK = digitalRead(R_CLK);

  if (  currentStateR_CLK != lastStateR_CLK  && currentStateR_CLK == 1 ) {
    if (digitalRead(R_DT) != currentStateR_CLK) {
      counterR ++;
      currentDirR = "CCW";
    } else {
      counterR --;
      currentDirR = "CW";
    }
    /*
      Serial.print("Direction: ");
      Serial.print(currentDirR);
      Serial.print(" | Counter: ");
      Serial.println(counterR);
    */
  }
  if (currentStateL_CLK != lastStateL_CLK  && currentStateL_CLK == 1   ) {
    if (digitalRead(L_DT) != currentStateL_CLK) {
      counterL --;
      currentDirL = "CCW";
    } else {
      counterL ++;
      currentDirL = "CW";
    }
  }
  lastStateL_CLK = currentStateL_CLK;
  lastStateR_CLK = currentStateR_CLK;

}

int EncoderReset() {
  counterR = 0;
  counterL = 0;
  TurnSpeedR = 100;
  TurnSpeedL = 100;
  MLStop = 100;
  MRStop = 100;
  M_PWMdeg = 0;
  ForwR = 0;
  ForwL = 0;
  ForwMiniR = 0;
  ForwMiniL = 0;
  Right90L = 0;
  Right90R = 0;
}
