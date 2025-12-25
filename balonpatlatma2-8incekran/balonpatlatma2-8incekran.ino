// BALON PATLATMA OYUNU - KALIBRASYON DUZELTILMIS
// 2.8" TFT LCD Shield icin

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
#define TS_MINX 88   // Y min (X ve Y degismis)
#define TS_MINY 187  // X min
#define TS_MAXX 916  // Y max
#define TS_MAXY 867  // X max

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
#define PINK    0xF81F

// Oyun degiskenleri
#define MAX_BALONLAR 5
struct Balon {
  int x;
  int y;
  int radius;
  uint16_t color;
  bool aktif;
  int hiz;
};

Balon balonlar[MAX_BALONLAR];
int skor = 0;
int kacirildi = 0;
unsigned long sonBalonZamani = 0;
bool oyunDevam = true;
int seviye = 1;
unsigned long oyunBaslangic = 0;

uint16_t renkler[] = {RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA, ORANGE, PINK};

void setup() {
  Serial.begin(9600);
  Serial.println("Balon Patlatma Oyunu Basliyor...");
  
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  
  randomSeed(analogRead(0));
  
  // Baslangic ekrani
  baslatEkrani();
  
  // Baslangici bekle
  while(true) {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      delay(200);
      break;
    }
    delay(100);
  }
  
  // Oyun baslat
  tft.fillScreen(BLACK);
  skorGoster();
  oyunBaslangic = millis();
  
  for(int i = 0; i < MAX_BALONLAR; i++) {
    balonlar[i].aktif = false;
  }
  
  Serial.println("Oyun basladi!");
}

void baslatEkrani() {
  tft.fillScreen(BLACK);
  
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.setCursor(30, 40);
  tft.println("BALON");
  tft.setCursor(10, 75);
  tft.println("PATLATMA");
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 130);
  tft.println("Yukari cikan balonlara");
  tft.setCursor(20, 145);
  tft.println("dokunarak patlat!");
  
  tft.setTextColor(CYAN);
  tft.setCursor(20, 170);
  tft.println("Her balon: +10 puan");
  tft.setCursor(20, 185);
  tft.println("10 balon kacirirsan biter!");
  
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.setCursor(20, 215);
  tft.println("EKRANA DOKUN");
  
  // Animasyonlu balon
  for(int i = 0; i < 3; i++) {
    tft.fillCircle(280, 200, 15, RED);
    tft.fillCircle(275, 195, 4, WHITE);
    delay(300);
    tft.fillCircle(280, 200, 15, BLACK);
    delay(300);
  }
  tft.fillCircle(280, 200, 15, RED);
  tft.fillCircle(275, 195, 4, WHITE);
}

// DOGRU KOORDINAT DONUSUMU
void getTouch(int &screenX, int &screenY) {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // X ve Y degismis - Y'yi normal yapalim (ters degil)
    screenX = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    screenY = map(p.x, TS_MINY, TS_MAXY, 0, tft.height()); // NORMAL!
    
    screenX = constrain(screenX, 0, tft.width());
    screenY = constrain(screenY, 0, tft.height());
    
    Serial.print("Ham X:");
    Serial.print(p.x);
    Serial.print(" Y:");
    Serial.print(p.y);
    Serial.print(" -> Ekran X:");
    Serial.print(screenX);
    Serial.print(" Y:");
    Serial.println(screenY);
  } else {
    screenX = -1;
    screenY = -1;
  }
}

