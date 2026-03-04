// Pin Tanımlamaları (ISD1820)
const int pinREC = 2;   // Kayıt
const int pinPE = 3;    // Tıkla Çal (Play Edge)
const int pinPL = 4;    // Basılı Tut Çal (Play Level)

// Pin Tanımlamaları (RGB LED)
const int ledKirmizi = 9;
const int ledYesil = 10;
const int ledMavi = 11;

void setup() {
  // ISD1820 Pinleri Çıkış Olsun
  pinMode(pinREC, OUTPUT);
  pinMode(pinPE, OUTPUT);
  pinMode(pinPL, OUTPUT);

  // RGB LED Pinleri Çıkış Olsun
  pinMode(ledKirmizi, OUTPUT);
  pinMode(ledYesil, OUTPUT);
  pinMode(ledMavi, OUTPUT);

  // Başlangıçta her şeyi kapat (LOW)
  digitalWrite(pinREC, LOW);
  digitalWrite(pinPE, LOW);
  digitalWrite(pinPL, LOW);
  ledleriSondur();

  Serial.begin(9600);
  Serial.println("--- RGB + SES KAYIT SISTEMI ---");
  Serial.println("'k' -> KIRMIZI ISIK + Kayit Yap");
  Serial.println("'c' -> MAVI ISIK + Kaydi Cal (P-E)");
  Serial.println("'u' -> YESIL ISIK + Uzun Cal (P-L)");
}

void loop() {
  if (Serial.available() > 0) {
    char komut = Serial.read();

    // --- DURUM 1: KAYIT (REC) - KIRMIZI ---
    if (komut == 'k') {
      Serial.println("KIRMIZI: Kayit yapiliyor...");
      
      ledYak(HIGH, LOW, LOW);     // Sadece Kırmızı Yak
      digitalWrite(pinREC, HIGH); // Kaydı Başlat
      
      delay(5000);                // 5 Saniye Bekle
      
      digitalWrite(pinREC, LOW);  // Kaydı Bitir
      ledleriSondur();            // Işığı Kapat
      Serial.println("Kayit Bitti.");
    }

    // --- DURUM 2: TIKLA ÇAL (P-E) - MAVİ ---
    else if (komut == 'c') {
      Serial.println("MAVI: Kayit caliniyor (P-E)...");
      
      ledYak(LOW, LOW, HIGH);    // Sadece Mavi Yak
      digitalWrite(pinPE, HIGH); // Çalmayı Tetikle
      delay(100);                // Kısa bir sinyal yeterli
      digitalWrite(pinPE, LOW);  // Pini kapat (Modül çalmaya devam eder)
      
      // Ses çalarken ışığın yanık kalması için biraz bekletelim
      delay(5000); 
      ledleriSondur();
    }

    // --- DURUM 3: BASILI TUT ÇAL (P-L) - YEŞİL ---
    else if (komut == 'u') {
      Serial.println("YESIL: Uzun caliniyor (P-L)...");
      
      ledYak(LOW, HIGH, LOW);    // Sadece Yeşil Yak
      digitalWrite(pinPL, HIGH); // Çalmayı Başlat (Basılı tutuyor gibi)
      
      delay(5000);               // 5 Saniye boyunca çal
      
      digitalWrite(pinPL, LOW);  // Çalmayı Durdur
      ledleriSondur();
    }
  }
}

// Yardımcı Fonksiyon: LED Renklerini Ayarlar
void ledYak(int k, int y, int m) {
  // Ortak Katot (-) Modül için:
  digitalWrite(ledKirmizi, k);
  digitalWrite(ledYesil, y);
  digitalWrite(ledMavi, m);
  
  /* NOT: Eğer modülün ORTAK ANOT (+) ise, yani renkler ters yanıyorsa 
     yukarıdaki 3 satırı silip şunları yaz:
     digitalWrite(ledKirmizi, !k);
     digitalWrite(ledYesil, !y);
     digitalWrite(ledMavi, !m);
  */
}

void ledleriSondur() {
  ledYak(LOW, LOW, LOW);
}