#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <IRremote.h>

// --- PIN TANIMLARI ---
#define SS_PIN 10
#define RST_PIN 9
#define IR_PIN 8

// --- NESNELER ---
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Stepper stepper(2048, 4, 6, 5, 7);
IRrecv irrecv(IR_PIN);
decode_results results;

// --- DEĞİŞKENLER ---
bool kapiAcik = false;

// --- AYARLAR ---
void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  irrecv.enableIRIn();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Kapi Kilit Sistemi");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kart bekleniyor...");
  lcd.setCursor(0, 1);
  lcd.print("Durum: KAPI ACIK"); // <-- tersine çevrildi (ilk durumda kapalı değil açık yazacak)

  stepper.setSpeed(10);
}

// --- ANA DÖNGÜ ---
void loop() {
  // --- RFID KART OKUMA ---
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("Okunan Kart UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    if (!kapiAcik) {
      kapiAc();
    } else {
      kapiKapat();
    }

    mfrc522.PICC_HaltA();
  }

  // --- IR KONTROL ---
  if (irrecv.decode(&results)) {
    unsigned long kod = results.value;
    Serial.print("IR Kod: ");
    Serial.println(kod, HEX);

    // 1 tuşu -> kapı aç
    if (kod == 0xFFA25D) {
      if (!kapiAcik) kapiAc();
    }

    // 2 tuşu -> kapı kapa
    if (kod == 0xFF629D) {
      if (kapiAcik) kapiKapat();
    }

    irrecv.resume();
  }
}

// --- KAPI AÇMA FONKSİYONU ---
void kapiAc() {
  Serial.println("Kapı Açılıyor...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kapı Açiliyor...");
  stepper.step(-2048 / 2); // 180 derece
  kapiAcik = true;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kart bekleniyor...");
  lcd.setCursor(0, 1);
  lcd.print("Durum: KILITLI"); // <-- tersine çevrildi
}

// --- KAPI KAPATMA FONKSİYONU ---
void kapiKapat() {
  Serial.println("Kapı Kapanıyor...");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kapı Kapanıyor...");
  stepper.step(2048 / 2); // 180 derece geri
  kapiAcik = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kart bekleniyor...");
  lcd.setCursor(0, 1);
  lcd.print("Durum: KAPI ACIK"); // <-- tersine çevrildi
}
