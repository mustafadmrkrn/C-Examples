// SATRANC TAKTIK EGITIM SISTEMI (V6 - YUKSEK KONTRAST)
// Tas renkleri ve okunurluk iyilestirildi.
// Beyaz Taslar -> KIRMIZI, Siyah Taslar -> MAVI/SIYAH

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

// --- KALIBRASYON DEGERLERI ---
#define TS_MINX 70
#define TS_MAXX 910
#define TS_MINY 130
#define TS_MAXY 930

// Pinler
#define YP A3
#define XM A2
#define YM 9
#define XP 8

#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// --- RENK PALETI (GUNCEL) ---
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GREY    0x8410
#define ORANGE  0xFD20

// Kare Renkleri (Goz yormayan, kontrastli)
#define KARE_ACIK 0xFFFF // Tam Beyaz
#define KARE_KOYU 0x7BEF // Gri-Mavi (Harflerin okundugu en iyi zemin)

// Tas Renkleri
#define TAS_BEYAZ_RENK RED   // Beyaz taşlar artık KIRMIZI görünecek (Dikkat çeker)
#define TAS_SIYAH_RENK BLACK // Siyah taşlar SİYAH (Ama beyaz gölgeli)

// Olculer
const int KARE_BOYUT = 26;
const int TAHTA_X = 10;
const int TAHTA_Y = 16;

// Global Degiskenler
int8_t tahta[8][8];
int secilenX = -1, secilenY = -1;
bool tasSecildi = false;
byte aktifKategori = 0; 
byte soruNo = 0;
bool soruCozuldu = false;

// --- BULMACA VERILERI (AYNI) ---
const char t1_msg[] PROGMEM = "Tek hamlede MAT";
const int8_t t1_p1[] PROGMEM = { 
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 4,0,0,0,0,0,0,0, -6,0,0,0,0,0,0,4
}; 
const int8_t t1_p2[] PROGMEM = { 
  -4,0,-3,0,-6,0,-2,-4, -1,-1,-1,0,0,-1,-1,-1, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,5,0,0,0, 0,0,0,0,6,0,0,0
};
const int8_t t1_p3[] PROGMEM = { 
  0,0,0,0,-6,0,0,0, 0,0,0,0,0,0,0,0, 4,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,6,0,0,4
};

const char t2_msg[] PROGMEM = "Catal atarak al";
const int8_t t2_p1[] PROGMEM = { 
  0,0,0,-5,0,0,0,-4, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,2,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,6,0,0,0
};
const int8_t t2_p2[] PROGMEM = { 
  0,0,0,-4,0,-3,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0,
  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,6,0,0,0
};
const int8_t t2_p3[] PROGMEM = { 
  0,0,0,0,0,0,0,-4, 0,0,-6,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0, 5,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,6,0,0,0
};

const char t3_msg[] PROGMEM = "Arkayi hedefle";
const int8_t t3_p1[] PROGMEM = { 
  0,0,0,-6,0,0,0,0, 0,0,0,-5,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,4,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,6,0,0,0,0
};
const int8_t t3_p2[] PROGMEM = { 
  0,0,-6,0,0,0,0,0, 0,0,0,-4,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
  0,0,0,0,0,3,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,6,0,0,0
};
const int8_t t3_p3[] PROGMEM = { 
  -6,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,3,0,0,0,0,
  4,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,6,0
};

// Fonksiyonlar
void anaMenuCiz();
void kategoriSec(byte kat);
void soruyuYukle();
void tahtaCiz();
void kareCiz(byte y, byte x, bool secili);
void tasCiz(int x, int y, int8_t tas);
void kareSecildi(byte s, byte c);
bool hamleDogruMu(byte ky, byte kx, byte hy, byte hx);

void setup() {
  Serial.begin(9600);
  tft.begin(tft.readID());
  tft.setRotation(1);
  anaMenuCiz();
}

