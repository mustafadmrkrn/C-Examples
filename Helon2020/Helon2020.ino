#define solpwm1 3
#define solpwm2 11
#define sagpwm1 9
#define sagpwm2 10

#define solqtr A0
#define sagqtr A3
#define solsensor 12
#define onsensor 8
#define sagsensor A5 


#define led_buzzer 13
#define button 7
#define ir 2

#define DS1 4
#define DS2 5
#define DS3 6

int qtr = 900;
int irr=0;
int Speed = 20;
int MaxSpeed = 102; // Idle Speed while no sensor giving data.
int LastValue = 5; // Last Value Variable for remembering last Opponent sensor sense.

void setup()
{

  pinMode(solpwm1, OUTPUT);
  pinMode(solpwm2, OUTPUT);
  pinMode(sagpwm1, OUTPUT);
  pinMode(sagpwm2, OUTPUT);

  pinMode(solqtr, INPUT_PULLUP);
  pinMode(sagqtr, INPUT_PULLUP);
  pinMode(solsensor, INPUT_PULLUP);
  pinMode(onsensor, INPUT_PULLUP);
  pinMode(sagsensor, INPUT_PULLUP);

  
  pinMode(led_buzzer, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(ir, INPUT);

  pinMode(DS1, INPUT_PULLUP);
  pinMode(DS2, INPUT_PULLUP);
  pinMode(DS3, INPUT_PULLUP);


  digitalWrite(led_buzzer, LOW);
  digitalWrite(solpwm1, LOW);
  digitalWrite(solpwm2, LOW);
  digitalWrite(sagpwm1, LOW);
  digitalWrite(sagpwm2, LOW);

//Serial.begin(9600);
  tone(led_buzzer, 200, 350);
  delay(300);
  noTone(led_buzzer);

  tone(led_buzzer, 300, 200);
  tone(led_buzzer, 4000, 500);
  noTone(led_buzzer);

}


void loop() {

 

Wait:


 

  if (digitalRead(solsensor) == LOW || digitalRead(onsensor) == LOW || digitalRead(sagsensor) == LOW) {
    digitalWrite(led_buzzer, HIGH);
  } else {
    digitalWrite(led_buzzer, LOW);
  }


   if (digitalRead(ir)==1) {
    irr=1;

    /*for (int i = 0; i < 6; i++) {
      Set_Motor(0, 0, 1);
      tone(led_buzzer, 523, 200);
      delay(200);
      noTone(led_buzzer);
      delay(600);
    }*/

    if (digitalRead(DS1) == 0 && digitalRead(DS2) == 1 && digitalRead(DS3) == 1) {
      //Serial.print("sola dön");
      Set_Motor(-100, 100, 120);
      Set_Motor(80, 80, 20);
      LastValue = 7 ;
    }

    else if (digitalRead(DS1) == 0 && digitalRead(DS2) == 0 && digitalRead(DS3) == 1) {
     // Serial.print("sola açı");
      Set_Motor(40, 80, 120);
      LastValue = 7;
    }

    else if (digitalRead(DS1) == 1 && digitalRead(DS2) == 1 && digitalRead(DS3) == 0) {
     // Serial.print("Sag");
      Set_Motor(100, -100, 120);
      Set_Motor(80, 80, 20);
      LastValue = 3;
    }
    else if (digitalRead(DS1) == 1 && digitalRead(DS2) == 0 && digitalRead(DS3) == 0) {
      //Serial.print("saga açı");
      Set_Motor(80, 40, 120);

      LastValue = 3;
    } 
     else if (digitalRead(DS1) == 0 && digitalRead(DS2) == 0 && digitalRead(DS3) == 0) {
      //Serial.print("saga açı");
      Set_Motor(40, 40, 100);

      LastValue = 3;
    } 
   

    noTone(led_buzzer);
   goto Start;
  }


  if (digitalRead(button) == 0) {

    for (int i = 0; i < 6; i++) {
      Set_Motor(0, 0, 1);
      tone(led_buzzer, 523, 200);
      delay(200);
      noTone(led_buzzer);
      delay(600);
    }

    if (digitalRead(DS1) == 0 && digitalRead(DS2) == 1 && digitalRead(DS3) == 1) {
      //Serial.print("sola dön");
      Set_Motor(-100, 100, 120);
      Set_Motor(80, 80, 20);
      LastValue = 7 ;
    }

    else if (digitalRead(DS1) == 0 && digitalRead(DS2) == 0 && digitalRead(DS3) == 1) {
     // Serial.print("sola açı");
      Set_Motor(40, 80, 170);
      LastValue = 7;
    }

    else if (digitalRead(DS1) == 1 && digitalRead(DS2) == 1 && digitalRead(DS3) == 0) {
     // Serial.print("Sag");
      Set_Motor(100, -100, 120);
      Set_Motor(80, 80, 20);
      LastValue = 3;
    }
    else if (digitalRead(DS1) == 1 && digitalRead(DS2) == 0 && digitalRead(DS3) == 0) {
      //Serial.print("saga açı");
      Set_Motor(80, 40, 170);

      LastValue = 3;
    } 
         else if (digitalRead(DS1) == 0 && digitalRead(DS2) == 0 && digitalRead(DS3) == 0) {
      //Serial.print("saga açı");
      Set_Motor(40, 40, 100);

      LastValue = 3;
    } 
   

    noTone(led_buzzer);
    goto Start;
  }

goto Wait;


Start:

if (digitalRead(ir)==0 && irr==1) {
  Set_Motor(0, 0, 1);
  irr=0;
  goto Wait;
  
 }


if (analogRead(solqtr)>600 && analogRead(sagqtr)>600 ){ 


if (digitalRead(onsensor) == LOW) {
   Set_Motor(MaxSpeed, MaxSpeed, 1);
   digitalWrite(led_buzzer, HIGH);
  } else if (digitalRead(solsensor) == LOW) {
    Set_Motor(-102, 102, 1);
    digitalWrite(led_buzzer, HIGH);
    LastValue = 7;
  } else if (digitalRead(sagsensor) == LOW) {
 
    Set_Motor(102, -102, 1);
    digitalWrite(led_buzzer, HIGH);
    LastValue = 3;
  } else
  {

      digitalWrite(led_buzzer, LOW);

    if (LastValue == 7) {
    Set_Motor(-102, 102, 2);
    } else  // Left Turning Based on SPD (A7) Trimpot
     
    if (LastValue == 3) {
    Set_Motor(102, -102, 2); // Right Turning Based on SPD (A7) Trimpot
    }
    
    }
    
}else

if (analogRead(solqtr)<600 && analogRead(sagqtr)>600 ){
  Set_Motor(-102, -102, 150);
  Set_Motor(102, -102, 100);
  //LastValue=3;
   digitalWrite(led_buzzer, LOW);
}else

if (analogRead(solqtr)>600 && analogRead(sagqtr)<600 ){
  Set_Motor(-102, -102, 150);
  Set_Motor(-102, 102, 100);
 // LastValue=7; 
   digitalWrite(led_buzzer, LOW);
}else

if (analogRead(solqtr)<600 && analogRead(sagqtr)<600 ){
if (LastValue==3){
    Set_Motor(-102, -102, 150);
  Set_Motor(102, -102, 100);
  //LastValue=3;
   digitalWrite(led_buzzer, LOW);
}else

if (LastValue==7){
    Set_Motor(-102, -102, 150);
  Set_Motor(-102, 102, 100);
  //LastValue=7;
   digitalWrite(led_buzzer, LOW);
}



}



  
  goto Start;
}



void Set_Motor (float Lval, float Rval, int timex) {
  Lval = Lval * 2.5;
  Rval = Rval * 2.5;

  if (Lval >= 0) {
    digitalWrite(solpwm1,LOW );
    analogWrite(solpwm2,Lval );
  } else {
    Lval= abs(Lval);
    analogWrite(solpwm1,Lval);
    digitalWrite(solpwm2, LOW );
  }

  if (Rval >= 0) {
    analogWrite(sagpwm1, Rval);
    digitalWrite(sagpwm2, LOW);
  } else {
    Rval= abs(Rval);
    digitalWrite(sagpwm1, LOW);
    analogWrite(sagpwm2, Rval);
  }
  delay(timex);
}
