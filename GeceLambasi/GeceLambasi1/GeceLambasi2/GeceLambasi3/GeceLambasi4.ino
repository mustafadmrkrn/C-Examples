// --- PIN TANIMLAMALARI ---
const int PIN_LDR        = A0;  // Işık Sensörü
const int PIN_POT        = A1;  // Potansiyometre (Parlaklık)

const int PIN_BUTON_RENK = 2;   // 1. Buton: Renk değiştirir
const int PIN_BUTON_GUC  = 3;   // 2. Buton: Açma/Kapama 

// RGB LED Pinleri
const int PIN_KIRMIZI    = 9;
const int PIN_YESIL      = 10;
const int PIN_MAVI       = 11;

// --- AYARLAR ---
int ESIK_DEGER = 600; // Karanlık eşiği (Senin sensöründe 600'ü geçince karanlık)

// --- DEĞİŞKENLER ---
int renkModu = 0;     
bool oncekiKaranlik = false; // Havanın yeni karardığını anlamak için

bool lambaAcik = false;      // Lambanın yanıp yanmayacağını belirler

// Butonların çift basmasını (ark) önlemek için
unsigned long sonRenkButonZamani = 0;
unsigned long sonGucButonZamani = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_KIRMIZI, OUTPUT);
  pinMode(PIN_YESIL, OUTPUT);
  pinMode(PIN_MAVI, OUTPUT);
  
  pinMode(PIN_BUTON_RENK, INPUT_PULLUP);
  pinMode(PIN_BUTON_GUC, INPUT_PULLUP);

  Serial.println("=============================================");
  Serial.println(" TAM KONTROLLU GECE LAMBASI (Zamanlayici YOK)");
  Serial.println("=============================================");
}

void loop() {
  // 1. Sensörleri Oku
  int isikSeviyesi = analogRead(PIN_LDR);
  int potDegeri = analogRead(PIN_POT);
  bool suAnKaranlik = (isikSeviyesi > ESIK_DEGER);
  
  // --- BUTON 1 (RENK DEĞİŞTİRME) ---
  if (digitalRead(PIN_BUTON_RENK) == LOW) {
    if (millis() - sonRenkButonZamani > 300) { 
      renkModu++; 
      if (renkModu > 4) renkModu = 0; 
      Serial.print("Renk Degistirdi -> Yeni Mod: "); 
      Serial.println(renkModu);
      sonRenkButonZamani = millis();
    }
  }

  // --- BUTON 2 (AÇMA / KAPAMA) ---
  if (digitalRead(PIN_BUTON_GUC) == LOW) {
    if (millis() - sonGucButonZamani > 300) { 
      lambaAcik = !lambaAcik; // Açiksa kapat, kapaliysa aç
      
      Serial.print("-> 2. Butona Basildi! Lamba su an: "); 
      Serial.println(lambaAcik ? "MANUEL ACILDI" : "MANUEL KAPATILDI");
      
      sonGucButonZamani = millis();
    }
  }

  // --- OTOMATİK GECE / GÜNDÜZ ALGILAMASI ---
  // Eğer hava ŞU AN karanlıksa VE bir önceki ölçümde aydınlıksa (Yani hava DAHA YENİ karardıysa)
  if (suAnKaranlik && oncekiKaranlik == false) {
    oncekiKaranlik = true;
    lambaAcik = true; // Hava kararınca lambayı otomatik aç
    Serial.println("\n-> HAVA KARARDI! Lamba otomatik olarak acildi.");
  } 
  // Eğer hava ŞU AN aydınlıksa VE bir önceki ölçümde karanlıksa (Yani SABAHA KARŞI aydınlandıysa)
  else if (!suAnKaranlik && oncekiKaranlik == true) {
    oncekiKaranlik = false;
    lambaAcik = false; // Sabah olunca lambayı kesinlikle kapat
    Serial.println("\n-> SABAH OLDU! Lamba otomatik olarak kapatildi.");
  }

  // --- IŞIKLARI YAK VEYA SÖNDÜR MEKANİZMASI ---
  if (lambaAcik) {
    // Lamba açıksa, seçili rengi potansiyometre parlaklığı ile yak
    if (renkModu == 0)      renkYak(255, 255, 255, potDegeri); // Beyaz
    else if (renkModu == 1) renkYak(255, 0, 0, potDegeri);     // Kırmızı
    else if (renkModu == 2) renkYak(0, 0, 255, potDegeri);     // Mavi
    else if (renkModu == 3) renkYak(0, 255, 0, potDegeri);     // Yeşil
    else if (renkModu == 4) renkYak(255, 0, 180, potDegeri);   // Pembe
  } else {
    // Lamba kapalı durumdaysa tamamen söndür
    isiklariSondur();
  }
}

// --- FONKSİYONLAR ---

void isiklariSondur() {
  analogWrite(PIN_KIRMIZI, 0);
  analogWrite(PIN_YESIL, 0);
  analogWrite(PIN_MAVI, 0);
}

void renkYak(int k, int y, int m, int pot) {
  // Potansiyometre tamamen kısıksa gücü sıfırla
  if(pot < 15) {
    isiklariSondur();
    return;
  }
  
  // map() ile potansiyometre değerini(0-1023) renk gücüne(0-255) oranla
  int parlakK = map(pot, 0, 1023, 0, k);
  int parlakY = map(pot, 0, 1023, 0, y);
  int parlakM = map(pot, 0, 1023, 0, m);

  analogWrite(PIN_KIRMIZI, parlakK);
  analogWrite(PIN_YESIL, parlakY);
  analogWrite(PIN_MAVI, parlakM);
}