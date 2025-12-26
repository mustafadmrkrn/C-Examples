// SATRANC OYUNU - AI MODLU
// 2.8" TFT LCD Shield icin
// 2 Oyunculu + AI'ya Karsi

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
#define WHITE   0xFFFF
#define BROWN   0x8200
#define BEIGE   0xF7BE
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define PURPLE  0x780F

// Tahta ayarlari
const int KARE_BOYUT = 28;
const int TAHTA_X = 8;
const int TAHTA_Y = 8;

// Tas kodlari
int tahta[8][8] = {
  {-4, -2, -3, -5, -6, -3, -2, -4},
  {-1, -1, -1, -1, -1, -1, -1, -1},
  { 0,  0,  0,  0,  0,  0,  0,  0},
  { 0,  0,  0,  0,  0,  0,  0,  0},
  { 0,  0,  0,  0,  0,  0,  0,  0},
  { 0,  0,  0,  0,  0,  0,  0,  0},
  { 1,  1,  1,  1,  1,  1,  1,  1},
  { 4,  2,  3,  5,  6,  3,  2,  4}
};

// Oyun degiskenleri
int siradakiOyuncu = 1; // 1=Beyaz, -1=Siyah
int secilenX = -1, secilenY = -1;
bool tasSecildi = false;
bool oyunBitti = false;
int kazanan = 0;
int hamleSayisi = 0;

// AI modu
bool aiModu = false;
bool aiDusunuyor = false;
bool oyunBaslamadi = true;

struct Hamle {
  int kaynakY, kaynakX;
  int hedefY, hedefX;
  int skor;
};

void setup() {
  Serial.begin(9600);
  Serial.println("Satranc Oyunu Basliyor...");
  
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  
  randomSeed(analogRead(0));
  
  modSecimi();
}

void modSecimi() {
  tft.fillScreen(BLACK);
  
  // Baslik
  tft.setTextSize(3);
  tft.setTextColor(YELLOW);
  tft.setCursor(40, 20);
  tft.println("SATRANC");
  
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
  tft.setCursor(80, 220);
  tft.println("Bir mod seciniz");
  
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
      
      // 2 Oyuncu
      if (touchX >= 30 && touchX <= 290 && touchY >= 80 && touchY <= 130) {
        aiModu = false;
        Serial.println("2 Oyuncu modu");
        delay(200);
        oyunBaslamadi = false;
        ekranCiz();
        return;
      }
      
      // AI
      if (touchX >= 30 && touchX <= 290 && touchY >= 150 && touchY <= 200) {
        aiModu = true;
        Serial.println("AI modu");
        delay(200);
        oyunBaslamadi = false;
        ekranCiz();
        return;
      }
    }
    delay(50);
  }
}

void ekranCiz() {
  tft.fillScreen(BLACK);
  
  // Baslik
  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.setCursor(240, 10);
  tft.println("SATRANC");
  
  if (aiModu) {
    tft.setTextSize(1);
    tft.setTextColor(PURPLE);
    tft.setCursor(245, 30);
    tft.println("AI MODU");
  }
  
  tahtaCiz();
  durumGoster();
  kontrolPaneliCiz();
}

void tahtaCiz() {
  for(int satir = 0; satir < 8; satir++) {
    for(int sutun = 0; sutun < 8; sutun++) {
      kareCiz(satir, sutun, false);
    }
  }
}

void kareCiz(int satir, int sutun, bool secili) {
  int x = TAHTA_X + sutun * KARE_BOYUT;
  int y = TAHTA_Y + satir * KARE_BOYUT;
  
  uint16_t kareRenk;
  if (secili) {
    kareRenk = YELLOW;
  } else {
    kareRenk = ((satir + sutun) % 2 == 0) ? BEIGE : BROWN;
  }
  
  tft.fillRect(x, y, KARE_BOYUT, KARE_BOYUT, kareRenk);
  
  int tas = tahta[satir][sutun];
  if (tas != 0) {
    tasCiz(x + KARE_BOYUT/2, y + KARE_BOYUT/2, tas);
  }
}

