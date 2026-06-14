#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Ekran adresi Tinkercad için genelde 0x20'dir. Gerçek devrede 0x27 olabilir.
LiquidCrystal_I2C lcd(0x20, 16, 2); 

Servo bayrakServo;

// --- GÜNCEL PIN TANIMLAMALARI ---
const int PIN_BASLAT = 10; // Başlatma Butonu
const int PIN_DURDUR = 11; // Durdurma (Bitiş) Butonu
const int PIN_BUZZER = 8;  
const int PIN_SERVO  = 9;
const int PIN_RGB_R  = 6;
const int PIN_RGB_G  = 5;
const int PIN_RGB_B  = 3;

// --- NOTA FREKANSLARI ---
#define a   440
#define ad  466
#define b   494
#define c   523
#define cd  554
#define d   587
#define dd  622
#define e   659
#define f   698
#define fd  740
#define g   784
#define gd  830
#define a2  880
#define ad2 932
#define b2  988
#define c2  1046
#define cd2 1108
#define d2  1174
#define dd2 1244
#define e2  1318
#define f2  1396
#define fd2 1480
#define g2  1568
#define gd2 1660
#define a3  1760

// --- SENİN MELODİN ---
int notalar[] = {
  c, f, g, gd, e, g, f, 0,
  f, ad2, c2, cd2, a2, c2, ad2,
  c2, ad2, c2, g, 0, g, ad, gd, e, f, g, gd, ad, c2, cd2, dd2, f2, dd2,
  dd, d, dd, c2, ad, gd,
  c, b, c, g, c, c2, ad, gd, g, gd, f,
  f2, dd2, cd2, c2, ad, gd, g, f, c2, c, f,
  
  c, f, g, gd, e, g, f, 0,
  f, ad2, c2, cd2, ad2, a2, c2, ad2,
  c2, ad2, c2, g, 0, g, ad, gd, e, f, g, gd, ad, c2, cd2, dd2, f2, dd2,
  dd, d, dd, c2, ad, gd,
  c, b, c, g, c, c2, ad, gd, g, gd, f,
  f2, dd2, cd2, c2, ad, gd, g, f, c2, c, f, 0
};

// --- SENİN RİTİMLERİN ---
int sureler[] = {
  800, 800, 800, 800, 400, 200, 1600, 300,
  800, 800, 800, 800, 400, 200, 1600,
  200, 200, 200, 400, 100, 400, 200, 400, 200, 400, 200, 400, 200, 400, 200, 400, 200, 400,
  200, 200, 200, 800, 800, 1600,
  200, 200, 200, 800, 800, 800, 200, 200, 400, 200, 800,
  800, 200, 400, 200, 400, 200, 400, 200, 400, 800, 1600,

  800, 800, 800, 800, 400, 200, 1600, 300,
  800, 800, 800, 400, 400, 400, 400, 800,
  200, 200, 200, 400, 100, 400, 200, 400, 200, 400, 200, 400, 200, 400, 200, 400, 200, 400,
  200, 200, 200, 800, 800, 1600,
  200, 200, 200, 800, 800, 800, 200, 200, 400, 200, 800,
  800, 200, 400, 200, 400, 200, 400, 200, 400, 800, 1600, 2000
};

unsigned long oncekiServoZamani = 0;
int servoAci = 90;
int servoYon = 2; 

unsigned long oncekiLedZamani = 0;
bool kirmiziYan = true;

bool torenDevamEdiyor = false; 

void setup() {
  pinMode(PIN_BASLAT, INPUT_PULLUP);
  pinMode(PIN_DURDUR, INPUT_PULLUP);
  
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RGB_R, OUTPUT);
  pinMode(PIN_RGB_G, OUTPUT);
  pinMode(PIN_RGB_B, OUTPUT);

  bayrakServo.attach(PIN_SERVO);
  bayrakServo.write(90); 

  lcd.init();
  lcd.backlight();
  sistemiSifirla(); 
}

void loop() {
  // 1. AŞAMA: Başlat butonuna basıldı mı?
  if (digitalRead(PIN_BASLAT) == LOW && !torenDevamEdiyor) {
    delay(50); // Buton titremesini (elektriksel gürültüyü) süz
    
    if (digitalRead(PIN_BASLAT) == LOW) { // Gerçekten basıldıysa onayla
      torenDevamEdiyor = true;
      toreniBaslat();
      
      // 2. AŞAMA (GÜVENLİK KİLİDİ): Tören bittiğinde veya durdurulduğunda buraya döner
      // Eğer başlatma veya durdurma butonlarına hala basılıyorsa, bırakılana kadar bekle!
      while (digitalRead(PIN_BASLAT) == LOW || digitalRead(PIN_DURDUR) == LOW) {
        delay(10); // Döngü içinde kilitlenip bekle
      }
      
      delay(200); // Parmağını çektikten sonra güvenli bekleme süresi
    }
  }
}