void loop() {
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT); 
  pinMode(XM, OUTPUT);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    int x = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
    int y = map(p.x, TS_MINY, TS_MAXY, 0, tft.height());

    if(x < 0) x = 0; if(x > tft.width()) x = tft.width();
    if(y < 0) y = 0; if(y > tft.height()) y = tft.height();

    // MENU
    if (aktifKategori == 0) {
      if (x > 20 && x < 300) {
        if (y > 50 && y < 100) kategoriSec(1);
        else if (y > 110 && y < 160) kategoriSec(2);
        else if (y > 170 && y < 220) kategoriSec(3);
      }
    }
    // BULMACA
    else {
      // Menu Butonu
      if (x > 230 && y > 180 && y < 230) {
        aktifKategori = 0;
        anaMenuCiz();
        delay(300);
        return;
      }
      // Sonraki Butonu
      if (soruCozuldu && x > 230 && y > 120 && y < 170) {
         soruNo++;
         if(soruNo > 2) soruNo = 0;
         soruyuYukle();
         delay(300);
         return;
      }

      // TAHTA
      if (!soruCozuldu && x >= TAHTA_X && x < TAHTA_X + 8*KARE_BOYUT &&
          y >= TAHTA_Y && y < TAHTA_Y + 8*KARE_BOYUT) {
         
         byte sutun = (x - TAHTA_X) / KARE_BOYUT;
         byte satir = (y - TAHTA_Y) / KARE_BOYUT;
         
         if(satir < 8 && sutun < 8) {
           kareSecildi(satir, sutun);
           delay(200);
         }
      }
    }
  }
}

void anaMenuCiz() {
  tft.fillScreen(BLACK);
  tft.setTextSize(3); tft.setTextColor(YELLOW);
  tft.setCursor(20, 10); tft.print("TAKTIK EGITIM");
  
  tft.fillRect(20, 50, 280, 50, BLUE);
  tft.drawRect(20, 50, 280, 50, WHITE);
  tft.setCursor(40, 65); tft.setTextColor(WHITE); tft.setTextSize(2); tft.print("1. MAT SORULARI");
  
  tft.fillRect(20, 110, 280, 50, MAGENTA);
  tft.drawRect(20, 110, 280, 50, WHITE);
  tft.setCursor(40, 125); tft.print("2. CATAL TAKTIGI");
  
  tft.fillRect(20, 170, 280, 50, RED);
  tft.drawRect(20, 170, 280, 50, WHITE);
  tft.setCursor(40, 185); tft.print("3. SIS VE ACARAK");
}

void kategoriSec(byte kat) {
  aktifKategori = kat;
  soruNo = 0;
  soruyuYukle();
}

void soruyuYukle() {
  soruCozuldu = false;
  tasSecildi = false;
  secilenX = -1; secilenY = -1;
  
  tft.fillScreen(BLACK);
  
  const int8_t* ptr = NULL;
  const char* msgPtr = NULL;

  if (aktifKategori == 1) { // MAT
    msgPtr = t1_msg;
    if(soruNo==0) ptr=t1_p1; else if(soruNo==1) ptr=t1_p2; else ptr=t1_p3;
  } 
  else if (aktifKategori == 2) { // CATAL
    msgPtr = t2_msg;
    if(soruNo==0) ptr=t2_p1; else if(soruNo==1) ptr=t2_p2; else ptr=t2_p3;
  }
  else { // SIS
    msgPtr = t3_msg;
    if(soruNo==0) ptr=t3_p1; else if(soruNo==1) ptr=t3_p2; else ptr=t3_p3;
  }
  
  for(int i=0; i<64; i++) tahta[i/8][i%8] = pgm_read_byte(ptr + i);
  
  tahtaCiz();
  
  // Baslik (Kelime kaydirma)
  tft.setTextSize(1); tft.setTextColor(YELLOW);
  tft.setCursor(225, 20); 
  char buffer[20];
  strcpy_P(buffer, msgPtr);
  tft.print(aktifKategori == 1 ? "MAT ET" : "KAZAN");

  // Menu Butonu
  tft.fillRect(230, 190, 80, 40, GREY);
  tft.drawRect(230, 190, 80, 40, WHITE);
  tft.setCursor(245, 203); tft.setTextSize(2); tft.setTextColor(WHITE); tft.print("MENU");
}

void tahtaCiz() {
  for(int y=0; y<8; y++) for(int x=0; x<8; x++) kareCiz(y, x, false);
}

