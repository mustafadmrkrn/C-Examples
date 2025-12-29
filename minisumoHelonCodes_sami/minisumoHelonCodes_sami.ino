#define solpwm1 3
#define solpwm2 11
#define sagpwm1 9
#define sagpwm2 10

#define solqtr A0
#define sagqtr A3

#define solsensor 12
#define onsensor 8
#define sagsensor A5

#define MaxSpeed 255
#define ScanSpeed 180 //Tarama yaparken dönüş hızı

#define qtrEsik 500
#define DEBUG false

// Algılama durumları
bool sol_algiladi = false;
bool on_algiladi = false;
bool sag_algiladi = false;
enum Sides { SOL, ON, SAG };
Sides LastFound;

void setup() {
  // Motor pinlerini çıkış olarak ayarla
  Serial.begin(9600);
  pinMode(solpwm1, OUTPUT);
  pinMode(solpwm2, OUTPUT);
  pinMode(sagpwm1, OUTPUT);
  pinMode(sagpwm2, OUTPUT);
  pinMode(solqtr, INPUT_PULLUP);
  pinMode(sagqtr, INPUT_PULLUP);
  pinMode(solsensor, INPUT);
  pinMode(onsensor, INPUT);
  pinMode(sagsensor, INPUT);
  while (DEBUG) { // Beyaz varken az değer, siyah 850 üstü
    Serial.println("Sol QTR: "+String(analogRead(solqtr)));
    Serial.println("Sağ QTR: "+String(analogRead(sagqtr)));
    Serial.println("------------------");
    delay(450);
  }
}
bool arama_dongusunden = false; //tam terse dön
void loop() {
  // Sensör okumalarını yap - anlık durumu al
  // Algılanırsa False olur
  sol_algiladi = digitalRead(solsensor) == LOW;
  on_algiladi = digitalRead(onsensor) == LOW;
  sag_algiladi = digitalRead(sagsensor) == LOW;
  
  // Anlık sensör durumuna göre hemen tepki ver
  if (on_algiladi) {
    Set_Motor(MaxSpeed, MaxSpeed);
    if (arama_dongusunden == false) {
      LastFound = ON;
    }
    else arama_dongusunden = false;
  }
  else if (sol_algiladi) {
    LastFound = SOL;
    while (!on_algiladi) {
      arama_dongusunden = true;
      Set_Motor(-MaxSpeed, MaxSpeed);
      on_algiladi = digitalRead(onsensor) == LOW;
    }
  }
  else if (sag_algiladi) {
    LastFound = SAG;
    while (!on_algiladi) {
      arama_dongusunden = true;
      Set_Motor(MaxSpeed, -MaxSpeed);
      on_algiladi = digitalRead(onsensor) == LOW;
    }
  }
  else { //Hiçbir şey algılanmıyor - tarama moduna geç
    if (LastFound == SOL) Set_Motor(MaxSpeed, -MaxSpeed);
    else if (LastFound == SAG) Set_Motor(-MaxSpeed, MaxSpeed);
    //else if (LastFound == ON)
    else;
    Set_Motor(MaxSpeed, -MaxSpeed);
  }
}

void Set_Motor(int solHiz, int sagHiz) {
  // Sol motor kontrolü
  if (solHiz >= 0) {
    analogWrite(solpwm1, solHiz);
    analogWrite(solpwm2, 0);
  } else {
    analogWrite(solpwm1, 0);
    analogWrite(solpwm2, -solHiz);
  }
  // Sağ motor kontrolü
  if (sagHiz >= 0) {
    analogWrite(sagpwm1, sagHiz);
    analogWrite(sagpwm2, 0);
  } else {
    analogWrite(sagpwm1, 0);
    analogWrite(sagpwm2, -sagHiz);
  }
}