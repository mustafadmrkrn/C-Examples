#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// Sensör ve Röle Pinleri
#define DHTPIN 3       // DHT11 veri pini
#define DHTTYPE DHT11  // DHT11 sensör tipi
#define LDRPIN A0      // LDR sensörü
#define SOILPIN A1     // Toprak nem sensörü
#define RELAYPIN 7     // Röle (Su motoru)

// LCD Tanımlama
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// Nem eşiği (%)
int nemEsigi = 30;

void setup() {
    Serial.begin(9600);
    dht.begin();
    lcd.begin(16,2);
    lcd.backlight();

    pinMode(RELAYPIN, OUTPUT);
    digitalWrite(RELAYPIN, HIGH); // Röleyi kapalı başlat

    lcd.setCursor(0, 0);
    lcd.print("Akilli Sera");
    delay(2000);
    lcd.clear();
}

void loop() {
    // Sensörlerden verileri oku
    float sicaklik = dht.readTemperature();
    float nem = dht.readHumidity();
    int isikSeviyesi = analogRead(LDRPIN);
    int toprakNem = analogRead(SOILPIN);
    int toprakNemYuzde = map(toprakNem, 1023, 0, 0, 100);

    // LCD'ye yazdır
    lcd.setCursor(0, 0);
    lcd.print("Sic:"); lcd.print(sicaklik); lcd.print("C");
    lcd.setCursor(9, 0);
    lcd.print("Nem:"); lcd.print(nem); lcd.print("%");

    lcd.setCursor(0, 1);
    lcd.print("Toprak:"); lcd.print(toprakNemYuzde); lcd.print("%");

    // Otomatik sulama kontrolü
    if (toprakNemYuzde < nemEsigi) {
        digitalWrite(RELAYPIN, LOW); // Röleyi aç (motor çalışır)
        lcd.setCursor(9, 1);
        lcd.print("Sulama On ");
    } else {
        digitalWrite(RELAYPIN, HIGH); // Röleyi kapat
        lcd.setCursor(9, 1);
        lcd.print("Sulama Kap ");
    }

    Serial.print("Sicaklik: "); Serial.print(sicaklik); Serial.print(" C, ");
    Serial.print("Nem: "); Serial.print(nem); Serial.print("%, ");
    Serial.print("Işık: "); Serial.print(isikSeviyesi); Serial.print(", ");
    Serial.print("Toprak Nem: "); Serial.print(toprakNemYuzde); Serial.println("%");

    delay(2000);
}
