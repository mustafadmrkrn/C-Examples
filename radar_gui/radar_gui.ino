#include <MeMCore.h> // mBot kartı (mCore) için gerekli temel kütüphane

// Sensör ve Motor Tanımlamaları
MeLineFollower lineFollower(PORT_2); // Çizgi İzleme Sensörü Port 2'de
MeDCMotor motorLeft(M1);             // Sol Motor (M1)
MeDCMotor motorRight(M2);            // Sağ Motor (M2)

// Hız ayarları (Orta-Yüksek Hız Ayarları)
int maxSpeed = 200;  // Çizgi üstündeyken maksimum hız
int normalSpeed = 160; // Hafif sapmalarda taban hız
int turnSpeed = 120; // Tek sensördeyken hız düşüşü
int lostSpeed = 100; // Çizgi kaybolduğunda arama hızı

// AGRESİF PID parametreleri
// Bu değerler, hızlı ve keskin dönüşler için çok yüksek tutulmuştur.
float Kp = 60.0; // Oransal Kazanç: Hata anında düzeltme gücü
float Kd = 45.0; // Türevsel Kazanç: Titremeyi engellemeye çalışır
int lastError = 0; // Bir önceki döngüdeki hata değeri

void setup() {
  // Serial.begin(9600); // Hata ayıklama (debug) yapmak için bu satırı açabilirsiniz.
}

void loop() {
  int s = lineFollower.readSensors();
  int error = 0;
  int baseSpeed = normalSpeed;

  // ------------------------- Hata Hesaplama -----------------------------
  switch (s) {
    case S1_OUT_S2_OUT:  // 00: Çizgi Kayboldu (En keskin kurtarma dönüşü)
      // En son hangi yöne sapıyorsa o yöne doğru daha da keskin dön
      if (lastError > 0) error = 7; 
      else error = -7; 
      baseSpeed = lostSpeed;
      break;

    case S1_OUT_S2_IN:   // 01: Sağ siyah (Hafif Sola Dön)
      error = 4; // Agresif hata değeri
      baseSpeed = turnSpeed;
      break;

    case S1_IN_S2_OUT:   // 10: Sol siyah (Hafif Sağa Dön)
      error = -4; // Agresif hata değeri
      baseSpeed = turnSpeed;
      break;

    case S1_IN_S2_IN:    // 11: İki sensör siyah (Düz Git)
      error = 0;
      baseSpeed = maxSpeed; // Maksimum hızda ilerle
      break;
  }

  // ------------------------- PID Kontrolü ------------------------------
  // Correction (Düzeltme): Kp * Hata + Kd * (Hata Farkı)
  int derivative = error - lastError;
  int correction = (int)(Kp * error + Kd * derivative); 
  lastError = error;

  // Motor hızları PID düzeltmesi ile ayarlanır
  int leftSpeed = baseSpeed + correction;
  int rightSpeed = baseSpeed - correction;

  // Hız Sınırlama (Motorların 255'ten büyük veya -255'ten küçük olmasını engeller)
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  // ------------------------- Motorları Çalıştır ---------------------------
  // ÖNEMLİ: Motor Yön Düzeltmesi
  // -leftSpeed: M1 motorunun standart yönünü ters çevirerek robotun ileri gitmesini sağlar.
  motorLeft.run(-leftSpeed); 
  motorRight.run(rightSpeed);

  // Kısa bekleme (PID döngü hızını çok yüksek tutar)
  delay(5);
}