void loop() {
  if (!oyunDevam) {
    oyunBitti();
    return;
  }
  
  // Seviye artisi
  if (millis() - oyunBaslangic > seviye * 30000) {
    seviye++;
    skorGoster();
  }
  
  // Yeni balon olustur
  unsigned long beklemeZamani = max(800, 1500 - (seviye * 100));
  if (millis() - sonBalonZamani > beklemeZamani) {
    yeniBalonOlustur();
    sonBalonZamani = millis();
  }
  
  // Balonlari hareket ettir
  for(int i = 0; i < MAX_BALONLAR; i++) {
    if (balonlar[i].aktif) {
      balonCiz(i, BLACK);
      balonlar[i].y -= balonlar[i].hiz;
      
      if (balonlar[i].y + balonlar[i].radius < 30) {
        balonlar[i].aktif = false;
        kacirildi++;
        
        tft.fillRect(0, 0, tft.width(), 25, RED);
        delay(100);
        
        if (kacirildi >= 10) {
          oyunDevam = false;
        }
        skorGoster();
      } else {
        balonCiz(i, balonlar[i].color);
      }
    }
  }
  
  // Dokunma kontrolu
  int touchX, touchY;
  getTouch(touchX, touchY);
  
  if (touchX >= 0 && touchY >= 0) {
    bool patlatildi = false;
    
    for(int i = 0; i < MAX_BALONLAR; i++) {
      if (balonlar[i].aktif) {
        int dx = touchX - balonlar[i].x;
        int dy = touchY - balonlar[i].y;
        int mesafe = sqrt(dx*dx + dy*dy);
        
        if (mesafe < balonlar[i].radius + 8) {
          Serial.println("BALON PATLADI!");
          patlama(balonlar[i].x, balonlar[i].y);
          balonlar[i].aktif = false;
          skor += 10;
          skorGoster();
          patlatildi = true;
          delay(50);
          break;
        }
      }
    }
    
    // Debug isaretleme
    if (!patlatildi) {
      tft.drawCircle(touchX, touchY, 5, WHITE);
      delay(50);
      tft.drawCircle(touchX, touchY, 5, BLACK);
    }
  }
  
  delay(30);
}

void yeniBalonOlustur() {
  for(int i = 0; i < MAX_BALONLAR; i++) {
    if (!balonlar[i].aktif) {
      balonlar[i].x = random(30, tft.width() - 30);
      balonlar[i].y = tft.height() + 20;
      balonlar[i].radius = random(15, 25);
      balonlar[i].color = renkler[random(0, 8)];
      balonlar[i].aktif = true;
      balonlar[i].hiz = random(2, min(2 + seviye, 7));
      break;
    }
  }
}

void balonCiz(int index, uint16_t color) {
  int x = balonlar[index].x;
  int y = balonlar[index].y;
  int r = balonlar[index].radius;
  
  tft.fillCircle(x, y, r, color);
  
  if (color != BLACK) {
    tft.fillCircle(x - r/3, y - r/3, r/4, WHITE);
    tft.drawLine(x, y + r, x, y + r + 10, color);
  }
}

void patlama(int x, int y) {
  for(int r = 5; r < 35; r += 7) {
    tft.drawCircle(x, y, r, YELLOW);
    tft.drawCircle(x, y, r+2, ORANGE);
  }
  delay(50);
  for(int r = 5; r < 35; r += 7) {
    tft.drawCircle(x, y, r, BLACK);
    tft.drawCircle(x, y, r+2, BLACK);
  }
}

void skorGoster() {
  tft.fillRect(0, 0, tft.width(), 25, BLUE);
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  
  tft.setCursor(5, 8);
  tft.print("SKOR:");
  tft.print(skor);
  
  tft.setCursor(100, 8);
  tft.print("SEVIYE:");
  tft.print(seviye);
  
  tft.setCursor(200, 8);
  tft.print("KACTI:");
  tft.print(kacirildi);
  tft.print("/10");
}

void oyunBitti() {
  tft.fillScreen(BLACK);
  
  tft.setTextSize(3);
  tft.setTextColor(RED);
  tft.setCursor(20, 50);
  tft.println("OYUN BITTI");
  
  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.setCursor(30, 100);
  tft.print("SKOR: ");
  tft.println(skor);
  
  tft.setCursor(30, 125);
  tft.print("SEVIYE: ");
  tft.println(seviye);
  
  tft.setTextSize(2);
  tft.setCursor(40, 160);
  if (skor >= 300) {
    tft.setTextColor(GREEN);
    tft.println("EFSANE!");
  } else if (skor >= 200) {
    tft.setTextColor(CYAN);
    tft.println("MUKEMMEL!");
  } else if (skor >= 100) {
    tft.setTextColor(MAGENTA);
    tft.println("COK IYI!");
  } else {
    tft.setTextColor(ORANGE);
    tft.println("IDMAN ET!");
  }
  
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(30, 200);
  tft.println("Yeniden oynamak icin");
  tft.setCursor(50, 215);
  tft.println("Arduino'yu resetleyin");
  
  Serial.print("Oyun bitti! Final Skoru: ");
  Serial.println(skor);
  
  while(1);
}