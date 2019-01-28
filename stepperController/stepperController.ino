#include "stepperControlAdafruit.h"

//steps per rev, which port, and 
stepper motor(100,2,60);

bool fired = 0;

int sensor = 11;

float rampFunc(float val){
  return pow(val,.5);
}

void rampUp(){
  motor.ramp(1,10000,rampDown);
}

void rampDown(){
  motor.ramp(-1,10000,rampUp);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(sensor,INPUT_PULLUP);
  pinMode(13,OUTPUT);

  motor.setup();

  motor.setProfile(rampFunc);
  
  rampUp();

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  motor.idle();
}
