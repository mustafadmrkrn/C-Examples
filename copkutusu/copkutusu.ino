#include <Servo.h>

// --- PIN TANIMLAMALARI ---
// Ultrasonik Sensör
const int trigPin = 9;  // Tetikleyici (Trigger) pini
const int echoPin = 10; // Yankı (Echo) pini

// Servo Motor
const int servoPin = 6;
Servo copKutusuServo;

// RGB LED (Sadece Kırmızı ve Yeşil kullanılacak)
const int redPin = 11;
const int greenPin = 12;

// --- AYARLAR ---
// Kapak açılması için maksimum mesafe eşiği (cm)
const int mesafeEsigi = 25; 
// Kapak açık kalma süresi (milisaniye)
const long beklemeSuresi = 3000; 

// Servo pozisyonları
const int KAPALI_ACI = 0;   // Kapalı pozisyon (0 derece)
const int ACIK_ACI = 90;    // Açık pozisyon (90 derece, motorunuzun mekaniğine göre ayarlayın)

// Durum değişkeni
bool kapakAcik = false;
unsigned long sonAcilmaZamani = 0;

// ----------------------------------------------------
// Fonksiyon: Mesafe Ölçümü
// ----------------------------------------------------
long olcumYap() {
  // Trig pini LOW yapılır (temizlik)
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Trig pini HIGH yapılır (10us darbe)
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Echo pininden gelen süreyi okur
  long sure = pulseIn(echoPin, HIGH);

  // Süreyi santimetreye çevirir (Ses hızı: 343 m/s = 0.0343 cm/µs)
  // Mesafe (cm) = (Süre / 2) * 0.0343. Basitleştirilmiş formül: Süre / 58
  long mesafe = sure / 58;

  return mesafe;
}

// ----------------------------------------------------
// Fonksiyon: LED Kontrolü
// ----------------------------------------------------
void ledDurumuAyarla(bool acikMi) {
  if (acikMi) {
    // Kapak Açıkken: Kırmızı Yanar (Uyarı)
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
  } else {
    // Kapak Kapalıyken: Yeşil Yanar (Hazır)
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
  }
}

// ----------------------------------------------------
// SETUP
// ----------------------------------------------------
void setup() {
  // Seri İletişimi Başlatma (Hata ayıklama için)
  Serial.begin(9600); 

  // Ultrasonik Pinleri
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // LED Pinleri
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);

  // Servo Motoru
  copKutusuServo.attach(servoPin);
  copKutusuServo.write(KAPALI_ACI); // Başlangıçta kapalı
  
  // Başlangıç LED durumu: Kapalı (Yeşil)
  ledDurumuAyarla(false);
  delay(500);
}

// ----------------------------------------------------
// LOOP
// ----------------------------------------------------
void loop() {
  long mesafe = olcumYap();
  Serial.print("Mesafe: ");
  Serial.print(mesafe);
  Serial.println(" cm");

  // 1. Durum: Yakınlık Algılandı ve Kapak Kapalı
  if (mesafe <= mesafeEsigi && !kapakAcik) {
    copKutusuServo.write(ACIK_ACI); // Kapağı Aç
    ledDurumuAyarla(true);          // Kırmızı Yak (Açık)
    kapakAcik = true;
    sonAcilmaZamani = millis();     // Açılış zamanını kaydet
    Serial.println("Kapak Açıldı - Kırmızı LED");
    delay(500); // Gürültüyü engellemek için kısa bekleme

  // 2. Durum: Kapak Açık ve Kapanma Süresi Doldu
  } else if (kapakAcik && (millis() - sonAcilmaZamani >= beklemeSuresi)) {
    copKutusuServo.write(KAPALI_ACI); // Kapağı Kapat
    ledDurumuAyarla(false);           // Yeşil Yak (Kapalı)
    kapakAcik = false;
    Serial.println("Kapak Kapandı - Yeşil LED");
    delay(500); // Gürültüyü engellemek için kısa bekleme
  }

  // Sensör okumaları arasında kısa bekleme
  delay(100); 
}