void tasCiz(int x, int y, int tas) {
  bool beyaz = tas > 0;
  int tasTip = abs(tas);
  
  uint16_t renk = beyaz ? WHITE : BLACK;
  uint16_t cerceve = beyaz ? BLACK : WHITE;
  
  tft.setTextSize(2);
  
  String sembol = "";
  switch(tasTip) {
    case 1: sembol = "P"; break;
    case 2: sembol = "A"; break;
    case 3: sembol = "F"; break;
    case 4: sembol = "K"; break;
    case 5: sembol = "V"; break;
    case 6: sembol = "S"; break;
  }
  
  tft.setCursor(x - 7, y - 8);
  tft.setTextColor(cerceve);
  tft.print(sembol);
  
  tft.setCursor(x - 6, y - 7);
  tft.setTextColor(renk);
  tft.print(sembol);
}

void durumGoster() {
  tft.fillRect(240, 40, 80, 80, BLACK);
  
  if (aiDusunuyor) {
    tft.setTextSize(1);
    tft.setTextColor(PURPLE);
    tft.setCursor(245, 55);
    tft.println("AI");
    tft.setCursor(245, 70);
    tft.println("DUSUNUYOR");
    tft.setCursor(255, 85);
    tft.println("...");
  } else if (oyunBitti) {
    tft.setTextSize(1);
    tft.setTextColor(RED);
    tft.setCursor(245, 45);
    tft.println("OYUN");
    tft.setCursor(245, 60);
    tft.println("BITTI!");
    tft.setCursor(245, 80);
    if (kazanan == 1) {
      tft.println("BEYAZ");
    } else {
      tft.println("SIYAH");
    }
    tft.setCursor(245, 95);
    tft.println("KAZANDI");
  } else {
    tft.setTextSize(1);
    tft.setTextColor(siradakiOyuncu == 1 ? WHITE : BLACK);
    tft.setCursor(245, 50);
    tft.println("SIRA:");
    tft.setCursor(245, 65);
    if (aiModu && siradakiOyuncu == -1) {
      tft.println("AI");
    } else {
      tft.println(siradakiOyuncu == 1 ? "BEYAZ" : "SIYAH");
    }
  }
  
  tft.setCursor(245, 100);
  tft.setTextColor(CYAN);
  tft.setTextSize(1);
  tft.print("Hamle:");
  tft.setCursor(245, 112);
  tft.println(hamleSayisi);
}

void kontrolPaneliCiz() {
  tft.fillRect(240, 140, 75, 35, GREEN);
  tft.drawRect(240, 140, 75, 35, WHITE);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(248, 153);
  tft.println("YENI OYUN");
  
  tft.fillRect(240, 185, 75, 35, BLUE);
  tft.drawRect(240, 185, 75, 35, WHITE);
  tft.setTextColor(WHITE);
  tft.setCursor(253, 198);
  tft.println("IPTAL");
}

void loop() {
  if (oyunBaslamadi) return;
  
  if (oyunBitti) {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      int touchX = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
      int touchY = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
      
      touchX = constrain(touchX, 0, tft.width());
      touchY = constrain(touchY, 0, tft.height());
      
      if (touchX >= 240 && touchX <= 315 && touchY >= 140 && touchY <= 175) {
        delay(200);
        yeniOyun();
      }
    }
    delay(50);
    return;
  }
  
  // AI'nin sirasi
  if (aiModu && siradakiOyuncu == -1 && !aiDusunuyor) {
    aiDusunuyor = true;
    durumGoster();
    delay(500); // Dusunme animasyonu
    aiHamlesi();
    aiDusunuyor = false;
    return;
  }
  
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int touchX = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    int touchY = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());
    
    touchX = constrain(touchX, 0, tft.width());
    touchY = constrain(touchY, 0, tft.height());
    
    if (touchX >= 240 && touchX <= 315 && touchY >= 140 && touchY <= 175) {
      delay(200);
      yeniOyun();
      return;
    }
    
    if (touchX >= 240 && touchX <= 315 && touchY >= 185 && touchY <= 220) {
      if (tasSecildi) {
        kareCiz(secilenY, secilenX, false);
        tasSecildi = false;
        secilenX = -1;
        secilenY = -1;
        delay(200);
      }
      return;
    }
    
    if (touchX >= TAHTA_X && touchX <= TAHTA_X + 8 * KARE_BOYUT &&
        touchY >= TAHTA_Y && touchY <= TAHTA_Y + 8 * KARE_BOYUT) {
      
      int sutun = (touchX - TAHTA_X) / KARE_BOYUT;
      int satir = (touchY - TAHTA_Y) / KARE_BOYUT;
      
      if (sutun >= 0 && sutun < 8 && satir >= 0 && satir < 8) {
        kareSecildi(satir, sutun);
        delay(200);
      }
    }
  }
  
  delay(50);
}

