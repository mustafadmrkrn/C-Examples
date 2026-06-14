/*
  Ertuğrul & Öğretmen - Duyan ve Konuşan Duygusal Robot Gözler
  Bileşenler: 2x Dot Matrix, HC-SR04, Ses Sensörü (D3), Buzzer (D7)
*/

#include <LedControl.h>

// --- PİN TANIMLARI ---
#define TRIG_PIN 6
#define ECHO_PIN 5
#define SES_SENSOR_PIN 3
#define BUZZER_PIN 7

LedControl lc = LedControl(12, 11, 10, 2); 

// ==========================================
// PİKSEL SANATI: GÖZ KARELERİ
// ==========================================
byte normalGoz[8] = { B00111100, B01111110, B11111111, B11111111, B11111111, B11111111, B01111110, B00111100 };
byte kapaliGoz[8] = { B00000000, B00000000, B00000000, B11111111, B11111111, B00000000, B00000000, B00000000 };
byte solaBakan[8] = { B00111100, B01111110, B11110000, B11110000, B11111111, B11111111, B01111110, B00111100 };
byte sagaBakan[8] = { B00111100, B01111110, B00001111, B00001111, B11111111, B11111111, B01111110, B00111100 };

// YENİ: Korkmuş / Şok Olmuş Göz (Göz bebeği çok küçük)
byte korkmusGoz[8] = { B00000000, B00111100, B01000010, B01011010, B01011010, B01000010, B00111100, B00000000 };

void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SES_SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  for(int i=0; i<2; i++) {
    lc.shutdown(i, false); 
    lc.setIntensity(i, 5); 
    lc.clearDisplay(i);     
  }

  // Uyanma Sesi (Mutlu Bip)
  tone(BUZZER_PIN, 1000, 100); delay(150);
  tone(BUZZER_PIN, 1500, 150); delay(100);
}

void loop() {
  // 1. ÖNCE KULAKLARI KONTROL ET (Öncelikli Durum)
  // Genelde ses sensörleri ses duyduğunda HIGH (1) sinyali verir
  int sesDurumu = digitalRead(SES_SENSOR_PIN);
  
  if (sesDurumu == HIGH) {
    Serial.println("!!! YUKSEK SES DUYULDU !!!");
    
    // Korkma Animasyonu ve Sesi
    ekranaCiz(korkmusGoz);
    
    // Robotik Çığlık (Tiz ve Hızlı)
    tone(BUZZER_PIN, 3000); delay(50);
    tone(BUZZER_PIN, 2000); delay(50);
    tone(BUZZER_PIN, 3500); delay(100);
    noTone(BUZZER_PIN);
    
    delay(1000); // 1 saniye boyunca şokta bekle
    return; // loop'u burada kes ve baştan başla (mesafeye bakma)
  }

  // 2. SES YOKSA GÖZLERİ KONTROL ET
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long sure = pulseIn(ECHO_PIN, HIGH, 30000); 
  int mesafe = (sure == 0) ? 400 : (sure * 0.034 / 2); 
  
  // Hareket Animasyonu
  if (mesafe > 0 && mesafe < 30) {
    // Yakında biri var, meraklı sesler çıkararak etrafa bak
    ekranaCiz(solaBakan);
    tone(BUZZER_PIN, 800, 50); // Minik tıklama sesi
    delay(400);
    
    ekranaCiz(sagaBakan);
    tone(BUZZER_PIN, 900, 50); 
    delay(400);
    
    ekranaCiz(normalGoz);
    delay(200);
    ekranaCiz(kapaliGoz); delay(150); 
    ekranaCiz(normalGoz); delay(500); 
  } 
  else {
    ekranaCiz(normalGoz);
    delay(100); 
  }
}

void ekranaCiz(byte tasarim[]) {
  for(int satir = 0; satir < 8; satir++) {
    lc.setRow(0, satir, tasarim[satir]); 
    lc.setRow(1, satir, tasarim[satir]); 
  }
}