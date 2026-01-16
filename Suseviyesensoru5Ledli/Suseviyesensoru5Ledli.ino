// --- TANIMLAMALAR ---
// LED'lerin takılı olduğu pinler
int led1 = 2; // Kırmızı (En alt seviye)
int led2 = 3; // Kahverengi/Sarı
int led3 = 4; // Sarı
int led4 = 5; // Yeşil
int led5 = 6; // Mavi (En üst seviye)

// Su Sensörü Pini
int sensorPin = A0;

// Değişkenler
int suDegeri = 0; // Sensörden gelen ham veri

void setup() {
  // LED pinlerini çıkış olarak ayarla
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led5, OUTPUT);
  
  // Seri haberleşmeyi başlat (Değerleri görmek için)
  Serial.begin(9600);
}

void loop() {
  // 1. Sensörü Oku
  suDegeri = analogRead(sensorPin);
  
  // Ekrana yazdıralım (Kalibrasyon için çok önemli)
  Serial.print("Su Seviyesi: ");
  Serial.println(suDegeri);

  // 2. LED Kontrolü (BARAJ MANTIĞI)
  // Her LED'in yanması için gereken bir sınır değeri var.
  
  // 1. Seviye (Çok az su) - Sınır: 50
  if (suDegeri > 50) {
    digitalWrite(led1, HIGH);
  } else {
    digitalWrite(led1, LOW);
  }

  // 2. Seviye - Sınır: 150
  if (suDegeri > 150) {
    digitalWrite(led2, HIGH);
  } else {
    digitalWrite(led2, LOW);
  }

  // 3. Seviye (Orta) - Sınır: 300
  if (suDegeri > 300) {
    digitalWrite(led3, HIGH);
  } else {
    digitalWrite(led3, LOW);
  }

  // 4. Seviye - Sınır: 450
  if (suDegeri > 450) {
    digitalWrite(led4, HIGH);
  } else {
    digitalWrite(led4, LOW);
  }

  // 5. Seviye (Depo Dolu!) - Sınır: 600
  if (suDegeri > 600) {
    digitalWrite(led5, HIGH);
  } else {
    digitalWrite(led5, LOW);
  }

  // Okumalar çok hızlı akmasın diye minik bir bekleme
  delay(100);
}