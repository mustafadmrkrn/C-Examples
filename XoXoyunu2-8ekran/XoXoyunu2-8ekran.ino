// TIC-TAC-TOE (XOX) OYUNU
// 2.8" TFT LCD Shield icin
// 2 Oyunculu + AI Modu

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
#define PURPLE  0x780F

// Oyun tahtasi
int tahta[3][3]; // 0=bos, 1=X, 2=O
int siradakiOyuncu = 1; // 1=X, 2=O
bool oyunBitti = false;
int kazanan = 0; // 0=devam, 1=X kazandi, 2=O kazandi, 3=berabere

// Skor
int skorX = 0;
int skorO = 0;
int berabere = 0;

// Oyun modu
bool aiModu = false; // false=2 oyuncu, true=AI'ya karsi

// Grid ayarlari
const int GRID_SIZE = 80;
const int GRID_START_X = 20;
const int GRID_START_Y = 50;

void setup() {
  Serial.begin(9600);
  Serial.println("XOX Oyunu Basliyor...");
  
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  
  randomSeed(analogRead(0));
  
  // Mod secimi ekrani
  modSecimi();
}

void modSecimi() {
  tft.fillScreen(BLACK);
  
  // Baslik
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.setCursor(50, 20);
  tft.println("XOX OYUNU");
  
  // Mod secenekleri
  tft.setTextSize(2);
  
  // 2 Oyuncu butonu
  tft.fillRect(30, 80, 260, 50, BLUE);
  tft.drawRect(30, 80, 260, 50, WHITE);
  tft.setTextColor(WHITE);
  tft.setCursor(50, 95);
  tft.println("2 OYUNCU");
  
  // AI butonu
  tft.fillRect(30, 150, 260, 50, RED);
  tft.drawRect(30, 150, 260, 50, WHITE);
  tft.setCursor(55, 165);
  tft.println("AI'YA KARSI");
  
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(60, 220);
  tft.println("Bir mod seciniz...");
  
  // Mod secimini bekle
  while(true) {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      int touchX = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
      int touchY = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
      
      touchX = constrain(touchX, 0, tft.width());
      touchY = constrain(touchY, 0, tft.height());
      
      // 2 Oyuncu butonu
      if (touchX >= 30 && touchX <= 290 && touchY >= 80 && touchY <= 130) {
        aiModu = false;
        Serial.println("2 Oyuncu modu secildi");
        delay(200);
        yeniOyun();
        return;
      }
      
      // AI butonu
      if (touchX >= 30 && touchX <= 290 && touchY >= 150 && touchY <= 200) {
        aiModu = true;
        Serial.println("AI modu secildi");
        delay(200);
        yeniOyun();
        return;
      }
    }
    delay(50);
  }
}

void yeniOyun() {
  // Tahtayi temizle
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      tahta[i][j] = 0;
    }
  }
  
  siradakiOyuncu = 1;
  oyunBitti = false;
  kazanan = 0;
  
  // Ekrani ciz
  tft.fillScreen(BLACK);
  tahtaCiz();
  durumGoster();
  skorGoster();
  
  Serial.println("Yeni oyun basladi!");
}

void tahtaCiz() {
  // Grid cizgileri
  for(int i = 1; i < 3; i++) {
    // Dikey cizgiler
    tft.drawFastVLine(GRID_START_X + i * GRID_SIZE, GRID_START_Y, GRID_SIZE * 3, WHITE);
    tft.drawFastVLine(GRID_START_X + i * GRID_SIZE + 1, GRID_START_Y, GRID_SIZE * 3, WHITE);
    
    // Yatay cizgiler
    tft.drawFastHLine(GRID_START_X, GRID_START_Y + i * GRID_SIZE, GRID_SIZE * 3, WHITE);
    tft.drawFastHLine(GRID_START_X, GRID_START_Y + i * GRID_SIZE + 1, GRID_SIZE * 3, WHITE);
  }
  
  // Tahta cercevesi
  tft.drawRect(GRID_START_X - 2, GRID_START_Y - 2, GRID_SIZE * 3 + 4, GRID_SIZE * 3 + 4, CYAN);
}

