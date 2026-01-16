#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int yagmurSensor = A0;
int yagmurDeger = 0;

// Kuru / Islak ayrımı için eşik
int ESIK_DEGER = 600;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Yagmur Test");
  delay(1500);
  lcd.clear();

  Serial.println("=== Yagmur Sensor Testi ===");
}

void loop() {
  yagmurDeger = analogRead(yagmurSensor);

  // SERIAL MONITOR
  Serial.print("Yagmur Degeri: ");
  Serial.println(yagmurDeger);

  // LCD
  lcd.setCursor(0, 0);
  lcd.print("Deger: ");
  lcd.print(yagmurDeger);
  lcd.print("   ");

  lcd.setCursor(0, 1);

  // DURUM
  if (yagmurDeger < ESIK_DEGER) {
    lcd.print("YAGMUR VAR   ");
  } else {
    lcd.print("YAGMUR YOK   ");
  }

  delay(500);
}
