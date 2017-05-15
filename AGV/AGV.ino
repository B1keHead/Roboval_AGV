//LIBRERIE
#include <Servo.h>
Servo servo1;

//VARIABILI
const int dirM1Apin = 4;
const int dirM2Apin = 2;
const int pwmM1Apin = 5;
const int pwmM2Apin = 3;

const int triggerPort1 = A0;
const int echoPort1 = A1;

const int buzzerPin = 6;

char com;

long distanzadx;
long distanzasx;

boolean automatica = false;
int s;

//FUNZIONI
void rampa_start(){
  for(s = 0; s < 255; s += 1)
  {
    digitalWrite(dirM1Apin, LOW);
    digitalWrite(dirM2Apin, LOW);
    analogWrite(pwmM1Apin, s);
    analogWrite(pwmM2Apin, s); 
    delay(3);
  }
}
void rampa_stop(){
  for(s = 255; s >= 0; s -= 1)
  {
    digitalWrite(dirM1Apin, LOW);
    digitalWrite(dirM2Apin, LOW);
    analogWrite(pwmM1Apin, s);
    analogWrite(pwmM2Apin, s); 
    delay(2);
  }
}
void rampa_entrata_curvaSX(){
  for(s = 255; s >= 127; s -= 1)
  {
    digitalWrite(dirM1Apin, HIGH);
    digitalWrite(dirM2Apin, LOW);
    analogWrite(pwmM1Apin, s);
    analogWrite(pwmM2Apin, s); 
    delay(2);
  }
}
void rampa_in_curvaSX(){
    digitalWrite(dirM1Apin, HIGH);
    digitalWrite(dirM2Apin, LOW);
    analogWrite(pwmM1Apin, 127);
    analogWrite(pwmM2Apin, 127); 
    delay(2);
}
void rampa_uscita_curvaSX(){
  for(s = 127; s < 255; s += 1)
  {
    digitalWrite(dirM1Apin, HIGH);
    digitalWrite(dirM2Apin, LOW);
    analogWrite(pwmM1Apin, s);
    analogWrite(pwmM2Apin, s); 
    delay(3);
  }
}
void rampa_entrata_curvaDX(){
  for(s = 255; s >= 127; s -= 1)
  {
    digitalWrite(dirM1Apin, LOW);
    digitalWrite(dirM2Apin, HIGH);
    analogWrite(pwmM1Apin, s);
    analogWrite(pwmM2Apin, s); 
    delay(2);
  }
}
void rampa_in_curvaDX(){
    digitalWrite(dirM1Apin, LOW);
    digitalWrite(dirM2Apin, HIGH);
    analogWrite(pwmM1Apin, 127);
    analogWrite(pwmM2Apin, 127); 
    delay(2);
}
void rampa_uscita_curvaDX(){
  for(s = 127; s < 255; s += 1)
  {
    digitalWrite(dirM1Apin, LOW);
    digitalWrite(dirM2Apin, HIGH);
    analogWrite(pwmM1Apin, s);
    analogWrite(pwmM2Apin, s); 
    delay(3);
  }
}
void avanti(){
  rampa_start();
  analogWrite(pwmM1Apin, 255);
  analogWrite(pwmM2Apin, 255); 
}
void avanti_automatica(){
   digitalWrite(dirM1Apin, LOW);
   digitalWrite(dirM2Apin, LOW);
   analogWrite(pwmM1Apin, 255);
   analogWrite(pwmM2Apin, 255);    
}

void indietro(){
  digitalWrite(dirM1Apin, HIGH);
  digitalWrite(dirM2Apin, HIGH);
  analogWrite(pwmM1Apin, 255);
  analogWrite(pwmM2Apin, 255);
}
void destra(){
  rampa_entrata_curvaDX();
  rampa_in_curvaDX();
  rampa_uscita_curvaDX();
}
void sinistra(){
  rampa_entrata_curvaSX();
  rampa_in_curvaSX();
  rampa_uscita_curvaSX();
}
void fermo(){
  rampa_stop();
  analogWrite(pwmM1Apin, 0);
  analogWrite(pwmM2Apin, 0);
}
long sonar1(){
  digitalWrite(triggerPort1, LOW);
  digitalWrite(triggerPort1, HIGH);
  delay(10);
  digitalWrite(triggerPort1, LOW);
  long durata = pulseIn(echoPort1, HIGH);
  long distanza = 0.034 * durata / 2;
  return distanza;
}

void getCommand(){
  
  if(Serial.available()) {
    char com = Serial.read();
    if(com == 'F') avanti();
    if(com == 'B') indietro();
    if(com == 'R') destra();
    if(com == 'L') sinistra();
    if(com == 'S') fermo();
    if(com == 'A'){
      automatica = !automatica;
      if(automatica == true){
        fermo();
        delay(1000);
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  
  pinMode(dirM1Apin, OUTPUT);
  pinMode(dirM2Apin, OUTPUT);
  pinMode(pwmM1Apin, OUTPUT);
  pinMode(pwmM2Apin, OUTPUT);

  pinMode(triggerPort1, OUTPUT);
  pinMode(echoPort1, INPUT);
  pinMode(buzzerPin, OUTPUT);

  fermo();

  servo1.attach(10);
  servo1.write(109);
}


void loop(){

  getCommand();

 if(automatica == true){

    servo1.write(109);

    if(sonar1() < 70){
      fermo();
      
      delay(400);
      servo1.write(10);
      delay(400);

      distanzadx = sonar1();

      delay(50);
      servo1.write(160);
      delay(600);

      distanzasx = sonar1();
      
      delay(50);
      servo1.write(100);
      delay(400);

      if(distanzadx > distanzasx){
        indietro();
        delay(300);
        destra();
        delay(400);
        fermo();
      }
      
      else{
         indietro();
        delay(300);
        sinistra();
        delay(400);
        fermo();
      }
   }

   else
    avanti_automatica();
  } 

  delay(1);
}