// ==============================================================
// TÖRENİ BAŞLATMA
// ==============================================================
void toreniBaslat() {
  int notaSayisi = sizeof(notalar) / sizeof(notalar[0]);

  for (int i = 0; i < notaSayisi; i++) {
    
    // Her notadan önce 11. Pini (Durdur) kontrol et
    if (digitalRead(PIN_DURDUR) == LOW) {
      break; // Döngüyü kır ve müziği anında kes
    }

    sozleriEkranaYazdir(i);

    if (notalar[i] == 0) {
      noTone(PIN_BUZZER);
    } else {
      tone(PIN_BUZZER, notalar[i]);
    }

    // Akıllı bekleme esnasında durdur butonuna basılırsa (false dönerse) müziği kes
    if (akilliBekleme(sureler[i]) == false) {
      break; 
    }
  }

  // Tören bittiğinde veya İPTAL edildiğinde sistemi sıfırla
  sistemiSifirla();
}

// ==============================================================
// AKILLI BEKLEME (İptal Edilebilir - false dönerse durur)
// ==============================================================
bool akilliBekleme(unsigned long beklemeSuresi) {
  unsigned long baslangic = millis();

  while (millis() - baslangic < beklemeSuresi) {
    
    // Bekleme sırasında saniyenin onda biri kadar bile olsa 11. Pini dinle
    if (digitalRead(PIN_DURDUR) == LOW) {
      return false; // Durdur butonuna basıldı, İPTAL ET!
    }

    // 1. SERVO MOTOR KONTROLÜ
    if (millis() - oncekiServoZamani > 15) { 
      servoAci += servoYon;
      if (servoAci >= 135 || servoAci <= 45) {
        servoYon = -servoYon; 
      }
      bayrakServo.write(servoAci);
      oncekiServoZamani = millis();
    }

    // 2. RGB LED KONTROLÜ
    if (millis() - oncekiLedZamani > 400) { 
      kirmiziYan = !kirmiziYan;
      if (kirmiziYan) {
        analogWrite(PIN_RGB_R, 255); analogWrite(PIN_RGB_G, 0); analogWrite(PIN_RGB_B, 0); 
      } else {
        analogWrite(PIN_RGB_R, 255); analogWrite(PIN_RGB_G, 255); analogWrite(PIN_RGB_B, 255); 
      }
      oncekiLedZamani = millis();
    }
  }
  return true; // Süre sorunsuz doldu
}

// ==============================================================
// SİSTEMİ SIFIRLAMA
// ==============================================================
void sistemiSifirla() {
  torenDevamEdiyor = false; 
  noTone(PIN_BUZZER);
  analogWrite(PIN_RGB_R, 0); analogWrite(PIN_RGB_G, 0); analogWrite(PIN_RGB_B, 0);
  bayrakServo.write(90); 

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Torene Hazir  ");
  lcd.setCursor(0, 1);
  lcd.print("Baslat'a Basiniz");
}

// ==============================================================
// SÖZLERİ EKRANA YAZDIRMA
// ==============================================================
void sozleriEkranaYazdir(int indeks) {
  if (indeks == 0) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Korkma sonmez bu"); 
    lcd.setCursor(0, 1); lcd.print("safaklarda yuzen");
  } 
  else if (indeks == 8) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("al sancak.");
  } 
  else if (indeks == 15) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Sonmeden yurdumu"); 
    lcd.setCursor(0, 1); lcd.print("ustunde tuten...");
  } 
  else if (indeks == 24) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("en son ocak.");
  } 
  else if (indeks == 33) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("O benim"); 
    lcd.setCursor(0, 1); lcd.print("milletimin");
  } 
  else if (indeks == 39) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("yildizidir,"); 
    lcd.setCursor(0, 1); lcd.print("parlayacak.");
  } 
  else if (indeks == 50) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("O benimdir,"); 
    lcd.setCursor(0, 1); lcd.print("o benim...");
  } 
  else if (indeks == 56) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("milletimindir"); 
    lcd.setCursor(0, 1); lcd.print("ancak.");
  } 
  else if (indeks == 61) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Catma kurban"); 
    lcd.setCursor(0, 1); lcd.print("olayim cehreni");
  } 
  else if (indeks == 69) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("ey nazli hilal!");
  } 
  else if (indeks == 76) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Kahraman irkima"); 
    lcd.setCursor(0, 1); lcd.print("bir gul, ne bu");
  } 
  else if (indeks == 85) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("siddet bu celal?");
  } 
  else if (indeks == 94) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Sana olmaz"); 
    lcd.setCursor(0, 1); lcd.print("dokulen...");
  } 
  else if (indeks == 100) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("kanlarimiz"); 
    lcd.setCursor(0, 1); lcd.print("sonra helal...");
  } 
  else if (indeks == 111) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("Hakkidir Hakk'a"); 
    lcd.setCursor(0, 1); lcd.print("tapan milletimin");
  } 
  else if (indeks == 117) {
    lcd.clear(); lcd.setCursor(0, 0); lcd.print("istiklal!");
  }
}