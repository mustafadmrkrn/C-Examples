// FLAPPY BIRD OYUNU
// 2.8" TFT LCD Shield icin
// Ekrana dokunarak kusu ucur!

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
#define BROWN   0x8200
#define SKY     0x4D9F

// Oyun degiskenleri
int kusY = 120;
float kusHiz = 0;
const float GRAVITY = 0.6;
const float JUMP = -8;
const int KUS_X = 50;
const int KUS_RADIUS = 8;

// Boru degiskenleri
#define MAX_BORULAR 3
struct Boru {
  int x;
  int gapY;  // Bosluk merkezi
  int gapSize;
  bool aktif;
  bool gecildi;
};

Boru borular[MAX_BORULAR];
const int BORU_WIDTH = 40;
const int BORU_SPEED = 3;
unsigned long sonBoruZamani = 0;
const int BORU_ARALIK = 1500;

int skor = 0;
int highScore = 0;
bool oyunDevam = false;
bool oyunBasladi = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Flappy Bird Basliyor...");
  
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  
  randomSeed(analogRead(0));
  
  // Baslangic ekrani
  baslatEkrani();
}

void baslatEkrani() {
  tft.fillScreen(SKY);
  
  // Baslik
  tft.setTextSize(4);
  tft.setTextColor(YELLOW);
  tft.setCursor(20, 40);
  tft.println("FLAPPY");
  tft.setCursor(40, 80);
  tft.println("BIRD");
  
  // Talimat
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(40, 140);
  tft.println("Ekrana dokunarak");
  tft.setCursor(60, 155);
  tft.println("kusu ucurun!");
  
  tft.setCursor(50, 180);
  tft.println("Borulara carpmayin!");
  
  // Baslamak icin
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(30, 210);
  tft.println("DOKUN BASLA");
  
  // Animasyonlu kus
  for(int i = 0; i < 5; i++) {
    kusCiz(280, 120 + (i % 2) * 5, YELLOW);
    delay(200);
    kusCiz(280, 120 + (i % 2) * 5, SKY);
  }
  kusCiz(280, 120, YELLOW);
}

void oyunBaslat() {
  tft.fillScreen(SKY);
  
  // Yeri ciz
  tft.fillRect(0, tft.height() - 20, tft.width(), 20, GREEN);
  tft.fillRect(0, tft.height() - 22, tft.width(), 2, BROWN);
  
  // Degiskenleri sifirla
  kusY = 120;
  kusHiz = 0;
  skor = 0;
  oyunDevam = true;
  oyunBasladi = true;
  
  // Borulari sifirla
  for(int i = 0; i < MAX_BORULAR; i++) {
    borular[i].aktif = false;
  }
  
  sonBoruZamani = millis();
  skorGoster();
  
  Serial.println("Oyun basladi!");
}

void loop() {
  if (!oyunBasladi) {
    // Baslangic ekraninda - dokunmayi bekle
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      delay(200);
      oyunBaslat();
    }
    delay(50);
    return;
  }
  
  if (!oyunDevam) {
    oyunBitti();
    return;
  }
  
  // Eski kusu sil
  kusCiz(KUS_X, kusY, SKY);
  
  // Dokunma kontrolu
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    kusHiz = JUMP;
    Serial.println("ZIP!");
  }
  
  // Fizik
  kusHiz += GRAVITY;
  kusY += kusHiz;
  
  // Sinir kontrolleri
  if (kusY < KUS_RADIUS) {
    kusY = KUS_RADIUS;
    kusHiz = 0;
  }
  
  if (kusY > tft.height() - 20 - KUS_RADIUS) {
    oyunDevam = false;
    Serial.println("Yere carpti!");
  }
  
  // Yeni kusu ciz
  kusCiz(KUS_X, kusY, YELLOW);
  
  // Boru olustur
  if (millis() - sonBoruZamani > BORU_ARALIK) {
    yeniBoruOlustur();
    sonBoruZamani = millis();
  }
  
  // Borulari hareket ettir
  for(int i = 0; i < MAX_BORULAR; i++) {
    if (borular[i].aktif) {
      // Eski boru sil
      boruCiz(i, SKY);
      
      // Hareket
      borular[i].x -= BORU_SPEED;
      
      // Ekrandan cikti mi?
      if (borular[i].x < -BORU_WIDTH) {
        borular[i].aktif = false;
      } else {
        // Yeni boru ciz
        boruCiz(i, GREEN);
        
        // Carpma kontrolu
        if (boruCarptiMi(i)) {
          oyunDevam = false;
          Serial.println("Boruya carpti!");
        }
        
        // Skor kontrolu
        if (!borular[i].gecildi && borular[i].x + BORU_WIDTH < KUS_X) {
          borular[i].gecildi = true;
          skor++;
          skorGoster();
          Serial.print("Skor: ");
          Serial.println(skor);
        }
      }
    }
  }
  
  delay(20);
}

