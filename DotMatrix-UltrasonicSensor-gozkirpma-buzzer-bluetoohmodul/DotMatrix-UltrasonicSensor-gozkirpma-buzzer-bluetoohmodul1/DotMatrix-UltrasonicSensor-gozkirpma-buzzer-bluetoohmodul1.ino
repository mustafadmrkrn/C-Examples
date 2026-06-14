#include "LedControl.h"
#include <SoftwareSerial.h>

// --- PİN AYARLARI ---
#define BT_RX_PIN 2  // Bluetooth TX buraya
#define BT_TX_PIN 3  // Bluetooth RX buraya
#define MIC_PIN 4    // Mikrofon Dijital Çıkış
#define ECHO_PIN 5   // Ultrasonik Echo
#define TRIG_PIN 6   // Ultrasonik Trig
#define BUZZER_PIN 8 // Buzzer Sinyal
#define CS_PIN 10    // Matrix CS
#define DIN_PIN 11   // Matrix DIN
#define CLK_PIN 13   // Matrix CLK

// Modül Tanımlamaları
SoftwareSerial btModul(BT_RX_PIN, BT_TX_PIN); 
LedControl ekran = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 2);

// --- PİKSEL ÇİZİMLERİ (MİMİKLER) ---
byte gozNormal[8]  = { B00000000, B00111100, B01000010, B01011010, B01011010, B01000010, B00111100, B00000000 };
byte gozKapali[8]  = { B00000000, B00000000, B00000000, B01111110, B00000000, B00000000, B00000000, B00000000 };
byte gozKizgin[8]  = { B00000000, B11111111, B01011010, B00011000, B00011000, B01000010, B00111100, B00000000 };
byte gozGulumse[8] = { B00000000, B00000000, B01000010, B01000010, B01000010, B00111100, B00000000, B00000000 };
byte gozKorkmus[8] = { B01111110, B10000001, B10011001, B10011001, B10011001, B10011001, B10000001, B01111110 };

void setup() {
  // Pin Modları
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MIC_PIN, INPUT); 

  // Ekran Başlatma
  for(int i = 0; i < 2; i++) {
    ekran.shutdown(i, false);
    ekran.setIntensity(i, 4); // Parlaklık
    ekran.clearDisplay(i);
  }
  
  Serial.begin(9600);
  btModul.begin(9600);
  Serial.println("==================================================");
  Serial.println("Sistem Hazir! Bluetooth, Mikrofon ve Mesafe devrede.");
  Serial.println("==================================================");
}

void loop() {
  // --- 1. BLUETOOTH (TELEFON) KONTROLÜ ---
  if (btModul.available() > 0) {
    String gelenMesaj = btModul.readStringUntil('\n'); 
    gelenMesaj.trim(); 
    
    // Telefondan gelen mesajı Seri Monitör'e yazdır
    Serial.print("[BLUETOOTH] Gelen Mesaj: ");
    Serial.println(gelenMesaj);

    if (gelenMesaj == "Merhaba" || gelenMesaj == "merhaba") {
      merhabaTepkisi();
    } 
    else if (gelenMesaj == "Nasilsin" || gelenMesaj == "nasılsın" || gelenMesaj == "nasilsin") {
      gulumsemeTepkisi();
    } else {
      Serial.println("[BLUETOOTH] Bilinmeyen komut alindi.");
    }
  } 
  
  // --- 2. MİKROFON (SES) KONTROLÜ ---
  else if (digitalRead(MIC_PIN) == HIGH) {
    korkuTepkisi();
  }

  // --- 3. OTONOM SENSÖR (MESAFE) KONTROLÜ ---
  else {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH);
    int mesafe = duration * 0.034 / 2;

    // Mesafeyi Seri Monitör'e yazdır
    Serial.print("[SENSÖR] Güncel Mesafe: ");
    Serial.print(mesafe);
    Serial.println(" cm");

    // Çok Yakınsa -> Kızgın Mod
    if (mesafe > 0 && mesafe < 10) {
      Serial.println("[DURUM] Çok Yakın! Robot KIZGIN.");
      ikiGozuDeCiz(gozKizgin);
      tone(BUZZER_PIN, 400); 
    } 
    // Normalse -> Sakin Mod
    else {
      noTone(BUZZER_PIN);
      ikiGozuDeCiz(gozNormal);
    }
    
    // Seri monitördeki yazıların okunamayacak kadar hızlı akmasını engellemek için
    // bekleme süresini 50'den 250'ye çıkardım. (Saniyede 4 kere ölçüm yazar)
    delay(250);
  }
}

// --- TEPKİ FONKSİYONLARI ---

void korkuTepkisi() {
  Serial.println("[MİKROFON] Yüksek ses algılandı! Robot KORKTU.");
  tone(BUZZER_PIN, 2000, 100); // Kısa tiz ses
  ikiGozuDeCiz(gozKorkmus);
  delay(1500); 
  ikiGozuDeCiz(gozNormal); 
  delay(500); 
}

void merhabaTepkisi() {
  Serial.println("[DURUM] Merhaba animasyonu oynatılıyor...");
  tone(BUZZER_PIN, 800, 150); delay(150); 
  tone(BUZZER_PIN, 1200, 200);
  ikiGozuDeCiz(gozKapali); delay(200); 
  ikiGozuDeCiz(gozNormal); delay(200);
  ikiGozuDeCiz(gozKapali); delay(200); 
  ikiGozuDeCiz(gozNormal); delay(1000); 
}

void gulumsemeTepkisi() {
  Serial.println("[DURUM] Gülümseme animasyonu oynatılıyor...");
  tone(BUZZER_PIN, 1500, 100); delay(100); 
  tone(BUZZER_PIN, 2000, 100); delay(100); 
  tone(BUZZER_PIN, 2500, 200);
  ikiGozuDeCiz(gozGulumse); 
  delay(3000); 
  ikiGozuDeCiz(gozNormal); 
}

void ikiGozuDeCiz(byte sekil[]) {
  for (int satir = 0; satir < 8; satir++) {
    ekran.setRow(0, satir, sekil[satir]); 
    ekran.setRow(1, satir, sekil[satir]); 
  }
}