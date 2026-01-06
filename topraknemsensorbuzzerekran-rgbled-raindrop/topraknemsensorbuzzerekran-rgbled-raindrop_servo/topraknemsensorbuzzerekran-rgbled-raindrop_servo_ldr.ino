#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// -------- PINLER --------
#define nemPin A0
#define raindropPin A1
#define ldrPin 4        // 3 bacaklı LDR (DİJİTAL)
#define dhtPin 2

#define redPin 9
#define greenPin 10
#define bluePin 11

#define buzzerPin 6
#define growLedPin 7

#define servoPin1 3
#define servoPin2 5

// -------- SERVO --------
Servo servo1;
Servo servo2;

// -------- DHT11 --------
#define DHTTYPE DHT11
DHT dht(dhtPin, DHTTYPE);

// -------- ESİKLER --------
#define KURU_SEVIYE 700
#define IDEAL_SEVIYE 400
#define YAGMUR_ESIK 500

bool buzzerCaldi = false;
unsigned long sonGuncelleme = 0;
byte ekranModu = 0;

// ================== SETUP ==================
void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  dht.begin();

  pinMode(ldrPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(growLedPin, OUTPUT);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);

  servo1.write(90);
  servo2.write(90);

  lcd.setCursor(0, 0);
  lcd.print("AKILLI SERA");
  lcd.setCursor(0, 1);
  lcd.print("Sistem Basliyor");
  delay(2000);
  lcd.clear();
}

// ================== LOOP ==================
void loop() {
  int toprakNem = analogRead(nemPin);
  int yagmurDeger = analogRead(raindropPin);
  bool karanlik = digitalRead(ldrPin) == LOW;

  float sicaklik = dht.readTemperature();
  float havaNem = dht.readHumidity();

  // -------- LDR KONTROL --------
  if (karanlik) {
    digitalWrite(growLedPin, HIGH);
    setColor(255, 150, 0);   // Sarı
  } else {
    digitalWrite(growLedPin, LOW);
  }

  // -------- SERVO (YAĞMUR) --------
  if (yagmurDeger < YAGMUR_ESIK) {
    servo1.write(180); // +90
    servo2.write(0);   // -90
  } else {
    servo1.write(90);
    servo2.write(90);
  }

  // -------- EKRAN MODU --------
  if (millis() - sonGuncelleme > 3000) {
    ekranModu++;
    if (ekranModu > 3) ekranModu = 0;
    sonGuncelleme = millis();
    lcd.clear();
  }

  // -------- LCD --------
  if (ekranModu == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Toprak Nem:");
    lcd.print(toprakNem);

    lcd.setCursor(0, 1);
    if (toprakNem > KURU_SEVIYE) {
      lcd.print("KURU - SULA!");
      setColor(255, 0, 0);
      if (!buzzerCaldi) {
        ditDit30sn();
        buzzerCaldi = true;
      }
    } else if (toprakNem > IDEAL_SEVIYE) {
      lcd.print("IDEAL       ");
      setColor(0, 255, 0);
      buzzerCaldi = false;
    } else {
      lcd.print("ISLAK       ");
      setColor(0, 0, 255);
      buzzerCaldi = false;
    }
  }

  else if (ekranModu == 1) {
    lcd.setCursor(0, 0);
    lcd.print("Sic:");
    lcd.print(sicaklik, 1);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Nem:%");
    lcd.print(havaNem, 0);
  }

  else if (ekranModu == 2) {
    lcd.setCursor(0, 0);
    lcd.print("Yagmur:");
    lcd.print(yagmurDeger);

    lcd.setCursor(0, 1);
    lcd.print(yagmurDeger < YAGMUR_ESIK ? "YAGMUR VAR " : "HAVA ACIK ");
  }

  else if (ekranModu == 3) {
    lcd.setCursor(0, 0);
    lcd.print("Isik:");
    lcd.print(karanlik ? "KARANLIK" : "AYDINLIK");

    lcd.setCursor(0, 1);
    lcd.print("Grow LED:");
    lcd.print(digitalRead(growLedPin) ? "ACIK " : "KAPALI");
  }

  // -------- SERIAL MONITOR --------
  Serial.println("===== AKILLI SERA =====");
  Serial.print("Toprak Nem: "); Serial.println(toprakNem);
  Serial.print("Yagmur: "); Serial.println(yagmurDeger < YAGMUR_ESIK ? "VAR" : "YOK");
  Serial.print("Isik: "); Serial.println(karanlik ? "KARANLIK" : "AYDINLIK");
  Serial.print("Sicaklik: "); Serial.print(sicaklik); Serial.println(" C");
  Serial.print("Hava Nem: "); Serial.print(havaNem); Serial.println(" %");
  Serial.print("Servo1: "); Serial.println(servo1.read());
  Serial.print("Servo2: "); Serial.println(servo2.read());
  Serial.print("Grow LED: "); Serial.println(digitalRead(growLedPin) ? "ACIK" : "KAPALI");
  Serial.println("=======================\n");

  delay(500);
}

// ================== FONKSIYONLAR ==================
void setColor(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}

void ditDit30sn() {
  unsigned long baslangic = millis();
  while (millis() - baslangic < 30000) {
    digitalWrite(buzzerPin, HIGH);
    delay(300);
    digitalWrite(buzzerPin, LOW);
    delay(150);
    digitalWrite(buzzerPin, HIGH);
    delay(300);
    digitalWrite(buzzerPin, LOW);
    delay(600);
  }
}
