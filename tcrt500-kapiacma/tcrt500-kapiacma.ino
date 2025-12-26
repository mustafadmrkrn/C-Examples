#include <Servo.h>

#define TCRT_PIN 2
#define SERVO_PIN 6

#define R_PIN 9
#define G_PIN 10
#define B_PIN 11

Servo doorServo;

int aciKapali = 0;    // Kapı kapalı açısı
int aciAcik = 90;    // Kapı açık açısı

void setup() {
  pinMode(TCRT_PIN, INPUT);

  pinMode(R_PIN, OUTPUT);
  pinMode(G_PIN, OUTPUT);
  pinMode(B_PIN, OUTPUT);

  doorServo.attach(SERVO_PIN);

  // Başlangıç
  doorServo.write(aciKapali);
  setColor(255, 0, 0); // Kırmızı (kapalı)
}

void loop() {
  int algilama = digitalRead(TCRT_PIN);

  if (algilama == LOW) { // Çoğu TCRT5000 LOW'da algılar
    // Hareket algılandı
    setColor(0, 0, 255); // Mavi
    doorServo.write(aciAcik);
    delay(3000);         // Kapı açık kalma süresi
    doorServo.write(aciKapali);
    setColor(255, 0, 0); // Kırmızı
  }
}

void setColor(int r, int g, int b) {
  analogWrite(R_PIN, r);
  analogWrite(G_PIN, g);
  analogWrite(B_PIN, b);
}
