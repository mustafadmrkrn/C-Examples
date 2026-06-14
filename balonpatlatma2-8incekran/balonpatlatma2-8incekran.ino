#include <SoftwareSerial.h>

// Bluetooth için sanal seri port (RX, TX)
SoftwareSerial btModul(2, 3); 

// RGB LED Pinleri (PWM destekli pinler olmalı: 3,5,6,9,10,11)
const int kirmiziPin = 9;
const int yesilPin = 10;
const int maviPin = 11;

char gelenVeri; // Telefondan gelen harfi tutacak

void setup() {
  // Pinleri çıkış olarak ayarla
  pinMode(kirmiziPin, OUTPUT);
  pinMode(yesilPin, OUTPUT);
  pinMode(maviPin, OUTPUT);

  // Seri haberleşmeyi başlat
  Serial.begin(9600);   // Bilgisayar ekranı için
  btModul.begin(9600);  // Bluetooth modülü için
  
  Serial.println("Sistem Hazir! Bluetooth baglanabilir.");
}

void loop() {
  // Bluetooth'tan veri gelip gelmediğini kontrol et
  if (btModul.available()) {
    gelenVeri = btModul.read(); // Veriyi oku
    Serial.print("Gelen Komut: ");
    Serial.println(gelenVeri);

    // Gelen harfe göre renk yak
    if (gelenVeri == 'k') { // k harfi gelirse KIRMIZI
      renkAyarla(255, 0, 0);
    }
    else if (gelenVeri == 'y') { // y harfi gelirse YEŞİL
      renkAyarla(0, 255, 0);
    }
    else if (gelenVeri == 'm') { // m harfi gelirse MAVİ
      renkAyarla(0, 0, 255);
    }
    else if (gelenVeri == 'b') { // b harfi gelirse BEYAZ
      renkAyarla(255, 255, 255);
    }
    else if (gelenVeri == 's') { // s harfi gelirse SÖNDÜR
      renkAyarla(0, 0, 0);
    }
  }
}

// Renkleri ayarlamak için yardımcı fonksiyon
void renkAyarla(int kirmizi, int yesil, int mavi) {
  analogWrite(kirmiziPin, kirmizi);
  analogWrite(yesilPin, yesil);
  analogWrite(maviPin, mavi);
}