void kareCiz(byte s, byte c, bool secili) {
  int x = TAHTA_X + c * KARE_BOYUT;
  int y = TAHTA_Y + s * KARE_BOYUT;
  
  // KARE RENKLERI: ACIK BEYAZ / KOYU GRI
  uint16_t renk = secili ? YELLOW : ((s+c)%2==0 ? KARE_ACIK : KARE_KOYU);
  
  tft.fillRect(x, y, KARE_BOYUT, KARE_BOYUT, renk);
  if(tahta[s][c] != 0) tasCiz(x+(KARE_BOYUT/2), y+(KARE_BOYUT/2), tahta[s][c]);
}

void tasCiz(int x, int y, int8_t tas) {
  tft.setTextSize(2);
  
  bool beyazTas = (tas > 0);
  
  // --- YUKSEK KONTRAST RENK AYARI ---
  // Beyaz Taslar -> KIRMIZI
  // Siyah Taslar -> SIYAH (Beyaz golgeli)
  
  uint16_t anaRenk = beyazTas ? TAS_BEYAZ_RENK : TAS_SIYAH_RENK;
  uint16_t golgeRenk = beyazTas ? WHITE : WHITE; // Her ikisinde de beyaz golge iyidir
  
  // Golge (Kontrast icin)
  tft.setCursor(x-7, y-8); tft.setTextColor(golgeRenk);
  
  char sembol = '?';
  switch(abs(tas)) {
    case 1: sembol='P'; break; case 2: sembol='A'; break;
    case 3: sembol='F'; break; case 4: sembol='K'; break;
    case 5: sembol='V'; break; case 6: sembol='S'; break;
  }
  tft.print(sembol);
  
  // Asil Harf
  tft.setCursor(x-6, y-7);
  tft.setTextColor(anaRenk);
  tft.print(sembol);
}

void kareSecildi(byte s, byte c) {
  int8_t tas = tahta[s][c];
  if (!tasSecildi) {
    if (tas > 0) { 
      secilenY = s; secilenX = c;
      tasSecildi = true;
      kareCiz(s, c, true);
    }
  } else {
    if (s == secilenY && c == secilenX) { 
      kareCiz(s, c, false); tasSecildi = false;
    } else {
      if (hamleDogruMu(secilenY, secilenX, s, c)) {
        tahta[s][c] = tahta[secilenY][secilenX];
        tahta[secilenY][secilenX] = 0;
        tahtaCiz();
        
        tft.fillRect(230, 60, 80, 40, GREEN);
        tft.setCursor(235, 73); tft.setTextSize(2); tft.setTextColor(BLACK); 
        tft.print("DOGRU!");
        
        tft.fillRect(230, 130, 80, 40, BLUE);
        tft.drawRect(230, 130, 80, 40, WHITE);
        tft.setCursor(240, 143); tft.setTextColor(WHITE); tft.print("ILERI");
        
        soruCozuldu = true;
      } else {
        tft.fillRect(230, 60, 80, 40, RED);
        tft.setCursor(235, 73); tft.setTextSize(2); tft.setTextColor(WHITE); 
        tft.print("HATA!");
        delay(500);
        tft.fillRect(230, 60, 80, 40, BLACK);
        kareCiz(secilenY, secilenX, false);
        tasSecildi = false;
      }
    }
  }
}

bool hamleDogruMu(byte ky, byte kx, byte hy, byte hx) {
  if (aktifKategori == 1) { // Mat
    if (soruNo == 0 && ky==6 && kx==7 && hy==0 && hx==7) return true;
    if (soruNo == 1 && ky==6 && kx==4 && hy==0 && hx==4) return true;
    if (soruNo == 2 && ky==7 && kx==7 && hy==0 && hx==7) return true;
  }
  else if (aktifKategori == 2) { // Catal
    if (soruNo == 0 && ky==5 && kx==2 && hy==3 && hx==3) return true;
    if (soruNo == 1 && ky==3 && kx==3 && hy==2 && hx==3) return true;
    if (soruNo == 2 && ky==5 && kx==0 && hy==1 && hx==4) return true;
  }
  else { // Sis
    if (soruNo == 0 && ky==4 && kx==3 && hy==1 && hx==3) return true;
    if (soruNo == 1 && ky==4 && kx==5 && hy==2 && hx==7) return true;
    if (soruNo == 2 && ky==3 && kx==3 && hy==1 && hx==5) return true;
  }
  return false;
}