void hucreXCiz(int satir, int sutun) {
  int x = GRID_START_X + sutun * GRID_SIZE + GRID_SIZE / 2;
  int y = GRID_START_Y + satir * GRID_SIZE + GRID_SIZE / 2;
  int boyut = 25;
  
  // X ciz (kalin)
  for(int i = -2; i <= 2; i++) {
    tft.drawLine(x - boyut, y - boyut + i, x + boyut, y + boyut + i, RED);
    tft.drawLine(x + boyut, y - boyut + i, x - boyut, y + boyut + i, RED);
  }
}

void hucreOCiz(int satir, int sutun) {
  int x = GRID_START_X + sutun * GRID_SIZE + GRID_SIZE / 2;
  int y = GRID_START_Y + satir * GRID_SIZE + GRID_SIZE / 2;
  int radius = 25;
  
  // O ciz (kalin)
  for(int i = -2; i <= 2; i++) {
    tft.drawCircle(x, y, radius + i, BLUE);
  }
}

void durumGoster() {
  tft.fillRect(0, 0, tft.width(), 35, BLACK);
  
  if (oyunBitti) {
    tft.setTextSize(2);
    if (kazanan == 3) {
      tft.setTextColor(YELLOW);
      tft.setCursor(70, 10);
      tft.println("BERABERE!");
    } else {
      tft.setTextColor(kazanan == 1 ? RED : BLUE);
      tft.setCursor(50, 10);
      tft.print(kazanan == 1 ? "X" : "O");
      tft.println(" KAZANDI!");
    }
  } else {
    tft.setTextSize(2);
    tft.setTextColor(siradakiOyuncu == 1 ? RED : BLUE);
    tft.setCursor(70, 10);
    tft.print("SIRA: ");
    tft.println(siradakiOyuncu == 1 ? "X" : "O");
  }
}

void skorGoster() {
  int y = GRID_START_Y + GRID_SIZE * 3 + 15;
  
  tft.fillRect(0, y, tft.width(), 40, BLACK);
  
  // Skor tablosu
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, y + 5);
  tft.println("SKORLAR:");
  
  tft.setTextColor(RED);
  tft.setCursor(20, y + 20);
  tft.print("X: ");
  tft.print(skorX);
  
  tft.setTextColor(YELLOW);
  tft.setCursor(120, y + 20);
  tft.print("Berabere: ");
  tft.print(berabere);
  
  tft.setTextColor(BLUE);
  tft.setCursor(240, y + 20);
  tft.print("O: ");
  tft.print(skorO);
}

void loop() {
  if (oyunBitti) {
    // Yeni oyun butonu bekle
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      delay(300);
      yeniOyun();
    }
    delay(100);
    return;
  }
  
  // AI modu ve O'nun sirasi
  if (aiModu && siradakiOyuncu == 2) {
    delay(500); // Dusunme suresi
    aiHamlesi();
    return;
  }
  
  // Dokunma kontrolu
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int touchX = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    int touchY = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
    
    touchX = constrain(touchX, 0, tft.width());
    touchY = constrain(touchY, 0, tft.height());
    
    // Hangi hucreye dokunuldu?
    if (touchX >= GRID_START_X && touchX <= GRID_START_X + GRID_SIZE * 3 &&
        touchY >= GRID_START_Y && touchY <= GRID_START_Y + GRID_SIZE * 3) {
      
      int sutun = (touchX - GRID_START_X) / GRID_SIZE;
      int satir = (touchY - GRID_START_Y) / GRID_SIZE;
      
      // Hucre bos mu?
      if (tahta[satir][sutun] == 0) {
        hamleYap(satir, sutun);
        delay(200);
      }
    }
  }
  
  delay(50);
}

void hamleYap(int satir, int sutun) {
  tahta[satir][sutun] = siradakiOyuncu;
  
  // Hucreyi ciz
  if (siradakiOyuncu == 1) {
    hucreXCiz(satir, sutun);
  } else {
    hucreOCiz(satir, sutun);
  }
  
  Serial.print("Hamle: ");
  Serial.print(siradakiOyuncu == 1 ? "X" : "O");
  Serial.print(" -> (");
  Serial.print(satir);
  Serial.print(",");
  Serial.print(sutun);
  Serial.println(")");
  
  // Kazanan kontrolu
  kazananKontrol();
  
  if (oyunBitti) {
    if (kazanan == 1) skorX++;
    else if (kazanan == 2) skorO++;
    else if (kazanan == 3) berabere++;
    
    durumGoster();
    skorGoster();
    
    // Yeni oyun mesaji
    delay(1000);
    tft.fillRect(40, 220, 240, 20, BLACK);
    tft.setTextSize(1);
    tft.setTextColor(GREEN);
    tft.setCursor(50, 222);
    tft.println("Yeni oyun icin dokun...");
  } else {
    // Sirayi degistir
    siradakiOyuncu = (siradakiOyuncu == 1) ? 2 : 1;
    durumGoster();
  }
}