void kareSecildi(int satir, int sutun) {
  int tas = tahta[satir][sutun];
  
  if (!tasSecildi) {
    if (tas != 0 && ((siradakiOyuncu == 1 && tas > 0) || 
                     (siradakiOyuncu == -1 && tas < 0))) {
      secilenX = sutun;
      secilenY = satir;
      tasSecildi = true;
      kareCiz(satir, sutun, true);
    }
  } else {
    if (satir == secilenY && sutun == secilenX) {
      kareCiz(secilenY, secilenX, false);
      tasSecildi = false;
      secilenX = -1;
      secilenY = -1;
    } else {
      if (hamleGecerliMi(secilenY, secilenX, satir, sutun)) {
        hamleYap(secilenY, secilenX, satir, sutun);
      } else {
        kareCiz(secilenY, secilenX, false);
        tasSecildi = false;
        secilenX = -1;
        secilenY = -1;
      }
    }
  }
}

bool hamleGecerliMi(int kaynakY, int kaynakX, int hedefY, int hedefX) {
  int tas = tahta[kaynakY][kaynakX];
  int hedefTas = tahta[hedefY][hedefX];
  
  if (hedefTas != 0 && ((tas > 0 && hedefTas > 0) || (tas < 0 && hedefTas < 0))) {
    return false;
  }
  
  int tasTip = abs(tas);
  int dy = hedefY - kaynakY;
  int dx = hedefX - kaynakX;
  
  switch(tasTip) {
    case 1: // Piyon
      if (tas > 0) {
        if (dx == 0 && dy == -1 && hedefTas == 0) return true;
        if (dx == 0 && dy == -2 && kaynakY == 6 && hedefTas == 0 && tahta[kaynakY-1][kaynakX] == 0) return true;
        if (abs(dx) == 1 && dy == -1 && hedefTas < 0) return true;
      } else {
        if (dx == 0 && dy == 1 && hedefTas == 0) return true;
        if (dx == 0 && dy == 2 && kaynakY == 1 && hedefTas == 0 && tahta[kaynakY+1][kaynakX] == 0) return true;
        if (abs(dx) == 1 && dy == 1 && hedefTas > 0) return true;
      }
      break;
      
    case 2: // At
      if ((abs(dx) == 2 && abs(dy) == 1) || (abs(dx) == 1 && abs(dy) == 2)) return true;
      break;
      
    case 3: // Fil
      if (abs(dx) == abs(dy)) {
        return yolAcikMi(kaynakY, kaynakX, hedefY, hedefX);
      }
      break;
      
    case 4: // Kale
      if (dx == 0 || dy == 0) {
        return yolAcikMi(kaynakY, kaynakX, hedefY, hedefX);
      }
      break;
      
    case 5: // Vezir
      if (dx == 0 || dy == 0 || abs(dx) == abs(dy)) {
        return yolAcikMi(kaynakY, kaynakX, hedefY, hedefX);
      }
      break;
      
    case 6: // Sah
      if (abs(dx) <= 1 && abs(dy) <= 1) return true;
      break;
  }
  
  return false;
}

bool yolAcikMi(int y1, int x1, int y2, int x2) {
  int dx = (x2 > x1) ? 1 : ((x2 < x1) ? -1 : 0);
  int dy = (y2 > y1) ? 1 : ((y2 < y1) ? -1 : 0);
  
  int x = x1 + dx;
  int y = y1 + dy;
  
  while (x != x2 || y != y2) {
    if (tahta[y][x] != 0) return false;
    x += dx;
    y += dy;
  }
  
  return true;
}

