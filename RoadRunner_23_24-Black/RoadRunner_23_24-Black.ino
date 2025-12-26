#define L_CLK 4
#define L_DT 2
#define R_CLK 5
#define R_DT 3
#define L_Mdir 7
#define L_Mpwm 9
#define R_Mdir 8
#define R_Mpwm 10
#define LSens A2
#define MSens A1
#define RSens A0
#define SensPin 12
#define Sws A6
#define QtrSens A3
#define VoltageSens A7
//20000636738S03CRLTY
double CountRVal = 1.80; // Left Motor CM Value
double CountLVal = 1.80; // Right Motor CM Value

// Number Of Right Turns
int RightTurnVal = 0;
int FinishRightTurn = 12;

//Sensor Function
int L_SensRange  = 350;
int M_SensRange = 400; // 400
int R_SensRange = 350;
int NoWall = 0;
int Start = 0;
int Finish = 0;
int L_WallRange = 40;
int M_WallRange = 90;
int R_WallRange = 40;

int L_ReelDeg = 0;
int M_ReelDeg = 0;
int R_ReelDeg = 0;
int L_Deg = 0;
int M_Deg = 0;
int R_Deg = 0;
// Led Pins

int LeftLed = 11;
int RightLed = 6;

// Turn Degrees
int Left90L = 0;
int Left90R = 0;
int Right90L = 0;
int Right90R = 0;
int ForwL = 0;
int ForwR = 0;
int ForwMiniL = 0;
int ForwMiniR = 0;
int Right90 = 0;
int Left180 = 0;
int TurnSpeedL = 90;
int TurnSpeedR = 90;
int TolL =0; //Left Turn Tolerance Negative Value
int TolR = 10; //Right Turn Tolerance Negative Value
int TurnWait = 500;
// Motor And Encoder Function
double counterL = 0;
double counterR = 0;
int currentStateL_CLK;
int lastStateL_CLK;
int currentStateR_CLK;
int lastStateR_CLK;
String currentDirL = "";
String currentDirR = "";
int CDirR ;
int MLStop = 100;
int MRStop = 100;
int MStop = -100;
double counterRCM;
double counterLCM;
int L_PWMC = 0;
int M_PWMC = 0;
int R_PWMC = 0;

int R_PWM_P = 160;
int R_PWM_N = -160;
int M_PWM_P = 0 ;
int M_PWM_N = -160;
int M_PWMdeg = 0;



void setup() {

  // Set encoder pins as inputs
  pinMode(L_CLK, INPUT);
  pinMode(L_DT, INPUT);
  pinMode(R_CLK, INPUT);
  pinMode(R_DT, INPUT);

  pinMode(LSens, INPUT);
  pinMode(MSens, INPUT);
  pinMode(RSens, INPUT);

  pinMode(Sws, INPUT);
  pinMode(QtrSens, INPUT);
  pinMode(VoltageSens, INPUT);
  pinMode(L_Mdir, OUTPUT);
  pinMode(L_Mpwm, OUTPUT);
  pinMode(R_Mdir, OUTPUT);
  pinMode(R_Mpwm, OUTPUT);
  pinMode(LeftLed, OUTPUT);
  pinMode(RightLed, OUTPUT);
  Serial.begin(115200);
  lastStateL_CLK = digitalRead(L_CLK);
  lastStateR_CLK = digitalRead(R_CLK);
  /*
  while (analogRead(VoltageSens) < 800) {
    FlashLed();
  }
*/
  FlashLed();
}

void loop() {

  if (analogRead(Sws) < 800) {
    WaitLed();
  }
  while (Start == 1) {

    if (Left90L == 1 && Left90R == 1) {
      MSensFlash();
      MotorStop();
      delay(TurnWait);//Motor Stop TurnWait Val ms..
      while (Left90L == 1 && Left90R == 1) {
        Left90Turn();
      }
      EncoderReset();
      MotorStop();
      delay(100);//Motor Stop TurnWait Val ms..
    } else {

      SensorDeg();
      digitalWrite(7, LOW);
      analogWrite(9, MLStop + (R_PWMC) - (M_PWMdeg));
      digitalWrite(8, HIGH);
      analogWrite(10, MRStop - (M_PWMdeg));

      if (NoWall == 1) {


        RightTurnVal =  RightTurnVal + 1;
        MotorStop();
        delay(TurnWait);//Motor Stop TurnWait Val ms..
        while (ForwL == 0 && ForwR == 0) {
          ForwOneStep();
        }
        EncoderReset();
        MotorStop();
        delay(100);
        while (Right90L == 0 && Right90R == 0) {
          Right90Turn();
        }
        EncoderReset();
        MotorStop();
        delay(100);//Motor Stop TurnWait Val ms..
        while (ForwMiniL == 0 && ForwMiniR == 0) {
          ForwMiniStep();
        }
        EncoderReset();
        MotorStop();
        delay(100);
        while (RightTurnVal == FinishRightTurn) {
          AutoStop();
          while (Finish == 1) {
            MotorStop();
            FinishLed();
          }
          // Serial.print(" | Counter: ");
          // Serial.println(RightTurnVal);
          // FinishLed();
        }
        SensorDeg();

      }

    }


  }
}
