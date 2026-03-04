#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// --- DONANIM AYARLARI ---
const int PIN_YAGMUR = A0;
const int PIN_SERVO  = 9;
const int PIN_BUZZER = 8;     // Buzzer'ın (+) ucu buraya

const int ESIK_DEGER = 600;

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo pencereServosu;

int yagmurDegeri = 0;

void setup() {
  Serial.begin(9600);
  
  lcd.init();
  lcd.backlight();
  
  pencereServosu.attach(PIN_SERVO);
  pencereServosu.write(0); // Başlangıç: Açık
  
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW); // Başlangıçta sustur

  lcd.setCursor(0, 0);
  lcd.print("Sistem Hazir");
  delay(1000);
  lcd.clear();
}

void loop() {
  yagmurDegeri = analogRead(PIN_YAGMUR);

  // Serial Monitor İzleme
  Serial.print("Deger: ");
  Serial.print(yagmurDegeri);

  // EKRAN SABİT KISIMLAR
  lcd.setCursor(0, 0);
  lcd.print("Yagmur: ");
  lcd.print(yagmurDegeri);
  lcd.print("   ");

  lcd.setCursor(0, 1);

  if (yagmurDegeri < ESIK_DEGER) {
    // --- YAĞMUR VAR (ALARM MODU) ---
    Serial.println(" -> ALARM! YAGMUR VAR");
    lcd.print("ALARM CALIYOR!  ");

    // 1. Pencereyi Kapat
    pencereServosu.write(180);

    // 2. Kesik Kesik Alarm (Bip - Bip)
    digitalWrite(PIN_BUZZER, HIGH); // Ses Ver
    delay(200);                     // 0.2 saniye bekle
    digitalWrite(PIN_BUZZER, LOW);  // Sesi Kes
    delay(200);                     // 0.2 saniye bekle
    
    // Not: Loop döndüğü sürece bu "Aç-Kapa" işlemi tekrar eder.
    
  } else {
    // --- YAĞMUR YOK (NORMAL MOD) ---
    Serial.println(" -> Hava Normal");
    lcd.print("DURUM: GUVENLI  ");

    // 1. Pencereyi Aç
    pencereServosu.write(0);
    
    // 2. Buzzer Emin Olmak İçin Sustur
    digitalWrite(PIN_BUZZER, LOW);
    
    delay(500); // Normal modda biraz daha yavaş akabilir
  }
}