/*
  Ertuğrul & Öğretmen - Görsel Uyarılı Güvenlik (RFID + RGB LED)
  Bileşenler: RC522 RFID, I2C LCD, SG90 Servo, RGB LED
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// --- PİN TANIMLARI ---
#define RST_PIN 9
#define SS_PIN 10
#define SERVO_PIN 3

// RGB LED Pinleri
#define KIRMIZI_PIN 5
#define YESIL_PIN 6
#define MAVI_PIN 7

// --- MODÜL KURULUMLARI ---
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2); 
Servo kapiMotoru;

// --- KART ŞİFRELERİ ---
String YETKILI_KART_1 = "birinci_sifre_buraya"; // Örn: Mavi Anahtarlık
String YETKILI_KART_2 = "ikinci_sifre_buraya";  // Örn: Beyaz Kart

void setup() {
  Serial.begin(9600);
  SPI.begin();       
  rfid.PCD_Init();   
  
  kapiMotoru.attach(SERVO_PIN);
  kapiMotoru.write(0); // Kapı Kilitli

  // RGB Pinlerini çıkış olarak ayarla
  pinMode(KIRMIZI_PIN, OUTPUT);
  pinMode(YESIL_PIN, OUTPUT);
  pinMode(MAVI_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();
  
  durumuSifirla(); // Ekranı ve LED'i bekleme moduna al
  
  Serial.println("--- SISTEM HAZIR ---");
}

void loop() {
  // Yeni kart okutulmadıysa başa dön
  if ( ! rfid.PICC_IsNewCardPresent()) {
    return; 
  }
  
  // Kartın numarası okunamadıysa başa dön
  if ( ! rfid.PICC_ReadCardSerial()) {
    return; 
  }

  // Kart numarasını metne çevir
  String okunanUID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    okunanUID += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    okunanUID += String(rfid.uid.uidByte[i], HEX);
  }

  Serial.print("Okunan Kart: ");
  Serial.println(okunanUID);

  // --- ŞİFRE KONTROL BÖLÜMÜ ---
  
  if (okunanUID == YETKILI_KART_1 || okunanUID == YETKILI_KART_2) {
    // --- DOĞRU KART ---
    kapiyiAc();
  } 
  else {
    // --- YANLIŞ KART ---
    hataliGiris();
  }

  rfid.PICC_HaltA(); // Modülü uyut
}

// ==========================================
// YARDIMCI FONKSİYONLAR
// ==========================================

void kapiyiAc() {
  // LED'i Yeşil Yap
  renkAyarla(LOW, HIGH, LOW); 
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Giris Basarili!");
  lcd.setCursor(0, 1);
  lcd.print("Kapi Acildi...");

  kapiMotoru.write(90); // Kapıyı aç
  delay(4000);          // 4 saniye bekle
  kapiMotoru.write(0);  // Kapıyı kilitle
  
  durumuSifirla();      // Sistemi eski haline getir
}

void hataliGiris() {
  // LED'i Kırmızı Yap
  renkAyarla(HIGH, LOW, LOW); 
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HATA:");
  lcd.setCursor(0, 1);
  lcd.print("Yetkisiz Kart!");
  
  delay(2000); // 2 saniye uyarı ver
  
  durumuSifirla(); // Sistemi eski haline getir
}

void durumuSifirla() {
  // LED'i Mavi Yap (Bekleme Modu)
  renkAyarla(LOW, LOW, HIGH); 
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sistem Kilitli");
  lcd.setCursor(0, 1);
  lcd.print("Karti Okutunuz..");
}

// RGB LED Renk Kontrol Fonksiyonu
void renkAyarla(bool k, bool y, bool m) {
  digitalWrite(KIRMIZI_PIN, k);
  digitalWrite(YESIL_PIN, y);
  digitalWrite(MAVI_PIN, m);
}