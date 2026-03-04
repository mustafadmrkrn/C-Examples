#include "LedControl.h"

// --- PİN AYARLARI ---
#define DIN_PIN 11
#define CS_PIN 10
#define CLK_PIN 13
#define TRIG_PIN 6
#define ECHO_PIN 5
#define BUZZER_PIN 8 // YENİ: Buzzer Pini

LedControl ekran = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 2);

// --- PİKSEL ÇİZİMLERİ (MİMİKLER) ---
byte gozNormal[8] = { B00000000, B00111100, B01000010, B01011010, B01011010, B01000010, B00111100, B00000000 };
byte gozKapali[8] = { B00000000, B00000000, B00000000, B01111110, B00000000, B00000000, B00000000, B00000000 };
byte gozSol[8]    = { B00000000, B00111100, B01000010, B01101000, B01101000, B01000010, B00111100, B00000000 };
byte gozSag[8]    = { B00000000, B00111100, B01000010, B00010110, B00010110, B01000010, B00111100, B00000000 };
byte gozSaskin[8] = { B00111100, B01000010, B10000001, B10011001, B10011001, B10000001, B01000010, B00111100 };
byte gozKizgin[8] = { B00000000, B11111111, B01011010, B00011000, B00011000, B01000010, B00111100, B00000000 };
byte harfZ[8]     = { B00000000, B01111110, B00000100, B00001000, B00010000, B00100000, B01111110, B00000000 };

// --- ZAMANLAMA DEĞİŞKENLERİ ---
unsigned long sonGormeZamani = 0;   
unsigned long sonKirpmaZamani = 0;  
unsigned long sonBakmaZamani = 0;   
unsigned long sonUykuAnimasyonu = 0;
unsigned long sonBuzzerZamani = 0; // YENİ: Buzzer ritmi için sayaç

bool sagaBakiyor = false; 
bool zGosteriyor = false; 
bool inceSes = false;      // YENİ: İnce ve kalın ses arasında geçiş için

const int uykuSuresi = 10000; 

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT); // YENİ: Buzzer çıkış olarak ayarlandı

  for(int i = 0; i < 2; i++) {
    ekran.shutdown(i, false);
    ekran.setIntensity(i, 4); 
    ekran.clearDisplay(i);
  }
  Serial.begin(9600);
}

void loop() {
  // 1. MESAFEYİ ÖLÇ
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  int mesafe = duration * 0.034 / 2;

  if (mesafe <= 0 || mesafe > 400) {
    mesafe = 400; 
  }

  // 2. KRONOMETREYİ OKU
  unsigned long suAn = millis();

  // 3. DUYGU MOTORU VE KARAR AĞACI
  if (mesafe < 40) {
    sonGormeZamani = suAn; 
  }

  // DURUM 1: KIZGIN VE BAĞIRAN MOD (< 10 cm)
  if (mesafe < 10) {
    ikiGozuDeCiz(gozKizgin);
    
    // Her 150 milisaniyede bir sesi değiştir (Siren efekti)
    if (suAn - sonBuzzerZamani > 150) {
      if (inceSes) {
        tone(BUZZER_PIN, 1200); // İnce rahatsız edici ses
      } else {
        tone(BUZZER_PIN, 600);  // Kalın rahatsız edici ses
      }
      inceSes = !inceSes; // Durumu tersine çevir
      sonBuzzerZamani = suAn;
    }
  } 
  
  // DİĞER TÜM DURUMLAR (Kişi 10 cm'den uzaksa)
  else {
    noTone(BUZZER_PIN); // YENİ: Robot kızgın değilse kesinlikle sussun
    
    // DURUM 2: ŞAŞKIN (10 - 20 cm)
    if (mesafe >= 10 && mesafe < 20) {
      ikiGozuDeCiz(gozSaskin);
    } 
    // DURUM 3: NORMAL GÖZ KIRPAN (20 - 40 cm)
    else if (mesafe >= 20 && mesafe < 40) {
      if (suAn - sonKirpmaZamani > 3000) { 
        ikiGozuDeCiz(gozKapali);
        delay(150);
        sonKirpmaZamani = suAn;
      } else {
        ikiGozuDeCiz(gozNormal);
      }
    } 
    // DURUM 4: UZAKTA KİMSE YOK (Uyku veya Sıkılma)
    else {
      if (suAn - sonGormeZamani > uykuSuresi) {
        // Uyku Modu (Zzz)
        if (suAn - sonUykuAnimasyonu > 1000) { 
          zGosteriyor = !zGosteriyor;
          sonUykuAnimasyonu = suAn;
        }
        if (zGosteriyor) {
          ikiGozuDeCiz(harfZ); 
        } else {
          ikiGozuDeCiz(gozKapali); 
        }
      } else {
        // Sıkılma Modu (Sağa sola bakma)
        if (suAn - sonBakmaZamani > 800) { 
          sagaBakiyor = !sagaBakiyor;
          sonBakmaZamani = suAn;
        }
        if (sagaBakiyor) {
          ikiGozuDeCiz(gozSag);
        } else {
          ikiGozuDeCiz(gozSol);
        }
      }
    }
  }
  
  delay(50); 
}

// --- YARDIMCI FONKSİYON ---
void ikiGozuDeCiz(byte sekil[]) {
  for (int satir = 0; satir < 8; satir++) {
    ekran.setRow(0, satir, sekil[satir]); 
    ekran.setRow(1, satir, sekil[satir]); 
  }
}