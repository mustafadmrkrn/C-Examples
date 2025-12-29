#include <Servo.h>

const int trigPin = 3;
const int echoPin = 2;
const int buzzerPin = 8;  // Buzzer pini

// Variables for the duration and the distance
long duration;
int distance;
unsigned long previousMillis = 0;  // Son bip zamanı
int beepInterval = 1000;  // Bip aralığı (ms)
bool buzzerState = false;  // Buzzer durumu

Servo myServo;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  myServo.attach(9);
}

void loop() {
  // rotates the servo motor from 15 to 165 degrees
  for(int i=15;i<=165;i++){  
    myServo.write(i);
    delay(30);
    distance = calculateDistance();
    
    // Buzzer kontrolü - mesafeye göre bip hızı
    controlBuzzer();
    
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
  }
  
  // Repeats the previous lines from 165 to 15 degrees
  for(int i=165;i>15;i--){  
    myServo.write(i);
    delay(30);
    distance = calculateDistance();
    
    // Buzzer kontrolü
    controlBuzzer();
    
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.print(".");
  }
}

// Function for calculating the distance measured by the Ultrasonic sensor
int calculateDistance(){ 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2;
  return distance;
}

// Buzzer kontrol fonksiyonu
void controlBuzzer() {
  unsigned long currentMillis = millis();
  
  // Mesafe 100 cm'den küçükse buzzer aktif
  if(distance > 0 && distance < 100) {
    // Mesafeye göre bip aralığını ayarla
    // Yaklaştıkça hızlanır: 100cm=1000ms, 50cm=500ms, 10cm=100ms
    beepInterval = map(distance, 0, 100, 100, 1000);
    
    // Bip süresi geldi mi kontrol et
    if(currentMillis - previousMillis >= beepInterval) {
      previousMillis = currentMillis;
      buzzerState = !buzzerState;  // Durumu değiştir
      
      if(buzzerState) {
        tone(buzzerPin, 1000);  // Bip sesi çal
      } else {
        noTone(buzzerPin);  // Sustur
      }
    }
  } else {
    // 100 cm'den uzaksa buzzer kapalı
    noTone(buzzerPin);
    buzzerState = false;
  }
}