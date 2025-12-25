// HESAP MAKINESI
// 2.8" TFT LCD Shield icin
// Tam fonksiyonel dokunmatik hesap makinesi

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

// Dokunmatik pinler
#define YP A3
#define XM A2
#define YM 9
#define XP 8

// KALIBRASYON DEGERLERI
#define TS_MINX 88
#define TS_MINY 187
#define TS_MAXX 916
#define TS_MAXY 867

#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Renkler
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define ORANGE  0xFD20
#define GRAY    0x7BEF
#define DARKGRAY 0x39E7

// Ekran ayarlari
const int EKRAN_YUKSEKLIK = 60;
const int BUTON_GENISLIK = 80;
const int BUTON_YUKSEKLIK = 45;

// Hesap makinesi degiskenleri
String mevcutSayi = "0";
double sayi1 = 0;
double sayi2 = 0;
char islem = ' ';
bool yeniSayiGir = true;
bool ondalikVar = false;

// Buton yapisi
struct Buton {
  int x, y, w, h;
  String etiket;
  uint16_t renk;
};

// Butonlar (4x5 grid)
Buton butonlar[20];

void setup() {
  Serial.begin(9600);
  Serial.println("Hesap Makinesi Basliyor...");
  
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  
  // Butonlari tanimla
  butonlariOlustur();
  
  // Ekrani ciz
  ekranCiz();
  
  Serial.println("Hesap makinesi hazir!");
}

void butonlariOlustur() {
  // Satirlar: 0=C/+/-, 1=7-8-9-/, 2=4-5-6-*, 3=1-2-3--, 4=0-.-=--+
  
  String etiketler[20] = {
    "C", "()", "%", "/",
    "7", "8", "9", "*",
    "4", "5", "6", "-",
    "1", "2", "3", "+",
    "0", ".", "=", "AC"
  };
  
  int index = 0;
  for(int satir = 0; satir < 5; satir++) {
    for(int sutun = 0; sutun < 4; sutun++) {
      butonlar[index].x = sutun * BUTON_GENISLIK;
      butonlar[index].y = EKRAN_YUKSEKLIK + satir * BUTON_YUKSEKLIK;
      butonlar[index].w = BUTON_GENISLIK;
      butonlar[index].h = BUTON_YUKSEKLIK;
      butonlar[index].etiket = etiketler[index];
      
      // Renk ata
      if (etiketler[index] == "C" || etiketler[index] == "AC") {
        butonlar[index].renk = RED;
      } else if (etiketler[index] == "=") {
        butonlar[index].renk = GREEN;
      } else if (etiketler[index] == "+" || etiketler[index] == "-" || 
                 etiketler[index] == "*" || etiketler[index] == "/" ||
                 etiketler[index] == "%" || etiketler[index] == "()") {
        butonlar[index].renk = ORANGE;
      } else {
        butonlar[index].renk = DARKGRAY;
      }
      
      index++;
    }
  }
}

void ekranCiz() {
  tft.fillScreen(BLACK);
  
  // Ekran alani
  tft.fillRect(0, 0, 320, EKRAN_YUKSEKLIK, BLACK);
  tft.drawRect(0, 0, 320, EKRAN_YUKSEKLIK, WHITE);
  
  // Butonlari ciz
  for(int i = 0; i < 20; i++) {
    butonCiz(i, false);
  }
  
  // Sayiyi goster
  sayiGoster();
}

void butonCiz(int index, bool basildi) {
  Buton b = butonlar[index];
  
  // Buton arka plani
  uint16_t renk = basildi ? WHITE : b.renk;
  tft.fillRect(b.x + 2, b.y + 2, b.w - 4, b.h - 4, renk);
  
  // Buton kenarligi
  tft.drawRect(b.x + 1, b.y + 1, b.w - 2, b.h - 2, WHITE);
  
  // Buton etiketi
  tft.setTextSize(2);
  tft.setTextColor(basildi ? BLACK : WHITE);
  
  // Metni ortala
  int textX = b.x + (b.w - b.etiket.length() * 12) / 2;
  int textY = b.y + (b.h - 16) / 2;
  
  tft.setCursor(textX, textY);
  tft.print(b.etiket);
}

void sayiGoster() {
  // Ekrani temizle
  tft.fillRect(5, 5, 310, EKRAN_YUKSEKLIK - 10, BLACK);
  
  // Eger islem varsa onu goster
  if (islem != ' ' && !yeniSayiGir) {
    tft.setTextSize(1);
    tft.setTextColor(GRAY);
    tft.setCursor(10, 10);
    tft.print(sayi1, 2);
    tft.print(" ");
    tft.print(islem);
  }
  
  // Mevcut sayiyi goster
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  
  // Metni saga yasla
  int uzunluk = mevcutSayi.length();
  int textX = 310 - (uzunluk * 18);
  if (textX < 10) textX = 10; // Sol kenara yapismamasi icin
  
  tft.setCursor(textX, 28);
  tft.print(mevcutSayi);
}

