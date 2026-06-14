#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// I2C Ekran Tanımlaması (Adres 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin Tanımlamaları
#define IR_PIN 4
#define ECHO_PIN 5
#define TRIG_PIN 6
#define BUZZER_PIN 8

// --- AYARLANABİLİR DEĞİŞKENLER ---
float sensorlerArasiMesafe = 20.0; // Santimetre cinsinden iki sensör arası
float hizSiniri = 40.0;            // Hız sınırı (cm/s). Bunu arabanın hızına göre değiştirin!

// Zamanlama Değişkenleri
unsigned long baslangicZamani = 0;
unsigned long bitisZamani = 0;
bool olcumBasladi = false;

void setup() {
  Serial.begin(9600);
  
  pinMode(IR_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT); 

  lcd.init();
  lcd.backlight();
  
  // Sistem başladığında Seri Monitör'e bilgi ver
  Serial.println("=========================================");
  Serial.println("[SİSTEM] Hiz Radari Baslatiliyor...");
  Serial.print("[SİSTEM] Hiz Siniri: ");
  Serial.print(hizSiniri);
  Serial.println(" cm/s");
  Serial.print("[SİSTEM] Sensorler Arasi Mesafe: ");
  Serial.print(sensorlerArasiMesafe);
  Serial.println(" cm");
  Serial.println("=========================================");
  
  beklemeEkraniniYazdir();
}

void loop() {
  // --- 1. AŞAMA: Başlangıç Kapısı (IR Sensör) ---
  int irDurum = digitalRead(IR_PIN);
  
  if (irDurum == LOW && !olcumBasladi) {
    baslangicZamani = millis(); 
    olcumBasladi = true;
    
    Serial.println("[SENSÖR 1] Arac IR sensorden gecti. Kronometre BASLADI!");
    tone(BUZZER_PIN, 2000, 50); // Kronometre başlama sesi
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Olcum Basladi!");
    lcd.setCursor(0, 1);
    lcd.print("Gecis Bekleniyor");
    
    delay(300); 
  }

  // --- 2. AŞAMA: Bitiş Kapısı (Ultrasonik Sensör) ---
  if (olcumBasladi) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
    int mesafe = duration * 0.034 / 2;

    if (mesafe > 0 && mesafe < 15) {
      bitisZamani = millis(); 
      Serial.println("[SENSÖR 2] Arac Ultrasonik sensorden gecti. Kronometre DURDU.");
      
      // HESAPLAMALAR
      unsigned long gecenSureMillis = bitisZamani - baslangicZamani;
      float gecenSureSaniye = gecenSureMillis / 1000.0; 
      float hiz_cm_s = sensorlerArasiMesafe / gecenSureSaniye;

      // SERİ MONİTÖRE SONUÇLARI YAZDIR
      Serial.println("-----------------------------------------");
      Serial.print("[SONUÇ] Gecen Sure: ");
      Serial.print(gecenSureSaniye);
      Serial.println(" saniye");
      
      Serial.print("[SONUÇ] Hesaplananan Hiz: ");
      Serial.print(hiz_cm_s);
      Serial.println(" cm/s");

      // EKRANA YAZDIRMA
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Hiz: ");
      lcd.print(hiz_cm_s);
      lcd.print(" cm/s");

      // --- 3. AŞAMA: Hız Sınırı ve Buzzer Kontrolü ---
      if (hiz_cm_s > hizSiniri) {
        Serial.println("[RADAR] UYARI: Hiz siniri asildi! CEZA KESILDI.");
        lcd.setCursor(0, 1);
        lcd.print("RADARA GIRDIN!");
        polisSireniCal();
      } else {
        Serial.println("[RADAR] Gecis temiz. Hiz siniri ihlali yok.");
        lcd.setCursor(0, 1);
        lcd.print("Gecis Basarili");
        onaySesiCal();
      }
      Serial.println("-----------------------------------------");

      delay(4000); // Sonucu 4 saniye ekranda tut
      
      // Sistemi Sıfırla
      olcumBasladi = false;
      Serial.println("[SİSTEM] Radar sifirlandi. Yeni arac bekleniyor...");
      beklemeEkraniniYazdir();
    }
  }
}

// --- YARDIMCI FONKSİYONLAR ---

void beklemeEkraniniYazdir() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hiz Radari Hazir");
  lcd.setCursor(0, 1);
  lcd.print("Sinir: ");
  lcd.print(hizSiniri);
  lcd.print(" cm/s");
}

void onaySesiCal() {
  tone(BUZZER_PIN, 1000, 100); 
  delay(150);
  tone(BUZZER_PIN, 1500, 200); 
}

void polisSireniCal() {
  for (int i = 0; i < 3; i++) {
    tone(BUZZER_PIN, 600); 
    delay(300);
    tone(BUZZER_PIN, 1200); 
    delay(300);
  }
  noTone(BUZZER_PIN); 
}