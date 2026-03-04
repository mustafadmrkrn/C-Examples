/*
  Ertuğrul - Kalibrasyonlu ve Susturulabilir Gaz Alarmı
  Amaç: Seri porttan değeri okuyup, eşik değerini kendimiz belirlemek.
*/

// --- AYAR KISMI (BURAYI SİZ BELİRLEYECEKSİNİZ) ---
// Adım 1'de ölçtüğünüz değere göre burayı değiştirin!
int ESIK_DEGERI = 400; 

// --- PİN TANIMLARI ---
#define GAZ_SENSORU A0
#define BUZZER_PIN 8     // Buzzer
#define BUTON_PIN 4      // Susturma Butonu
#define KIRMIZI_PIN 9
#define YESIL_PIN 10
#define MAVI_PIN 11

// --- DEĞİŞKENLER ---
int anlikGazDegeri = 0;
bool alarmSusturuldu = false; // Susturma hafızası

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(KIRMIZI_PIN, OUTPUT);
  pinMode(YESIL_PIN, OUTPUT);
  pinMode(MAVI_PIN, OUTPUT);
  
  // Buton için dahili direnç (Basınca LOW olur)
  pinMode(BUTON_PIN, INPUT_PULLUP);
  
  Serial.begin(9600); // Seri haberleşmeyi başlat
  
  Serial.println("SENSOR ISINIYOR... Lutfen bekleyin...");
  delay(2000); 
  Serial.println("Olcum Basladi!");
}

void loop() {
  // 1. ADIM: Sensörü Oku
  anlikGazDegeri = analogRead(GAZ_SENSORU);

  // 2. ADIM: Ekrana Yazdır (Görmek İstediğiniz Kısım)
  Serial.print("Anlik Gaz: ");
  Serial.print(anlikGazDegeri);
  Serial.print("\t Esik Degeri: "); // Yanına belirlediğimiz sınırı da yazalım
  Serial.println(ESIK_DEGERI);

  // 3. ADIM: Buton Kontrolü (Susturma)
  if (digitalRead(BUTON_PIN) == LOW) {
    alarmSusturuldu = true;
    Serial.println("--- ALARM ELLE SUSTURULDU! ---");
    delay(200); // Buton arkı (titreşimi) önleme
  }

  // 4. ADIM: Karar Mekanizması
  if (anlikGazDegeri > ESIK_DEGERI) {
    // --- TEHLİKE VAR ---
    renkAyarla(255, 0, 0); // Kırmızı Işık (Hep yanar)

    if (alarmSusturuldu == false) {
      // Eğer susturulmadıysa ötmeye devam et
      tone(BUZZER_PIN, 1000); 
    } else {
      // Susturulduysa sus
      noTone(BUZZER_PIN);
    }
  } 
  else {
    // --- GÜVENLİ ---
    renkAyarla(0, 255, 0); // Yeşil Işık
    noTone(BUZZER_PIN);
    
    // Gaz gitti, tehlike geçti. Susturma hafızasını sıfırla.
    // Böylece bir sonraki tehlikede alarm tekrar çalışabilir.
    if (alarmSusturuldu == true) {
      alarmSusturuldu = false;
      Serial.println("--- ORTAM NORMALE DONDU. SISTEM SIFIRLANDI ---");
    }
  }
  
  delay(100); // Okumayı çok hızlı yapıp ekranı kitlemesin
}

void renkAyarla(int k, int y, int m) {
  analogWrite(KIRMIZI_PIN, k);
  analogWrite(YESIL_PIN, y);
  analogWrite(MAVI_PIN, m);
}