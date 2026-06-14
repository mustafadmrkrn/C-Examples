// ESP32 RGB LED Pinleri
const int kirmiziPin = 25;
const int yesilPin = 26;
const int maviPin = 27;

void setup() {
  // Pinleri çıkış (OUTPUT) olarak ayarlıyoruz
  pinMode(kirmiziPin, OUTPUT);
  pinMode(yesilPin, OUTPUT);
  pinMode(maviPin, OUTPUT);
}

void loop() {
  // renkAyarla(Kırmızı, Yeşil, Mavi); -> Değerler 0 ile 255 arası olmalıdır.
  
  // Ana Renkler
  renkAyarla(255, 0, 0);     // Tam Parlak Kırmızı
  delay(1000);
  
  renkAyarla(0, 255, 0);     // Tam Parlak Yeşil
  delay(1000);
  
  renkAyarla(0, 0, 255);     // Tam Parlak Mavi
  delay(1000);

  // Ara Renkler (Karışımlar)
  renkAyarla(255, 255, 0);   // Sarı (Kırmızı + Yeşil)
  delay(1000);

  renkAyarla(128, 0, 128);   // Mor (Yarı Kırmızı + Yarı Mavi)
  delay(1000);

  renkAyarla(0, 255, 255);   // Turkuaz / Cyan (Yeşil + Mavi)
  delay(1000);

  renkAyarla(255, 100, 0);   // Turuncu
  delay(1000);

  // Hepsi Açık ve Hepsi Kapalı
  renkAyarla(255, 255, 255); // Beyaz (Tüm renkler maksimumda)
  delay(1000);

  renkAyarla(0, 0, 0);       // Kapalı (Tüm renkler sıfır)
  delay(1000);
}

// Renkleri 0-255 arası PWM sinyali ile karıştırma fonksiyonu
void renkAyarla(int kirmiziDeger, int yesilDeger, int maviDeger) {
  analogWrite(kirmiziPin, kirmiziDeger);
  analogWrite(yesilPin, yesilDeger);
  analogWrite(maviPin, maviDeger);
}