void yeniBoruOlustur() {
  for(int i = 0; i < MAX_BORULAR; i++) {
    if (!borular[i].aktif) {
      borular[i].x = tft.width();
      borular[i].gapSize = 70;
      borular[i].gapY = random(60, tft.height() - 80);
      borular[i].aktif = true;
      borular[i].gecildi = false;
      break;
    }
  }
}

void boruCiz(int index, uint16_t color) {
  int x = borular[index].x;
  int gapY = borular[index].gapY;
  int gapSize = borular[index].gapSize;
  
  // Ust boru
  int ustBoruYukseklik = gapY - gapSize/2;
  tft.fillRect(x, 0, BORU_WIDTH, ustBoruYukseklik, color);
  
  if (color != SKY) {
    // Boru kenarlari
    tft.drawRect(x, 0, BORU_WIDTH, ustBoruYukseklik, BROWN);
    // Boru agzi
    tft.fillRect(x-3, ustBoruYukseklik-8, BORU_WIDTH+6, 8, color);
    tft.drawRect(x-3, ustBoruYukseklik-8, BORU_WIDTH+6, 8, BROWN);
  }
  
  // Alt boru
  int altBoruY = gapY + gapSize/2;
  int altBoruYukseklik = tft.height() - 20 - altBoruY;
  tft.fillRect(x, altBoruY, BORU_WIDTH, altBoruYukseklik, color);
  
  if (color != SKY) {
    // Boru kenarlari
    tft.drawRect(x, altBoruY, BORU_WIDTH, altBoruYukseklik, BROWN);
    // Boru agzi
    tft.fillRect(x-3, altBoruY, BORU_WIDTH+6, 8, color);
    tft.drawRect(x-3, altBoruY, BORU_WIDTH+6, 8, BROWN);
  }
}

bool boruCarptiMi(int index) {
  int x = borular[index].x;
  int gapY = borular[index].gapY;
  int gapSize = borular[index].gapSize;
  
  // Kus borunun x alaninda mi?
  if (KUS_X + KUS_RADIUS > x && KUS_X - KUS_RADIUS < x + BORU_WIDTH) {
    // Ust veya alt boruya carpti mi?
    if (kusY - KUS_RADIUS < gapY - gapSize/2 || 
        kusY + KUS_RADIUS > gapY + gapSize/2) {
      return true;
    }
  }
  
  return false;
}

void kusCiz(int x, int y, uint16_t color) {
  // Govde
  tft.fillCircle(x, y, KUS_RADIUS, color);
  
  if (color != SKY) {
    // Goz
    tft.fillCircle(x + 3, y - 2, 2, WHITE);
    tft.fillCircle(x + 3, y - 2, 1, BLACK);
    
    // Gaga
    tft.fillTriangle(
      x + KUS_RADIUS, y,
      x + KUS_RADIUS + 5, y - 2,
      x + KUS_RADIUS + 5, y + 2,
      ORANGE
    );
    
    // Kanat
    tft.fillCircle(x - 2, y + 3, 4, YELLOW);
  }
}

void skorGoster() {
  // Skor paneli
  tft.fillRect(5, 5, 100, 20, BLACK);
  tft.drawRect(5, 5, 100, 20, WHITE);
  
  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.setCursor(15, 8);
  tft.print("SKOR:");
  tft.print(skor);
}

void oyunBitti() {
  // High score guncelle
  if (skor > highScore) {
    highScore = skor;
  }
  
  delay(500);
  
  tft.fillScreen(SKY);
  
  // Oyun bitti
  tft.setTextSize(3);
  tft.setTextColor(RED);
  tft.setCursor(30, 40);
  tft.println("GAME OVER");
  
  // Skor
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.setCursor(50, 90);
  tft.print("SKOR: ");
  tft.println(skor);
  
  tft.setCursor(30, 115);
  tft.print("REKOR: ");
  tft.println(highScore);
  
  // Degerlendirme
  tft.setTextSize(2);
  tft.setCursor(60, 150);
  if (skor >= 20) {
    tft.setTextColor(GREEN);
    tft.println("EFSANE!");
  } else if (skor >= 10) {
    tft.setTextColor(CYAN);
    tft.println("HARIKA!");
  } else if (skor >= 5) {
    tft.setTextColor(YELLOW);
    tft.println("IYI!");
  } else {
    tft.setTextColor(ORANGE);
    tft.println("DENE!");
  }
  
  // Yeniden baslatma
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(40, 190);
  tft.println("Tekrar oynamak icin");
  
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(50, 210);
  tft.println("DOKUN");
  
  Serial.print("Oyun bitti! Skor: ");
  Serial.print(skor);
  Serial.print(" | Rekor: ");
  Serial.println(highScore);
  
  // Yeniden baslamak icin dokun
  while(true) {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      delay(200);
      oyunBasladi = false;
      baslatEkrani();
      return;
    }
    delay(50);
  }
}