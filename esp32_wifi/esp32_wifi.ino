#include <WiFi.h> // ESP32'nin Wi-Fi kütüphanesini çağırıyoruz

// Kendi Wi-Fi adınızı ve şifrenizi buraya yazın
// (Büyük/küçük harf duyarlıdır, telefonda nasıl görünüyorsa aynen yazın)
const char* ssid = "EV_WIFI_ADINIZ";
const char* password = "WIFI_SIFRENIZ";

void setup() {
  // ESP32'nin haberleşme hızı genelde 115200'dür. 
  // Seri Monitör'ü açtığınızda sağ alttan 115200 baud seçmeyi unutmayın!
  Serial.begin(115200); 
  delay(1000); // Sistemin uyanması için kısa bir saniye bekle

  Serial.println();
  Serial.print("Su aga baglaniliyor: ");
  Serial.println(ssid);

  // Wi-Fi bağlantı işlemini başlat (Modemin kapısını çal)
  WiFi.begin(ssid, password);

  // Modeme bağlanana kadar (cevap gelene kadar) ekrana nokta koyarak bekle
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Yarım saniye bekle
    Serial.print(".");
  }

  // Döngüden çıktıysa bağlantı başarılı demektir!
  Serial.println("");
  Serial.println("Harika! WiFi Baglantisi Basarili.");
  
  // Modemin bize verdiği o özel IP adresini ekrana yazdır
  Serial.print("ESP32'nin IP Adresi: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Sadece bağlantı testi yaptığımız için burası şimdilik boş
}