void hamleYap(int kaynakY, int kaynakX, int hedefY, int hedefX) {
  int tas = tahta[kaynakY][kaynakX];
  int yenilenTas = tahta[hedefY][hedefX];
  
  if (abs(yenilenTas) == 6) {
    oyunBitti = true;
    kazanan = siradakiOyuncu;
  }
  
  tahta[hedefY][hedefX] = tas;
  tahta[kaynakY][kaynakX] = 0;
  
  kareCiz(kaynakY, kaynakX, false);
  kareCiz(hedefY, hedefX, false);
  
  tasSecildi = false;
  secilenX = -1;
  secilenY = -1;
  
  siradakiOyuncu = -siradakiOyuncu;
  hamleSayisi++;
  
  durumGoster();
}

void aiHamlesi() {
  Serial.println("AI dusunuyor...");
  
  Hamle enIyiHamle;
  enIyiHamle.skor = -9999;
  
  // Tum mumkun hamleleri degerlendir
  for(int y = 0; y < 8; y++) {
    for(int x = 0; x < 8; x++) {
      int tas = tahta[y][x];
      if (tas < 0) { // AI'nin tasi
        
        // Bu tas icin tum hedefleri dene
        for(int hy = 0; hy < 8; hy++) {
          for(int hx = 0; hx < 8; hx++) {
            if (hamleGecerliMi(y, x, hy, hx)) {
              
              // Hamleyi simule et
              int yenilenTas = tahta[hy][hx];
              tahta[hy][hx] = tas;
              tahta[y][x] = 0;
              
              // Hamleyi skorla
              int skor = hamleSkorla(hy, hx, yenilenTas);
              
              // Hamleyi geri al
              tahta[y][x] = tas;
              tahta[hy][hx] = yenilenTas;
              
              // En iyi hamle mi?
              if (skor > enIyiHamle.skor) {
                enIyiHamle.kaynakY = y;
                enIyiHamle.kaynakX = x;
                enIyiHamle.hedefY = hy;
                enIyiHamle.hedefX = hx;
                enIyiHamle.skor = skor;
              }
            }
          }
        }
      }
    }
  }
  
  // En iyi hamleyi yap
  if (enIyiHamle.skor > -9999) {
    Serial.print("AI hamlesi: (");
    Serial.print(enIyiHamle.kaynakY);
    Serial.print(",");
    Serial.print(enIyiHamle.kaynakX);
    Serial.print(") -> (");
    Serial.print(enIyiHamle.hedefY);
    Serial.print(",");
    Serial.print(enIyiHamle.hedefX);
    Serial.print(") Skor:");
    Serial.println(enIyiHamle.skor);
    
    hamleYap(enIyiHamle.kaynakY, enIyiHamle.kaynakX, 
             enIyiHamle.hedefY, enIyiHamle.hedefX);
  }
}

int hamleSkorla(int hedefY, int hedefX, int yenilenTas) {
  int skor = 0;
  
  // Yenilen tas skoru
  switch(abs(yenilenTas)) {
    case 1: skor += 10; break;  // Piyon
    case 2: skor += 30; break;  // At
    case 3: skor += 30; break;  // Fil
    case 4: skor += 50; break;  // Kale
    case 5: skor += 90; break;  // Vezir
    case 6: skor += 900; break; // Sah
  }
  
  // Pozisyon skoru (merkeze yakin olma)
  int merkezeUzaklik = abs(hedefY - 3.5) + abs(hedefX - 3.5);
  skor += (7 - merkezeUzaklik) * 2;
  
  // Ileri gitme bonusu
  skor += (7 - hedefY) * 3;
  
  // Rastgelelik ekle (ayni skorlarda farklilik icin)
  skor += random(-5, 5);
  
  return skor;
}

void yeniOyun() {
  int baslangic[8][8] = {
    {-4, -2, -3, -5, -6, -3, -2, -4},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 1,  1,  1,  1,  1,  1,  1,  1},
    { 4,  2,  3,  5,  6,  3,  2,  4}
  };
  
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      tahta[i][j] = baslangic[i][j];
    }
  }
  
  siradakiOyuncu = 1;
  secilenX = -1;
  secilenY = -1;
  tasSecildi = false;
  oyunBitti = false;
  kazanan = 0;
  hamleSayisi = 0;
  aiDusunuyor = false;
  
  ekranCiz();
  
  Serial.println("Yeni oyun!");
}