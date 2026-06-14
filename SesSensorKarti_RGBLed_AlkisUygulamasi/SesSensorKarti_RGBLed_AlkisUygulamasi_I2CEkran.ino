#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Tinkercad için 0x20, gerçek ekranlar için genelde 0x27 veya 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- PIN TANIMLARI ---
const int PIN_SES      = 2;
const int PIN_KIRMIZI  = 9;
const int PIN_YESIL    = 10;
const int PIN_MAVI     = 11;

// --- SİSTEM AYARLARI ---
bool ortakArtiLed = true; // LED Ortak Artı ise true, Ortak Eksi ise false yapın

void setup() {
  pinMode(PIN_SES, INPUT);
  pinMode(PIN_KIRMIZI, OUTPUT);
  pinMode(PIN_YESIL, OUTPUT);
  pinMode(PIN_MAVI, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Gurultu Olcer");
  lcd.setCursor(0, 1);
  lcd.print("Baslatiliyor...");
  delay(1500);
  lcd.clear();
}

void loop() {
  int sesSayaci = 0;
  unsigned long baslangicZamani = millis();

  // 200 milisaniye (saniyenin beşte biri) boyunca ortamdaki ses tıklarını say
  while (millis() - baslangicZamani < 200) {
    // Sensör ses algıladığında genellikle LOW (0) verir
    if (digitalRead(PIN_SES) == LOW) {
      sesSayaci++;
      delay(1); // Kısa bir filtreleme süresi
    }
  }

  // Ekranda çizilecek bar (||||) sayısını hesapla (Maksimum 16 bar)
  // Ortamdaki sensörün hassasiyetine göre 0-50 sınırlarını değiştirebilirsiniz
  int barSayisi = map(sesSayaci, 0, 50, 0, 16); 
  if (barSayisi > 16) barSayisi = 16;

  // --- SEVİYE KONTROLLERİ VE EKRAN GÜNCELLEME ---
  lcd.clear();
  lcd.setCursor(0, 0);

  if (sesSayaci < 5) {
    // 1. SEVİYE: SAKIN ORTAM
    lcd.print("Durum: Sakin");
    renkAyarla(0, 255, 0); // Yeşil Işık
  } 
  else if (sesSayaci >= 5 && sesSayaci < 25) {
    // 2. SEVİYE: NORMAL ORTAM
    lcd.print("Durum: Normal");
    renkAyarla(255, 150, 0); // Turuncu/Sarı Işık
  } 
  else {
    // 3. SEVİYE: ÇOK GÜRÜLTÜLÜ
    lcd.print("GURULTULU!");
    renkAyarla(255, 0, 0); // Kırmızı Işık
  }

  // İkinci satıra ses şiddetine göre dinamik grafik (||||||) çiz
  lcd.setCursor(0, 1);
  for (int i = 0; i < barSayisi; i++) {
    lcd.print("|"); // Her seviye için bir çizgi bas
  }
}

// ==========================================
// RGB LED RENK FONKSİYONU
// ==========================================
void renkAyarla(int kirmizi, int yesil, int mavi) {
  if (ortakArtiLed == true) {
    analogWrite(PIN_KIRMIZI, 255 - kirmizi);
    analogWrite(PIN_YESIL, 255 - yesil);
    analogWrite(PIN_MAVI, 255 - mavi);
  } else {
    analogWrite(PIN_KIRMIZI, kirmizi);
    analogWrite(PIN_YESIL, yesil);
    analogWrite(PIN_MAVI, mavi);
  }
}