void loop() {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int touchX = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    int touchY = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
    
    touchX = constrain(touchX, 0, tft.width());
    touchY = constrain(touchY, 0, tft.height());
    
    // Hangi butona basildi?
    for(int i = 0; i < 20; i++) {
      Buton b = butonlar[i];
      if (touchX >= b.x && touchX <= b.x + b.w &&
          touchY >= b.y && touchY <= b.y + b.h) {
        
        // Buton basilma efekti
        butonCiz(i, true);
        delay(100);
        butonCiz(i, false);
        
        // Butonu isle
        butonIsle(b.etiket);
        
        delay(150); // Debounce
        break;
      }
    }
  }
  
  delay(50);
}

void butonIsle(String buton) {
  Serial.print("Buton: ");
  Serial.println(buton);
  
  // Rakam butonlari
  if (buton >= "0" && buton <= "9") {
    if (yeniSayiGir || mevcutSayi == "0") {
      mevcutSayi = buton;
      yeniSayiGir = false;
    } else {
      if (mevcutSayi.length() < 10) { // Max 10 basamak
        mevcutSayi += buton;
      }
    }
  }
  
  // Ondalik nokta
  else if (buton == ".") {
    if (!ondalikVar) {
      if (yeniSayiGir) {
        mevcutSayi = "0.";
        yeniSayiGir = false;
      } else {
        mevcutSayi += ".";
      }
      ondalikVar = true;
    }
  }
  
  // Temizle
  else if (buton == "C") {
    mevcutSayi = "0";
    yeniSayiGir = true;
    ondalikVar = false;
  }
  
  // Tamamen sifirla
  else if (buton == "AC") {
    mevcutSayi = "0";
    sayi1 = 0;
    sayi2 = 0;
    islem = ' ';
    yeniSayiGir = true;
    ondalikVar = false;
  }
  
  // Islem butonlari
  else if (buton == "+" || buton == "-" || buton == "*" || buton == "/" || buton == "%") {
    if (islem != ' ' && !yeniSayiGir) {
      // Onceki islemi tamamla
      hesapla();
    } else {
      sayi1 = mevcutSayi.toDouble();
    }
    
    islem = buton.charAt(0);
    yeniSayiGir = true;
    ondalikVar = false;
    
    Serial.print("Islem: ");
    Serial.print(sayi1);
    Serial.print(" ");
    Serial.println(islem);
  }
  
  // Esittir
  else if (buton == "=") {
    if (islem != ' ') {
      hesapla();
      islem = ' ';
    }
  }
  
  sayiGoster();
}

void hesapla() {
  sayi2 = mevcutSayi.toDouble();
  double sonuc = 0;
  bool hata = false;
  
  Serial.print("Hesaplama: ");
  Serial.print(sayi1);
  Serial.print(" ");
  Serial.print(islem);
  Serial.print(" ");
  Serial.print(sayi2);
  
  switch(islem) {
    case '+':
      sonuc = sayi1 + sayi2;
      break;
    case '-':
      sonuc = sayi1 - sayi2;
      break;
    case '*':
      sonuc = sayi1 * sayi2;
      break;
    case '/':
      if (sayi2 != 0) {
        sonuc = sayi1 / sayi2;
      } else {
        mevcutSayi = "HATA";
        hata = true;
      }
      break;
    case '%':
      sonuc = ((int)sayi1) % ((int)sayi2);
      break;
  }
  
  Serial.print(" = ");
  Serial.println(sonuc);
  
  if (!hata) {
    // Sonucu string'e cevir
    if (sonuc == (int)sonuc && abs(sonuc) < 1000000) {
      // Tam sayi
      mevcutSayi = String((int)sonuc);
    } else {
      // Ondalikli sayi
      mevcutSayi = String(sonuc, 4);
      
      // Gereksiz sifirlari sil
      while (mevcutSayi.indexOf('.') != -1 && 
             (mevcutSayi.endsWith("0") || mevcutSayi.endsWith("."))) {
        mevcutSayi.remove(mevcutSayi.length() - 1);
      }
    }
    
    // Cok uzun sonuclar icin bilimsel gosterim
    if (mevcutSayi.length() > 10) {
      mevcutSayi = String(sonuc, 2);
      if (mevcutSayi.length() > 10) {
        mevcutSayi = "BUYUK";
      }
    }
    
    sayi1 = sonuc;
  }
  
  yeniSayiGir = true;
  ondalikVar = mevcutSayi.indexOf('.') != -1;
}