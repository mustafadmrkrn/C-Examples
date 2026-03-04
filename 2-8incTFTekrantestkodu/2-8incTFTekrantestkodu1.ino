// BASIT DOKUNMATIK TEST
// Ham degerleri gormek ve eksen yonlerini anlamak icin

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

MCUFRIEND_kbv tft;

#define YP A3
#define XM A2
#define YM 9
#define XP 8

#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup() {
  Serial.begin(9600);
  Serial.println("=================================");
  Serial.println("BASIT DOKUNMATIK TEST");
  Serial.println("=================================");
  Serial.println("Ekranin her yerine dokunun...\n");
  
  uint16_t ID = tft.readID();
  Serial.print("TFT ID: 0x");
  Serial.println(ID, HEX);
  
  tft.begin(ID);
  tft.setRotation(1);
  
  ekranCiz();
}

void ekranCiz() {
  tft.fillScreen(BLACK);
  
  // Baslik
  tft.setTextSize(2);
  tft.setTextColor(YELLOW);
  tft.setCursor(30, 10);
  tft.println("DOKUNMATIK TEST");
  
  // Talimat
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(20, 40);
  tft.println("Ekrana dokunun...");
  tft.setCursor(20, 55);
  tft.println("Ham degerler gorunecek");
  
  // Cizgi ciz (ekrani 4 bolume ayir)
  tft.drawFastVLine(tft.width()/2, 70, tft.height()-70, GREEN);
  tft.drawFastHLine(0, tft.height()/2, tft.width(), GREEN);
  
  // Kose isaretleri
  tft.setTextSize(1);
  tft.setTextColor(CYAN);
  tft.setCursor(5, 75);
  tft.print("1");
  tft.setCursor(tft.width()-15, 75);
  tft.print("2");
  tft.setCursor(tft.width()-15, tft.height()-15);
  tft.print("3");
  tft.setCursor(5, tft.height()-15);
  tft.print("4");
}

void loop() {
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    
    // Ham degerleri al
    int hamX = p.x;
    int hamY = p.y;
    int basinc = p.z;
    
    // Ekran boyutlari
    int ekranW = tft.width();  // 320
    int ekranH = tft.height(); // 240
    
    // FARKLI MAPPING DENEMELERI
    
    // Deneme 1: Normal
    int pixel1_x = map(hamX, 187, 867, 0, ekranW);
    int pixel1_y = map(hamY, 134, 867, 0, ekranH);
    
    // Deneme 2: X ters
    int pixel2_x = map(hamX, 187, 867, ekranW, 0);
    int pixel2_y = map(hamY, 134, 867, 0, ekranH);
    
    // Deneme 3: Y ters
    int pixel3_x = map(hamX, 187, 867, 0, ekranW);
    int pixel3_y = map(hamY, 134, 867, ekranH, 0);
    
    // Deneme 4: X ve Y degismis
    int pixel4_x = map(hamY, 134, 867, 0, ekranW);
    int pixel4_y = map(hamX, 187, 867, 0, ekranH);
    
    // Deneme 5: X ve Y degismis + Y ters
    int pixel5_x = map(hamY, 134, 867, 0, ekranW);
    int pixel5_y = map(hamX, 187, 867, ekranH, 0);
    
    // Deneme 6: X ve Y degismis + X ters
    int pixel6_x = map(hamY, 134, 867, ekranW, 0);
    int pixel6_y = map(hamX, 187, 867, 0, ekranH);
    
    // Serial'e yaz
    Serial.println("------------------------");
    Serial.print("HAM -> X:");
    Serial.print(hamX);
    Serial.print(" Y:");
    Serial.print(hamY);
    Serial.print(" Z:");
    Serial.println(basinc);
    
    Serial.println("\nFARKLI MAPPING DENEMELERI:");
    Serial.print("1-Normal    : X:");
    Serial.print(pixel1_x);
    Serial.print(" Y:");
    Serial.println(pixel1_y);
    
    Serial.print("2-X Ters    : X:");
    Serial.print(pixel2_x);
    Serial.print(" Y:");
    Serial.println(pixel2_y);
    
    Serial.print("3-Y Ters    : X:");
    Serial.print(pixel3_x);
    Serial.print(" Y:");
    Serial.println(pixel3_y);
    
    Serial.print("4-X<->Y     : X:");
    Serial.print(pixel4_x);
    Serial.print(" Y:");
    Serial.println(pixel4_y);
    
    Serial.print("5-X<->Y+YTrs: X:");
    Serial.print(pixel5_x);
    Serial.print(" Y:");
    Serial.println(pixel5_y);
    
    Serial.print("6-X<->Y+XTrs: X:");
    Serial.print(pixel6_x);
    Serial.print(" Y:");
    Serial.println(pixel6_y);
    
    // Ekranda goster (Deneme 5'i kullanalim - genelde bu dogrudur)
    int pixelX = constrain(pixel5_x, 0, ekranW);
    int pixelY = constrain(pixel5_y, 0, ekranH);
    
    // Kirmizi nokta ciz
    tft.fillCircle(pixelX, pixelY, 3, RED);
    
    // Alt kisimda degerler
    tft.fillRect(0, ekranH-30, ekranW, 30, BLACK);
    tft.setTextSize(1);
    tft.setTextColor(YELLOW);
    tft.setCursor(5, ekranH-25);
    tft.print("Ham X:");
    tft.print(hamX);
    tft.print(" Y:");
    tft.print(hamY);
    
    tft.setCursor(5, ekranH-12);
    tft.setTextColor(CYAN);
    tft.print("Pixel X:");
    tft.print(pixelX);
    tft.print(" Y:");
    tft.print(pixelY);
    
    delay(100);
  }
  
  delay(10);
}