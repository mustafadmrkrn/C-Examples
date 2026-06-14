#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// I2C Adresi (Ekranında yazı çıkmazsa 0x27 yerine 0x3F yazmayı dene)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- PIN TANIMLAMALARI ---
const int PIN_BUZZER = 10; // Yeni standart buzzer pinimiz

// --- KEYPAD AYARLARI ---
const byte SATIR = 4;
const byte SUTUN = 4;

// Senin İstediğin Özel Tuş Dizilimi
char tusTakimi[SATIR][SUTUN] = {
  {'1', '2', '3', 'A'}, // A = +
  {'4', '5', '6', 'B'}, // B = -
  {'7', '8', '9', 'C'}, // C = *
  {'*', '0', '#', 'D'}  // D = /, * = Sil (Clear), # = Eşittir
};

// Keypad pinleri (Soldan sağa: 9, 8, 7, 6, 5, 4, 3, 2)
byte satirPinleri[SATIR] = {9, 8, 7, 6}; 
byte sutunPinleri[SUTUN] = {5, 4, 3, 2}; 

Keypad keypad = Keypad(makeKeymap(tusTakimi), satirPinleri, sutunPinleri, SATIR, SUTUN);

// --- MATEMATİK DEĞİŞKENLERİ ---
String girilenSayi = "";
float sayi1 = 0;
float sayi2 = 0;
char islem = ' ';
boolean islemSecildi = false;
boolean sonucGosterildi = false; // Eşittire bastıktan sonra yeni sayıyı temiz başlatmak için

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BUZZER, OUTPUT);
  
  // Ekranı Başlat
  lcd.init();
  lcd.backlight();
  
  // Açılış Ekranı ve Sesi
  lcd.setCursor(0, 0);
  lcd.print(" Sesli Makine   ");
  lcd.setCursor(0, 1);
  lcd.print("   Yusuf Emir   ");
  
  // Açılış Melodisi (Kısa 2 bip)
  tone(PIN_BUZZER, 2000, 100); delay(150);
  tone(PIN_BUZZER, 2500, 150); delay(2000);
  
  lcd.clear();
}

void loop() {
  char tus = keypad.getKey();

  // Eğer herhangi bir tuşa basıldıysa
  if (tus) {
    // Standart Tuş Sesi (Tık sesi)
    tone(PIN_BUZZER, 1500, 50); 
    
    // --- TEMİZLE ( * ) TUŞU ---
    if (tus == '*') {
      temizle();
      tone(PIN_BUZZER, 1000, 150); // Silme sesi biraz daha kalın
      return; 
    }

    // --- RAKAM GİRİŞİ (0-9) ---
    if (tus >= '0' && tus <= '9') {
      // Eğer az önce bir sonuç gösterildiyse ve yeni rakama basıldıysa ekranı otomatik temizle
      if (sonucGosterildi && !islemSecildi) {
        temizle();
      }
      girilenSayi += tus;
      lcd.print(tus);
    }
    
    // --- DÖRT İŞLEM TUŞLARI (A, B, C, D) ---
    else if (tus == 'A' || tus == 'B' || tus == 'C' || tus == 'D') {
      if (girilenSayi.length() > 0) {
        sayi1 = girilenSayi.toFloat(); // İlk sayıyı kaydet
        girilenSayi = "";              // İkinci sayı için hafızayı boşalt
        
        // Senin dizilimine göre harfleri sembole çevir
        if (tus == 'A') islem = '+';
        if (tus == 'B') islem = '-';
        if (tus == 'C') islem = '*';
        if (tus == 'D') islem = '/';
        
        lcd.print(islem);
        islemSecildi = true;
        sonucGosterildi = false;
      }
    }
    
    // --- EŞİTTİR ( # ) TUŞU ---
    else if (tus == '#') {
      // Hem ilk sayı girilmiş, hem işlem seçilmiş, hem de ikinci sayı girilmiş olmalı
      if (girilenSayi.length() > 0 && islemSecildi == true) {
        sayi2 = girilenSayi.toFloat(); 
        float sonuc = 0;
        
        // İşlemleri Yap
        if (islem == '+') sonuc = sayi1 + sayi2;
        if (islem == '-') sonuc = sayi1 - sayi2;
        if (islem == '*') sonuc = sayi1 * sayi2;
        if (islem == '/') {
          // Sıfıra bölme hatası kontrolü
          if (sayi2 == 0) {
            tone(PIN_BUZZER, 300, 500); // Hata Sesi (Uzun ve kalın)
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(" HATA!");
            lcd.setCursor(0, 1);
            lcd.print("Sifira Bolunmez!");
            delay(2000);
            temizle();
            return;
          } else {
            sonuc = sayi1 / sayi2;
          }
        }
        
        // Eşittir Sesi (Başarı melodisi gibi ince)
        tone(PIN_BUZZER, 3000, 100);
        
        // Sonucu 2. Satıra Yazdır
        lcd.setCursor(0, 1);
        lcd.print("= ");
        lcd.print(sonuc);
        
        // Bir sonraki işleme hazırlık
        girilenSayi = String(sonuc); // Çıkan sonuçla işleme devam edebilmek için
        islemSecildi = false;
        sonucGosterildi = true;
      }
    }
  }
}

// Tüm ekranı ve hafızayı sıfırlayan fonksiyon
void temizle() {
  lcd.clear();
  lcd.setCursor(0, 0);
  girilenSayi = "";
  sayi1 = 0;
  sayi2 = 0;
  islem = ' ';
  islemSecildi = false;
  sonucGosterildi = false;
}