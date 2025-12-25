#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD adresi, ekran boyutu
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // LCD'yi başlat
  lcd.init();
  lcd.backlight();  // Arka ışığı aç

  // LCD ekranına "Merhaba Dünya" yaz
  lcd.setCursor(0, 0);  // 0. satır, 0. sütun
  lcd.print("Merhaba Dünya");
}

void loop() {
  // Döngüde yapılacak bir şey yok
}
