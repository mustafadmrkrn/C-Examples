/*
  Ertuğrul & Öğretmen - Telefon Kontrollü RGB Lamba (Gelişmiş İzleme)
  Bileşenler: HM-10/BT04-A Bluetooth (BLE) Modülü, RGB LED
*/

#include <SoftwareSerial.h>

// Bluetooth modülü: RX=2, TX=3 (Sanal Seri Port)
SoftwareSerial btModul(2, 3);

// RGB LED Pinleri (PWM özellikli pinler: 9, 10, 11)
#define KIRMIZI_PIN 9
#define YESIL_PIN 10
#define MAVI_PIN 11

// ==========================================
// LED TİPİ AYARI ŞALTERİ
// ==========================================
// Eğer LED'iniz Ortak Artı (Anode) ise burası 'true' kalsın. (Uzun bacak 5V'a takılır)
// Eğer Ortak Eksi (Cathode) ise burayı 'false' yapın. (Uzun bacak GND'ye takılır)
bool ortakArtiLed = true; 

void setup() {
  // 1. Bilgisayar Ekranı (Seri Port) Başlatılıyor
  Serial.begin(9600);

  // 2. Bluetooth Modülü Başlatılıyor
  btModul.begin(9600);

  pinMode(KIRMIZI_PIN, OUTPUT);
  pinMode(YESIL_PIN, OUTPUT);
  pinMode(MAVI_PIN, OUTPUT);

  // Başlangıçta ışık kapalı olsun
  isigiKapat();

  Serial.println("=====================================");
  Serial.println("SISTEM HAZIR!");
  Serial.println("Bluetooth uygulamasindan baglanabilirsiniz...");
  Serial.println("Gecerli Komutlar: R, G, B, Y, M, W, 0");
  Serial.println("=====================================");
}

void loop() {
  // Bluetooth'tan telefondan bir mesaj (harf) geldi mi?
  if (btModul.available()) {
    
    // Gelen harfi oku
    char gelenVeri = btModul.read();

    // SERİ PORT'A YAZDIR (Hata Ayıklama / Röntgen)
    Serial.print(">>> Telefondan Gelen Komut: ");
    Serial.println(gelenVeri);

    // Gelen harfe göre renk değiştir
    if (gelenVeri == 'R' || gelenVeri == 'r') {
      Serial.println("    Islem: KIRMIZI uygulaniyor.");
      renkAyarla(255, 0, 0);
    } 
    else if (gelenVeri == 'G' || gelenVeri == 'g') {
      Serial.println("    Islem: YESIL uygulaniyor.");
      renkAyarla(0, 255, 0);
    } 
    else if (gelenVeri == 'B' || gelenVeri == 'b') {
      Serial.println("    Islem: MAVI uygulaniyor.");
      renkAyarla(0, 0, 255);
    } 
    else if (gelenVeri == 'Y' || gelenVeri == 'y') {
      Serial.println("    Islem: SARI uygulaniyor.");
      renkAyarla(255, 255, 0); // Kırmızı + Yeşil
    } 
    else if (gelenVeri == 'M' || gelenVeri == 'm') {
      Serial.println("    Islem: MOR uygulaniyor.");
      renkAyarla(255, 0, 255); // Kırmızı + Mavi
    } 
    else if (gelenVeri == 'W' || gelenVeri == 'w') {
      Serial.println("    Islem: BEYAZ uygulaniyor.");
      renkAyarla(255, 255, 255); // Tümü açık
    } 
    else if (gelenVeri == '0') {
      Serial.println("    Islem: Isiklar KAPATILDI.");
      isigiKapat();
    }
  }
}

// ==========================================
// AKILLI RENK AYARLAMA FONKSİYONU
// ==========================================
void renkAyarla(int kirmizi, int yesil, int mavi) {
  
  // Eğer LED Ortak Artı ise, sayıları tersine çevirerek gönder
  if (ortakArtiLed == true) {
    analogWrite(KIRMIZI_PIN, 255 - kirmizi);
    analogWrite(YESIL_PIN, 255 - yesil);
    analogWrite(MAVI_PIN, 255 - mavi);
  } 
  // Eğer LED Ortak Eksi ise, sayıları normal gönder
  else {
    analogWrite(KIRMIZI_PIN, kirmizi);
    analogWrite(YESIL_PIN, yesil);
    analogWrite(MAVI_PIN, mavi);
  }
}

void isigiKapat() {
  renkAyarla(0, 0, 0);
}