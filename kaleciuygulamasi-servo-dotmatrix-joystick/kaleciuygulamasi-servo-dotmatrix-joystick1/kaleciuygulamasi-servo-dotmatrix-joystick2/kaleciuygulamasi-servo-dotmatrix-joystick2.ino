#include <Servo.h>
#include <LedControl.h>

// --- PIN TANIMLAMALARI ---
const int PIN_JOY_X = A0;  // Joystick sağ-sol
const int PIN_SERVO = 9;   // Kaleci servosu

const int PIN_DIN = 11;
const int PIN_CS  = 10;
const int PIN_CLK = 13;

// --- NESNELER ---
Servo kaleciServo;
LedControl lc = LedControl(PIN_DIN, PIN_CLK, PIN_CS, 1);

// --- EKRAN GRAFİKLERİ (Byte Dizileri) ---
// 1. Ortada Beklerken (Gülümseyen Yüz)
byte yuzOrtada[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

// 2. Sola Atlarken (Sola Ok)
byte okSol[8] = {
  B00011000,
  B00111000,
  B01111111,
  B11111111,
  B11111111,
  B01111111,
  B00111000,
  B00011000
};

// 3. Sağa Atlarken (Sağa Ok)
byte okSag[8] = {
  B00011000,
  B00011100,
  B11111110,
  B11111111,
  B11111111,
  B11111110,
  B00011100,
  B00011000
};

void setup() {
  Serial.begin(9600);
  
  // Servo Başlat
  kaleciServo.attach(PIN_SERVO);
  
  // Ekranı Başlat
  lc.shutdown(0, false);
  lc.setIntensity(0, 5); // Parlaklık (0-15)
  lc.clearDisplay(0);
  
  Serial.println("--- FIZIKSEL KALECI SISTEMI HAZIR ---");
}

void loop() {
  // 1. Joystick'ten X eksenini oku (0 ile 1023 arası değer verir, orta noktası ~512'dir)
  int joyDeger = analogRead(PIN_JOY_X);
  
  // 2. Joystick değerini Servo açısına (0-180) çevir
  // map(okunan_deger, okunan_min, okunan_max, hedef_min, hedef_max)
  int servoAci = map(joyDeger, 0, 1023, 0, 180); 
  
  // 3. Servoyu hareket ettir
  kaleciServo.write(servoAci);
  
  // 4. Ekrana Servo Açısına Göre Grafik Yansıt
  // Kaleci sağa, sola veya ortada mı?
  if (servoAci < 60) {
    // Joystick bir yöne çekildi (Örn: Sola)
    sekilCiz(okSol);
    Serial.println("Kaleci SOLA atladi!");
  } 
  else if (servoAci > 120) {
    // Joystick diğer yöne çekildi (Örn: Sağa)
    sekilCiz(okSag);
    Serial.println("Kaleci SAGA atladi!");
  } 
  else {
    // Joystick ortada (60 ile 120 derece arası)
    sekilCiz(yuzOrtada);
  }
  
  delay(15); // Servonun pozisyon alması için minik bir bekleme
}

// --- EKRANA ÇİZİM YAPMA FONKSİYONU ---
void sekilCiz(byte sekil[]) {
  for(int i = 0; i < 8; i++) {
    lc.setRow(0, i, sekil[i]);
  }
}