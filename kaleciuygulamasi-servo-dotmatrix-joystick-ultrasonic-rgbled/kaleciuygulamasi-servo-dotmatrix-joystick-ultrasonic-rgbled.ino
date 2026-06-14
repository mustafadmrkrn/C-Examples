#include <Servo.h>
#include <LedControl.h>

// --- PIN TANIMLAMALARI ---
const int PIN_JOY_X = A0;  // Joystick X Ekseni
const int PIN_SERVO = 9;   // Kaleci Servosu

// Dot Matrix Pinleri
const int PIN_DIN = 11;
const int PIN_CS  = 10;
const int PIN_CLK = 13;

// Ultrasonik Sensör Pinleri
const int PIN_TRIG = 6;
const int PIN_ECHO = 7;

// RGB LED Pinleri
const int PIN_KIRMIZI = 3;
const int PIN_YESIL   = 5;
const int PIN_MAVI    = 4;

// --- AYARLAR ---
// Sensörün topu algılayacağı maksimum mesafe (Santimetre)
// Kendi kalene göre bu sayıyı Serial Monitor'e bakarak değiştirmelisin!
const int GOL_MESAFESI = 10; 

// --- NESNELER ---
Servo kaleciServo;
LedControl lc = LedControl(PIN_DIN, PIN_CLK, PIN_CS, 1);

// --- DOT MATRIX GRAFİKLERİ ---
byte yuzOrtada[8] = { B00111100, B01000010, B10100101, B10000001, B10100101, B10011001, B01000010, B00111100 };
byte okSol[8]     = { B00011000, B00111000, B01111111, B11111111, B11111111, B01111111, B00111000, B00011000 };
byte okSag[8]     = { B00011000, B00011100, B11111110, B11111111, B11111111, B11111110, B00011100, B00011000 };
byte carpiGol[8]  = { B10000001, B01000010, B00100100, B00011000, B00011000, B00100100, B01000010, B10000001 }; // Gol olduğunda çıkacak X

void setup() {
  Serial.begin(9600);
  
  // Pin Modları
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  pinMode(PIN_KIRMIZI, OUTPUT);
  pinMode(PIN_YESIL, OUTPUT);
  pinMode(PIN_MAVI, OUTPUT);

  // Servo Başlat
  kaleciServo.attach(PIN_SERVO);
  
  // Ekran Başlat
  lc.shutdown(0, false);
  lc.setIntensity(0, 5); 
  lc.clearDisplay(0);
  
  Serial.println("=============================================");
  Serial.println("   PENALTI KALESI AKILLI SISTEMI BASLADI     ");
  Serial.println("=============================================");
  
  rgbRenk(0, 255, 0); // Oyun başlarken Yeşil yan
}

void loop() {
  // ==========================================
  // 1. ULTRASONİK SENSÖR İLE GOL KONTROLÜ
  // ==========================================
  long sure, mesafe;
  digitalWrite(PIN_TRIG, LOW); delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  // pulseIn fonksiyonuna 30000 (30ms) zaman aşımı ekledik ki oyun kasmasın
  sure = pulseIn(PIN_ECHO, HIGH, 30000); 
  mesafe = sure * 0.034 / 2; // Süreyi santimetreye çevir

  // Serial Monitor: Sensörün ne gördüğünü yazdır (Hata ayıklamak için)
  Serial.print("Sensor Mesafesi: ");
  Serial.print(mesafe);
  Serial.print(" cm | Durum: ");

  // Eğer mesafe tanımladığımız eşikten küçükse (VE 0 değilse) GOL OLMUŞTUR!
  if (mesafe > 0 && mesafe < GOL_MESAFESI) {
    Serial.println("!!! GOL OLDU !!! TOP CIZGIYI GECTI!");
    
    // 1. RGB Kırmızı Yanar
    rgbRenk(255, 0, 0); 
    
    // 2. Ekranda X İşareti Çıkar
    sekilCiz(carpiGol);
    
    // 3. Kaleci Üzüntüden Yere Yatar (0 Derece)
    kaleciServo.write(0);
    
    // Oyunu 3 saniye durdur (Gol kutlaması / Üzüntüsü)
    delay(3000); 
    
    // Oyun Yeniden Başlıyor
    Serial.println("--- OYUN YENIDEN BASLIYOR ---");
    rgbRenk(0, 255, 0); // Tekrar Yeşil yan
    lc.clearDisplay(0);
    
  } else {
    // ==========================================
    // 2. GOL YOKSA OYUN DEVAM EDİYOR (NORMAL MOD)
    // ==========================================
    Serial.println("Oyun Devam Ediyor...");
    rgbRenk(0, 255, 0); // Yeşil yanmaya devam et
    
    // Joystick Oku ve Kaleciyi Hareket Ettir
    int joyDeger = analogRead(PIN_JOY_X);
    int servoAci = map(joyDeger, 0, 1023, 0, 180); 
    kaleciServo.write(servoAci);
    
    // Ekrana Yönleri Çiz
    if (servoAci < 60) {
      sekilCiz(okSol);
    } 
    else if (servoAci > 120) {
      sekilCiz(okSag);
    } 
    else {
      sekilCiz(yuzOrtada);
    }
  }
  
  delay(20); // Sistem rahatlasın diye minik bir bekleme
}

// --- YARDIMCI FONKSİYONLAR ---

// Dot Matrix Çizim Fonksiyonu
void sekilCiz(byte sekil[]) {
  for(int i = 0; i < 8; i++) {
    lc.setRow(0, i, sekil[i]);
  }
}

// RGB LED Renk Ayarlama Fonksiyonu
void rgbRenk(int k, int y, int m) {
  analogWrite(PIN_KIRMIZI, k);
  analogWrite(PIN_YESIL, y);
  analogWrite(PIN_MAVI, m);
}