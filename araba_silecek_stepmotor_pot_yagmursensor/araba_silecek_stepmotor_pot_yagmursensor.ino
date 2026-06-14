/*
  Ertuğrul & Öğretmen - FİZİKSEL BUTONLU Akıllı Hesap Makinesi
  A(+): Topla, B(-): Çıkar, C(X): Çarp, D(/): Böl
  *: 1 Adım Sil, #: Eşittir (=)
  BUTON (Pin 13): Mesafe Ölçer (Alan hesabı için)
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// --- MODÜL PİNLERİ ---
LiquidCrystal_I2C lcd(0x27, 16, 2); 
#define BUZZER_PIN 10
#define TRIG_PIN 11
#define ECHO_PIN 12
#define BUTON_PIN 13 // Yeni eklenen ölçüm butonumuz

// --- KEYPAD AYARLARI ---
const byte SATIR = 4; 
const byte SUTUN = 4; 
char tuslar[SATIR][SUTUN] = {
  {'1','2','3','+'}, // A = +
  {'4','5','6','-'}, // B = -
  {'7','8','9','X'}, // C = Çarpma (X)
  {'S','0','#','/'}  // * = Silme (S), D = Bölme (/)
};

byte satirPinleri[SATIR] = {9, 8, 7, 6}; 
byte sutunPinleri[SUTUN] = {5, 4, 3, 2}; 

Keypad hesapMakinesi = Keypad(makeKeymap(tuslar), satirPinleri, sutunPinleri, SATIR, SUTUN);

// --- HESAPLAMA DEĞİŞKENLERİ ---
String girilenVeri = "";
float ilkSayi = 0;
char islem = ' ';
bool sonucGosterildi = false;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUTON_PIN, INPUT_PULLUP); // Direnç kullanmadan butonu aktif ettik
  
  lcd.init();
  lcd.backlight();
  ekraniBaslat();
}

void loop() {
  
  // ==========================================
  // 1. FİZİKSEL BUTON İLE MESAFE ÖLÇÜMÜ
  // ==========================================
  if (digitalRead(BUTON_PIN) == LOW) {
    if (sonucGosterildi) { 
        girilenVeri = ""; 
        islem = ' ';
        sonucGosterildi = false; 
    }
    
    // Mesafeyi ölç ve direkt sanki klavyeden yazılmış gibi hafızaya al
    float olculenMesafe = mesafeOlc();
    girilenVeri = String(olculenMesafe, 0); // Virgülsüz santimetre olarak al
    
    radarSesi(); // Ölçüm yapıldığını bildiren farklı bir ses
    ekranGuncelle();
    
    delay(500); // Butona basılı tutulursa sürekli ölçüm yapmasını engelle
  }

  // ==========================================
  // 2. TUŞ TAKIMI (KEYPAD) KONTROLLERİ
  // ==========================================
  char tus = hesapMakinesi.getKey();

  if (tus) {
    if (tus != '#') { tusSesi(); } // # hariç her tuşta Dit Dit sesi

    // SAYI GİRİŞİ (Normal hesap makinesi kullanımı için)
    if (tus >= '0' && tus <= '9') {
      if (sonucGosterildi) { 
        girilenVeri = ""; 
        islem = ' ';
        sonucGosterildi = false; 
      }
      girilenVeri += tus;
      ekranGuncelle();
    }

    // İŞLEM SEÇİMİ (+, -, X, /)
    else if (tus == '+' || tus == '-' || tus == 'X' || tus == '/') {
      if (girilenVeri != "") {
        ilkSayi = girilenVeri.toFloat();
        islem = tus;
        girilenVeri = ""; // İkinci sayı için hafızayı temizle
        lcd.setCursor(15, 0);
        lcd.print(islem);
      }
    }

    // EŞİTTİR (#)
    else if (tus == '#') {
      esittirSesi(); // 3 kez Dit sesi çalar
      float ikinciSayi = girilenVeri.toFloat();
      float sonuc = 0;

      if (islem == '+') sonuc = ilkSayi + ikinciSayi;
      else if (islem == '-') sonuc = ilkSayi - ikinciSayi;
      else if (islem == 'X') sonuc = ilkSayi * ikinciSayi;
      else if (islem == '/') {
        if (ikinciSayi != 0) sonuc = ilkSayi / ikinciSayi;
        else { lcd.clear(); lcd.print("Hata: Sifira Bol"); delay(2000); ekraniBaslat(); return; }
      }

      lcd.setCursor(0, 1);
      lcd.print("=");
      lcd.print(sonuc);
      
      girilenVeri = String(sonuc); 
      ilkSayi = 0; 
      islem = ' ';
      sonucGosterildi = true;
    }

    // BİR ADIM SİL (*)
    else if (tus == 'S') {
      if (girilenVeri.length() > 0) {
        girilenVeri.remove(girilenVeri.length() - 1);
        ekranGuncelle();
      }
    }
  }
}

// ==========================================
// YARDIMCI FONKSİYONLAR
// ==========================================

// Sensörden mesafe ölçen fonksiyon
float mesafeOlc() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long sure = pulseIn(ECHO_PIN, HIGH);
  float mesafe = (sure / 2.0) * 0.0343; // cm cinsinden hesapla
  return mesafe;
}

void tusSesi() {
  tone(BUZZER_PIN, 2000, 50); delay(70);
  tone(BUZZER_PIN, 2000, 50);
}

void radarSesi() {
  // Ölçüm yapıldığında çalan tarama sesi
  tone(BUZZER_PIN, 1000, 100); delay(150);
  tone(BUZZER_PIN, 1500, 150); delay(200);
}

void esittirSesi() {
  // 3 kez Dit Dit Dit
  tone(BUZZER_PIN, 2500, 50); delay(70);
  tone(BUZZER_PIN, 2500, 50); delay(70);
  tone(BUZZER_PIN, 2500, 50);
}

void ekranGuncelle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(girilenVeri);
  if (islem != ' ') {
    lcd.setCursor(15, 0);
    lcd.print(islem);
  }
}

void ekraniBaslat() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ertugrul & Hoca");
  lcd.setCursor(0, 1);
  lcd.print("Alan Hesaplayici");
  delay(2000);
  lcd.clear();
  girilenVeri = "";
  ilkSayi = 0;
  islem = ' ';
}