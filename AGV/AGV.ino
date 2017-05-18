#include <SimpleTimer.h>

const uint8_t motorStepTime = 2;
const unsigned long autoStopDelta = 700;
const uint8_t autoStopPollInterval = autoStopDelta - 100;
const uint8_t autoModeInterval = 1000;  //todo put time needed to do action

const uint8_t motorPin1 = 5;
const uint8_t motorPin2 = 3;
const uint8_t directionMotor1 = 4;
const uint8_t directionMotor2 = 4;

const uint8_t sonarTrigger = A0;
const uint8_t sonarEcho = A1;

boolean autoModeEnabled;
SimpleTimer autoModeTimer;

unsigned long lastCmdAt;
char lastCmd;

SimpleTimer autoStopTimer;

boolean moving;
boolean turning;

void setup(){
  Serial.begin(9600);
  //todo put time needed to do action
  autoStopTimer.setInterval(autoStopPollInterval, autoStop);
  autoModeTimer.setInterval(autoModeInterval, autoMode);

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(directionMotor1, OUTPUT);
  pinMode(directionMotor2, OUTPUT);

  pinMode(sonarTrigger, OUTPUT);
  pinMode(sonarEcho, INPUT);
}

void loop(){
  if(!autoModeEnabled){
    autoStopTimer.run();
  }
  autoModeTimer.run();
}

void serialEvent(){
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if(inChar == 'A') autoModeEnabled = !autoModeEnabled;

    if(!autoModeEnabled){
      lastCmdAt = millis();

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
    Serial.print("Starting to move... ");
    
    for(int s; s < 255; s++){
      analogWrite(motorPin1, s);
      analogWrite(motorPin1, s);
      delay(motorStepTime);
    }
    moving = true;

    Serial.println("Done!");
  }else{
    Serial.println("Already moving.");
  }
}

void turn(){
  if(!turning){
    Serial.print("Starting to turn... ");
    
    for(int s; s < 50; s++){
      analogWrite(motorPin1, s);
      analogWrite(motorPin1, s);
      delay(1);
    }
    turning = true;

    Serial.println("Done!");
  }else{
    Serial.println("Already turning.");
  }
}

void decelerate(){  
  Serial.print("Stopping... ");

  digitalWrite(directionMotor1, LOW);
  digitalWrite(directionMotor2, LOW);
  
  for(int s = 254; s >= 0; s--){
    analogWrite(motorPin1, s);
    analogWrite(motorPin1, s);
    delay(motorStepTime);
  }
  moving = false;

  Serial.println("Stopped!");
}

void stopTurning(){
  Serial.print("Stopping turn... ");

  digitalWrite(directionMotor1, LOW);
  digitalWrite(directionMotor2, LOW);
  
  for(int s = 49; s >= 0; s--){
    analogWrite(motorPin1, s);
    analogWrite(motorPin1, s);
    delay(1);
  }
  turning = false;
  
  Serial.println("Stopped turning!");
}

void autoStop(){
  if(millis() - lastCmdAt > autoStopDelta){
    if(moving){
      decelerate();
    }else if(turning){
      stopTurning();
    }
  }
}

void autoMode(){
  if(autoModeEnabled){
    Serial.println("I'm doing automatic stuff");
  }
}

