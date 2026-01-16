#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <EEPROM.h>

#include <ThreeWire.h>
#include <RtcDS1302.h>

ThreeWire myWire(7, 6, 8); 
RtcDS1302<ThreeWire> Rtc(myWire);

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo balikServosu;

// ---- AYAR ----
const uint32_t YEMLEME_SURESI = 84UL * 3600UL; // 84 saat
const int EEPROM_ADRES = 0;

uint32_t sonYemlemeZamani = 0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  balikServosu.attach(9);
  balikServosu.write(0);

  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!Rtc.IsDateTimeValid()) {
    Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected()) {
    Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }

  // ---- EEPROM'DAN SON YEMLEME ZAMANINI OKU ----
  EEPROM.get(EEPROM_ADRES, sonYemlemeZamani);

  // EEPROM ilk kez kullanılıyorsa
  if (sonYemlemeZamani == 0 || sonYemlemeZamani > 4000000000UL) {
    RtcDateTime simdi = Rtc.GetDateTime();
    sonYemlemeZamani = simdi.TotalSeconds();
    EEPROM.put(EEPROM_ADRES, sonYemlemeZamani);
  }
}

void loop() {
  RtcDateTime simdi = Rtc.GetDateTime();
  uint32_t simdiSaniye = simdi.TotalSeconds();

  // ---- LCD SAAT ----
  lcd.setCursor(0, 0);
  lcd.print("Saat: ");
  printDigits(simdi.Hour());
  lcd.print(":");
  printDigits(simdi.Minute());
  lcd.print(":");
  printDigits(simdi.Second());

  uint32_t gecenSure = simdiSaniye - sonYemlemeZamani;
  uint32_t kalanSure = (gecenSure >= YEMLEME_SURESI) ? 0 : (YEMLEME_SURESI - gecenSure);

  lcd.setCursor(0, 1);
  lcd.print("Kalan: ");
  lcd.print(kalanSure / 3600);
  lcd.print("s ");
  lcd.print((kalanSure % 3600) / 60);
  lcd.print("d   ");

  // ---- YEMLEME ----
  if (gecenSure >= YEMLEME_SURESI) {
    yemle();
    sonYemlemeZamani = simdiSaniye;
    EEPROM.put(EEPROM_ADRES, sonYemlemeZamani); // KAYDET
  }

  delay(500);
}

void yemle() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("YEMLENIYOR!");

  balikServosu.write(90);
  delay(1500);
  balikServosu.write(0);

  lcd.clear();
}

// ---- 0'lı yazdırma ----
void printDigits(int d) {
  if (d < 10) lcd.print("0");
  lcd.print(d);
}
