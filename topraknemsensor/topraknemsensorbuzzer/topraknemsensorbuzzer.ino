#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------- PINLER --------
#define nemPin A0

#define redPin 9
#define greenPin 10
#define bluePin 11

#define buzzerPin 6

// -------- NEM SEVIYELERI --------
#define KURU_SEVIYE 700
#define IDEAL_SEVIYE 400

bool buzzerCaldi = false;

// ================== SETUP ==================
void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Bitki Sulama");
  lcd.setCursor(0, 1);
  lcd.print("Kontrol Sist.");
  delay(2000);
  lcd.clear();
}

// ================== LOOP ==================
void loop() {
  int nem = analogRead(nemPin);

  lcd.setCursor(0, 0);
  lcd.print("Nem Degeri:");
  lcd.setCursor(11, 0);
  lcd.print(nem);
  lcd.print("   ");

  if (nem > KURU_SEVIYE) {
    // -------- KURU --------
    setColor(255, 0, 0);
    lcd.setCursor(0, 1);
    lcd.print("Toprak KURU   ");

    if (!buzzerCaldi) {
      ditDit30sn();      // 🔊 30 sn gür ses
      buzzerCaldi = true;
    }
  }
  else if (nem > IDEAL_SEVIYE) {
    // -------- IDEAL --------
    setColor(0, 255, 0);
    lcd.setCursor(0, 1);
    lcd.print("Nem IDEAL     ");
    buzzerCaldi = false;
  }
  else {
    // -------- ISLAK --------
    setColor(0, 0, 255);
    lcd.setCursor(0, 1);
    lcd.print("Toprak ISLAK  ");
    buzzerCaldi = false;
  }

  delay(1000);
}

// ================== FONKSIYONLAR ==================
void setColor(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

void ditDit30sn() {
  unsigned long baslangic = millis();

  while (millis() - baslangic < 30000) {   // ⏱ 30 saniye
    digitalWrite(buzzerPin, HIGH);         // DIT
    delay(300);
    digitalWrite(buzzerPin, LOW);
    delay(150);
    digitalWrite(buzzerPin, HIGH);         // DIT
    delay(300);
    digitalWrite(buzzerPin, LOW);
    delay(600);                            // dit-dit arası
  }
}
