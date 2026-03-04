#include <SoftwareSerial.h>

// --- AYARLAR ---
// Bluetooth modülü için sanal port (TX, RX)
// Arduino Pin 2 -> Bluetooth TX ucuna bağlanacak
// Arduino Pin 3 -> Bluetooth RX ucuna bağlanacak
SoftwareSerial myBluetooth(2, 3); 

// RGB Modül Pin Tanımlamaları
const int pinKirmizi = 9;
const int pinYesil = 10;
const int pinMavi = 11;

char gelenVeri; // Telefondan gelen harfi tutacak değişken

void setup() {
  // Pinleri çıkış olarak ayarla
  pinMode(pinKirmizi, OUTPUT);
  pinMode(pinYesil, OUTPUT);
  pinMode(pinMavi, OUTPUT);

  // Seri haberleşmeleri başlat
  Serial.begin(9600);       // Bilgisayar ekranı için
  myBluetooth.begin(38400);  // Bluetooth modülü için (Genelde 9600'dür)

  // Başlangıçta LED'i söndür
  renkAyarla(0, 0, 0);
  Serial.println("Sistem Hazir! Bluetooth baglantisi bekleniyor...");
}

void loop() {
  // Bluetooth'tan veri gelip gelmediğini kontrol et
  if (myBluetooth.available()) {
    gelenVeri = myBluetooth.read(); // Veriyi oku ve değişkene at
    Serial.print("Gelen Komut: ");
    Serial.println(gelenVeri);

    // Gelen harfe göre işlem yap
    switch (gelenVeri) {
      case 'k': // 'k' gönderince KIRMIZI
        renkAyarla(255, 0, 0);
        break;
      case 'y': // 'y' gönderince YEŞİL
        renkAyarla(0, 255, 0);
        break;
      case 'm': // 'm' gönderince MAVİ
        renkAyarla(0, 0, 255);
        break;
      case 's': // 's' gönderince SARI
        renkAyarla(255, 255, 0);
        break;
      case 'u': // 'u' gönderince TURKUAZ (Cyan)
        renkAyarla(0, 255, 255);
        break;
      case 'p': // 'p' gönderince MOR (Purple)
        renkAyarla(255, 0, 255);
        break;
      case 'b': // 'b' gönderince BEYAZ
        renkAyarla(255, 255, 255);
        break;
      case 'x': // 'x' gönderince KAPAT
        renkAyarla(0, 0, 0);
        break;
    }
  }
}

// Renkleri ayarlayan ana fonksiyon
void renkAyarla(int kirmiziDegeri, int yesilDegeri, int maviDegeri) {
  // Ortak Katot (-) modül için kod:
  analogWrite(pinKirmizi, kirmiziDegeri);
  analogWrite(pinYesil, yesilDegeri);
  analogWrite(pinMavi, maviDegeri);
  
  /* NOT: Eğer modülün ORTAK ANOT (+) ise yukarıdaki 3 satırı sil,
     yerine aşağıdakileri yaz:
     analogWrite(pinKirmizi, 255 - kirmiziDegeri);
     analogWrite(pinYesil, 255 - yesilDegeri);
     analogWrite(pinMavi, 255 - maviDegeri);
  */
}