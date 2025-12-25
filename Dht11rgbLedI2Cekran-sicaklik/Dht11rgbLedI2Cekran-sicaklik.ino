#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ==== DHT Ayarları ====
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ==== RGB LED Pinleri ====
#define RED_PIN   9
#define GREEN_PIN 10
#define BLUE_PIN  11

// ==== LCD ====
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  dht.begin();

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("DHT11 Basladi");
  delay(2000);
  lcd.clear();
}

void loop() {
  float nem = dht.readHumidity();
  float sicaklik = dht.readTemperature(); // Celsius

  if (isnan(nem) || isnan(sicaklik)) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor Hatasi!");
    return;
  }

  // ==== LCD Yazdir ====
  lcd.setCursor(0, 0);
  lcd.print("Sicaklik: ");
  lcd.print(sicaklik);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Nem: ");
  lcd.print(nem);
  lcd.print(" %");

  // ==== RGB LED Kontrol ====
  if (sicaklik < 20) {
    setColor(0, 0, 255);   // Mavi
  }
  else if (sicaklik >= 20 && sicaklik <= 30) {
    setColor(0, 255, 0);   // Yesil
  }
  else {
    setColor(255, 0, 0);   // Kirmizi
  }

  delay(2000);
}

// ==== RGB Renk Fonksiyonu ====
void setColor(int r, int g, int b) {
  analogWrite(RED_PIN, r);
  analogWrite(GREEN_PIN, g);
  analogWrite(BLUE_PIN, b);
}
