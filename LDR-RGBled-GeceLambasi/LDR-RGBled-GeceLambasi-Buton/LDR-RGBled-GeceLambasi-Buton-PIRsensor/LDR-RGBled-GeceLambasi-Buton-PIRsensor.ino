/*
  Ertuğrul - Üst Düzey Akıllı Gece Lambası
  Bileşenler: LDR (Karanlık mı?), PIR (Hareket var mı?), Buton (Renk Seçimi)
*/

// --- PİN TANIMLARI ---
#define LDR_PIN 2       // Karanlık sensörü
#define PIR_PIN 3       // Hareket sensörü (YENİ)
#define BUTON_PIN 4     // Renk değiştirme butonu
#define KIRMIZI_PIN 9
#define YESIL_PIN 10
#define MAVI_PIN 11

// --- DEĞİŞKENLER ---
int kacinciMod = 0;     // 0:Beyaz, 1:Kırmızı, 2:Mavi
int butonDurumu;
int sonButonDurumu = 1;
unsigned long sonBasmaZamani = 0;

void setup() {
  pinMode(LDR_PIN, INPUT);
  pinMode(PIR_PIN, INPUT);        // PIR sensörü giriş yapıyoruz
  pinMode(BUTON_PIN, INPUT_PULLUP);

  pinMode(KIRMIZI_PIN, OUTPUT);
  pinMode(YESIL_PIN, OUTPUT);
  pinMode(MAVI_PIN, OUTPUT);
  
  Serial.begin(9600);
  
  // Sensörün ısınması için açılışta 2 sn bekleyelim (PIR gereksinimi)
  delay(2000); 
}

void loop() {
  // --- 1. BÖLÜM: BUTON İLE RENK SEÇİMİ ---
  int okunanButon = digitalRead(BUTON_PIN);

  if (okunanButon == LOW && sonButonDurumu == HIGH && (millis() - sonBasmaZamani > 50)) {
    kacinciMod++;
    if (kacinciMod > 2) kacinciMod = 0; // 3 Mod olsun: Beyaz, Kırmızı, Mavi
    Serial.print("Secilen Renk Modu: ");
    Serial.println(kacinciMod);
    sonBasmaZamani = millis();
  }
  sonButonDurumu = okunanButon;


  // --- 2. BÖLÜM: SENSÖRLERİ OKU ---
  int karanlikVarMi = digitalRead(LDR_PIN);  // 1 (HIGH) ise karanlık
  int hareketVarMi = digitalRead(PIR_PIN);   // 1 (HIGH) ise hareket var

  // Hata ayıklama için ekrana yazdıralım
  // Serial.print("Karanlık: "); Serial.print(karanlikVarMi);
  // Serial.print(" - Hareket: "); Serial.println(hareketVarMi);


  // --- 3. BÖLÜM: KARAR ANI (VE KAPISI) ---
  // Lamba SADECE hem karanlıksa HEM DE hareket varsa yanmalı
  
  if (karanlikVarMi == HIGH && hareketVarMi == HIGH) {
    
    // Hareket algılandı! Seçili rengi yakalım.
    if (kacinciMod == 0) {
      renkAyarla(255, 255, 255); // Beyaz
    }
    else if (kacinciMod == 1) {
      renkAyarla(255, 0, 0);     // Kırmızı
    }
    else if (kacinciMod == 2) {
      renkAyarla(0, 0, 255);     // Mavi
    }
  } 
  else {
    // Ya hava aydınlık, ya da kimse yok. O zaman kapat.
    renkAyarla(0, 0, 0);
  }
  
  delay(100);
}

void renkAyarla(int k, int y, int m) {
  analogWrite(KIRMIZI_PIN, k);
  analogWrite(YESIL_PIN, y);
  analogWrite(MAVI_PIN, m);
}