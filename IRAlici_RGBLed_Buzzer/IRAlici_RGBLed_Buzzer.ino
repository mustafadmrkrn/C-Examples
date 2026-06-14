/*
  Ertuğrul & Öğretmen - Faz 1: Akıllı Gece Lambası (GELİŞMİŞ İZLEME)
  Bileşenler: IR Alıcı (D4), Buzzer (D7), RGB LED (D9, D10, D6)
*/

#include <IRremote.h>

// --- PİN TANIMLARI ---
#define IR_ALICI_PIN 4
#define BUZZER_PIN 7

#define KIRMIZI_PIN 9
#define YESIL_PIN 10
#define MAVI_PIN 6  // Timer çakışmasını önlemek için 6'da

// --- AKILLI ŞALTER ---
bool ortakArtiLed = true; // LED Ortak Artı ise true, Ortak Eksi ise false yapın

void setup() {
  // Bilgisayar haberleşmesini başlat
  Serial.begin(9600);
  
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(KIRMIZI_PIN, OUTPUT);
  pinMode(YESIL_PIN, OUTPUT);
  pinMode(MAVI_PIN, OUTPUT);

  // IR Alıcıyı Başlat
  IrReceiver.begin(IR_ALICI_PIN, ENABLE_LED_FEEDBACK);
  
  isigiKapat(); // Başlangıçta ışık kapalı
  
  Serial.println("=====================================");
  Serial.println("SISTEM HAZIR!");
  Serial.println("Kumandadan bir tusa basarak sifreleri kesfedin...");
  Serial.println("=====================================");
}

void loop() {
  // Eğer kumandadan görünmez bir sinyal (ışık) geldiyse:
  if (IrReceiver.decode()) {
    
    // Gelen sinyalin şifresini çöz ve kaydet
    int gelenKomut = IrReceiver.decodedIRData.command;
    
    // 0 sinyali genelde tuşa basılı tutulduğunda gelen tekrar (gürültü) sinyalidir, onu yoksayıyoruz
    if (gelenKomut != 0) {