void kazananKontrol() {
  // Satirlari kontrol et
  for(int i = 0; i < 3; i++) {
    if (tahta[i][0] != 0 && tahta[i][0] == tahta[i][1] && tahta[i][1] == tahta[i][2]) {
      kazanan = tahta[i][0];
      oyunBitti = true;
      kazananCizgisi(i, 0, i, 2);
      return;
    }
  }
  
  // Sutunlari kontrol et
  for(int i = 0; i < 3; i++) {
    if (tahta[0][i] != 0 && tahta[0][i] == tahta[1][i] && tahta[1][i] == tahta[2][i]) {
      kazanan = tahta[0][i];
      oyunBitti = true;
      kazananCizgisi(0, i, 2, i);
      return;
    }
  }
  
  // Caprazlari kontrol et
  if (tahta[0][0] != 0 && tahta[0][0] == tahta[1][1] && tahta[1][1] == tahta[2][2]) {
    kazanan = tahta[0][0];
    oyunBitti = true;
    kazananCizgisi(0, 0, 2, 2);
    return;
  }
  
  if (tahta[0][2] != 0 && tahta[0][2] == tahta[1][1] && tahta[1][1] == tahta[2][0]) {
    kazanan = tahta[0][2];
    oyunBitti = true;
    kazananCizgisi(0, 2, 2, 0);
    return;
  }
  
  // Berabere mi?
  bool dolu = true;
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      if (tahta[i][j] == 0) {
        dolu = false;
        break;
      }
    }
  }
  
  if (dolu) {
    kazanan = 3;
    oyunBitti = true;
  }
}

void kazananCizgisi(int s1, int su1, int s2, int su2) {
  int x1 = GRID_START_X + su1 * GRID_SIZE + GRID_SIZE / 2;
  int y1 = GRID_START_Y + s1 * GRID_SIZE + GRID_SIZE / 2;
  int x2 = GRID_START_X + su2 * GRID_SIZE + GRID_SIZE / 2;
  int y2 = GRID_START_Y + s2 * GRID_SIZE + GRID_SIZE / 2;
  
  // Kalin cizgi
  for(int i = -3; i <= 3; i++) {
    tft.drawLine(x1 + i, y1, x2 + i, y2, GREEN);
    tft.drawLine(x1, y1 + i, x2, y2 + i, GREEN);
  }
}

void aiHamlesi() {
  // Basit AI: Kazanma veya engelleme hamlesi ara
  
  // Once kazanma hamlesi var mi bak
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      if (tahta[i][j] == 0) {
        tahta[i][j] = 2; // AI'nin hamlesi
        kazananKontrol();
        if (kazanan == 2) {
          // Kazanma hamlesi bulundu!
          tahta[i][j] = 0; // Geri al
          kazanan = 0;
          oyunBitti = false;
          hamleYap(i, j);
          return;
        }
        tahta[i][j] = 0; // Geri al
        kazanan = 0;
        oyunBitti = false;
      }
    }
  }
  
  // Engelleme hamlesi var mi bak
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      if (tahta[i][j] == 0) {
        tahta[i][j] = 1; // Rakibin hamlesi
        kazananKontrol();
        if (kazanan == 1) {
          // Engelleme hamlesi bulundu!
          tahta[i][j] = 0;
          kazanan = 0;
          oyunBitti = false;
          hamleYap(i, j);
          return;
        }
        tahta[i][j] = 0;
        kazanan = 0;
        oyunBitti = false;
      }
    }
  }
  
  // Merkez bossa al
  if (tahta[1][1] == 0) {
    hamleYap(1, 1);
    return;
  }
  
  // Koseler oncelikli
  int koseler[4][2] = {{0,0}, {0,2}, {2,0}, {2,2}};
  for(int i = 0; i < 4; i++) {
    int s = koseler[i][0];
    int su = koseler[i][1];
    if (tahta[s][su] == 0) {
      hamleYap(s, su);
      return;
    }
  }
  
  // Rastgele bos hucre
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      if (tahta[i][j] == 0) {
        hamleYap(i, j);
        return;
      }
    }
  }
}