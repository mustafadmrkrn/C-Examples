#include "LedControl.h"

// --- PİN AYARLARI ---
#define DIN_PIN 11
#define CS_PIN 10
#define CLK_PIN 13
#define TRIG_PIN 6
#define ECHO_PIN 5

// DİKKAT: En sondaki '1' sayısını '2' yaptık! (2 adet Matrix kullanıyoruz)
LedControl ekran = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 2);

// --- PİKSEL ÇİZİMLERİ ---

byte gozAcik[8] = {
  B00000000,
  B00111100, // Gözün üst sınırı
  B01000010,
  B01011010, // Göz bebeği
  B01011010, // Göz bebeği
  B01000010,
  B00111100, // Gözün alt sınırı
  B00000000
};

byte gozKapali[8] = {
  B00000000,
  B00000000,
  B00000000,
  B01111110, // Kapalı göz çizgisi
  B00000000,
  B00000000,
  B00000000,
  B00000000
};

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Her İKİ ekranı da başlat ve temizle
  for(int i = 0; i < 2; i++) {
    ekran.shutdown(i, false);  // Ekranı uyandır
    ekran.setIntensity(i, 5);  // Parlaklık (0-15)
    ekran.clearDisplay(i);     // Ekranı temizle
  }
  
  Serial.begin(9600);
}

void loop() {
  // 1. Mesafeyi Ölç
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  int mesafe = duration * 0.034 / 2;

  // 2. Karar Ver ve Çiz
  if (mesafe > 0 && mesafe < 30) { 
    // Biri 30 cm'den yakına geldiyse: İki gözü birden kırp
    gozKirpmaAnimasyonu();
    delay(1000); 
  } else {
    // Yakında kimse yoksa: İki göz de açık
    ikiGozuDeCiz(gozAcik);
  }
  
  delay(100); 
}

// --- YARDIMCI FONKSİYONLAR ---

// İki gözü de aynı anda kırpan animasyon
void gozKirpmaAnimasyonu() {
  ikiGozuDeCiz(gozKapali);
  delay(150);
  ikiGozuDeCiz(gozAcik);
  delay(150);
  ikiGozuDeCiz(gozKapali);
  delay(150);
  ikiGozuDeCiz(gozAcik);
}

// Seçilen şekli her iki matrise de aynı anda basan fonksiyon
void ikiGozuDeCiz(byte sekil[]) {
  for (int satir = 0; satir < 8; satir++) {
    ekran.setRow(0, satir, sekil[satir]); // 1. Ekran (Sağ Göz)
    ekran.setRow(1, satir, sekil[satir]); // 2. Ekran (Sol Göz)
  }
}