#include <SimpleTimer.h>

const uint8_t motorStepTime = 4;
const uint8_t motorStepTurningTime = 3;
const uint8_t autoModeInterval = 1000;  //todo put time needed to do action

const uint8_t motorPin1 = 3;
const uint8_t motorPin2 = 5;
const uint8_t directionMotor1 = 2;
const uint8_t directionMotor2 = 4;

const uint8_t sonarTrigger = A0;
const uint8_t sonarEcho = A1;

boolean autoModeEnabled;
SimpleTimer autoModeTimer;

unsigned long lastCmdAt;
char lastCmd;

boolean moving;
boolean turning;

void setup(){
  Serial.begin(9600);
  autoModeTimer.setInterval(autoModeInterval, autoMode);

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(directionMotor1, OUTPUT);
  pinMode(directionMotor2, OUTPUT);

  pinMode(sonarTrigger, OUTPUT);
  pinMode(sonarEcho, INPUT);
}

void loop(){
  autoModeTimer.run();
}

void serialEvent(){
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    Serial.write(6);
    if(inChar == 'A') autoModeEnabled = !autoModeEnabled;

    if(!autoModeEnabled){
      if(lastCmd != inChar){
        if(moving){
          decelerate();
        }else if(turning){
          stopTurning();
        }
      }
      
      switch(inChar){
        case 'F':
          fwd();
          break;
        case 'B':
          bwd();
          break;
        case 'R':
          rx();
          break;
        case 'L':
          lx();
          break;
        case 'S':
          if(moving){
            decelerate();
          }else if(turning){
            stopTurning();
          }
          break;
      }

      lastCmd = inChar;
    }
  }
}

void fwd(){
  digitalWrite(directionMotor1, LOW);
  digitalWrite(directionMotor2, LOW);
  accelerate();
}

void bwd(){
  digitalWrite(directionMotor1, HIGH);
  digitalWrite(directionMotor2, HIGH);
  accelerate();
}

void rx(){
  digitalWrite(directionMotor1, LOW);
  digitalWrite(directionMotor2, HIGH);
  turn();
}

void lx(){
  digitalWrite(directionMotor1, HIGH);
  digitalWrite(directionMotor2, LOW);
  turn();
}

void accelerate(){  
  if(!moving){
    //Serial.print("Starting to move... ");
    
    for(int s = 0; s < 255; s++){
      analogWrite(motorPin1, s);
      analogWrite(motorPin2, s);
      delay(motorStepTime);
    }
    moving = true;

    //Serial.println("Done!");
  }else{
    //Serial.println("Already moving.");
  }
}

void turn(){
  if(!turning){
    //Serial.print("Starting to turn... ");
    
    for(int s = 0; s < 100; s++){
      analogWrite(motorPin1, s);
      analogWrite(motorPin2, s);
      delay(motorStepTurningTime);
    }
    turning = true;

    //Serial.println("Done!");
  }else{
    //Serial.println("Already turning.");
  }
}

void decelerate(){  
  //Serial.print("Stopping... ");
  
  for(int s = 254; s >= 0; s--){
    analogWrite(motorPin1, s);
    analogWrite(motorPin2, s);
    delay(motorStepTime);
  }
  moving = false;

  digitalWrite(directionMotor1, LOW);
  digitalWrite(directionMotor2, LOW);

  //Serial.println("Stopped!");
}

void stopTurning(){
  //Serial.print("Stopping turn... ");
  
  for(int s = 99; s >= 0; s--){
    analogWrite(motorPin1, s);
    analogWrite(motorPin2, s);
    delay(motorStepTurningTime);
  }
  turning = false;

  digitalWrite(directionMotor1, LOW);
  digitalWrite(directionMotor2, LOW);
  
  //Serial.println("Stopped turning!");
}

void autoMode(){
  if(autoModeEnabled){
    //Serial.println("I'm doing automatic stuff");
  }
}

