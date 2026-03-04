/*
  Ertuğrul & Öğretmen - Ramazan Su Hatırlatıcı (Jumper Kablo Butonlu)
  Bileşenler: DS1302 Saat, I2C LCD, RGB LED, 2x Jumper Kablo, Buzzer
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

// Ekran ve Saat Kurulumu
LiquidCrystal_I2C lcd(0x27, 16, 2); 
ThreeWire benimKablom(6, 7, 5); 
RtcDS1302<ThreeWire> saat(benimKablom);

// Pin Tanımları
#define KABLO_BUTON_PIN 2
#define BUZZER_PIN 8
#define KIRMIZI_PIN 9
#define YESIL_PIN 10
#define MAVI_PIN 11

// --- PROJE DEĞİŞKENLERİ ---
int bardakSayisi = 0;
bool sonKabloDurumu = HIGH;
unsigned long sonSuIcmeZamani = 0;

// --- ALARM SÜRESİ AYARI ---
// ŞU AN TEST İÇİN 10 SANİYE (10000 ms) AYARLIDIR.
// Gerçek kullanımda 10000 olan satırın başına // koyup, üsttekini açın.
// const unsigned long ALARM_SURESI = 30UL * 60UL * 1000UL; // GERÇEK 30 DAKİKA
const unsigned long ALARM_SURESI = 10000;                   // TEST İÇİN 10 SANİYE

bool alarmAktifMi = false;
bool gunlukSifirlamaYapildi = false;
int sonYazdirilanSaniye = -1;

void setup() {
  pinMode(KABLO_BUTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(KIRMIZI_PIN, OUTPUT);
  pinMode(YESIL_PIN, OUTPUT);
  pinMode(MAVI_PIN, OUTPUT);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  saat.Begin();

  // Saati ayarlamak isterseniz bu satırı SADECE BİR KERE açıp yükleyin, sonra geri kapatın:
  // saat.SetDateTime(RtcDateTime(2026, 3, 4, 20, 30, 0)); 

  sonSuIcmeZamani = millis(); 
}

void loop() {
  RtcDateTime simdi = saat.GetDateTime();
  unsigned long suankiZaman = millis();

  // --- 1. JUMPER KABLO KONTROLÜ ---
  bool okunanKablo = digitalRead(KABLO_BUTON_PIN);

  if (okunanKablo == LOW && sonKabloDurumu == HIGH) {
    delay(250); // Çıplak kablo titremesini (ark) önlemek için 250ms bekleme
    
    // 19:00'dan sonra veya gece yarısından sahura kadar (00:00 - 05:00 arası)
    if (simdi.Hour() >= 19 || simdi.Hour() < 5) {
      bardakSayisi++;
      sonSuIcmeZamani = suankiZaman; // Sayacı sıfırla
      
      if(alarmAktifMi == true) {
        alarmAktifMi = false;          // Alarmı sustur
        noTone(BUZZER_PIN);            // Sesi kesin olarak kapat
      }
      
      // Ekrana başarı mesajı yazdır
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Afiyet Olsun!");
      lcd.setCursor(0, 1);
      lcd.print("Toplam: ");
      lcd.print(bardakSayisi);
      
      tone(BUZZER_PIN, 2000, 200); // Başarı "bip" sesi
      delay(1500); 
      lcd.clear();
      sonYazdirilanSaniye = -1; // Ekranın hemen toparlanması için
    } 
    else {
      // Oruçluyken kablolar değdirilirse
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Henuz Iftar");
      lcd.setCursor(0, 1);
      lcd.print("Olmadi!");
      tone(BUZZER_PIN, 200, 500); // Hata sesi
      delay(2000);
      lcd.clear();
      sonYazdirilanSaniye = -1;
    }
  }