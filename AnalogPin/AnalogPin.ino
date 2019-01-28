#include "monitor.h"
#include "averager.h"

ValueMonitor example;
averager exampleAve(10);
auto i =0;

void setup() {
  pinMode(13,OUTPUT);
  Serial.begin(115200);

  example.setup([](){
    exampleAve.idle(analogRead(0));
    return exampleAve.ave;
  }, [](float val){
    Serial.println(int(val));
    digitalWrite(13, val == 2);
  }, .05); 

  example.divide(0,1023,1023);

}

void loop() {
  example.idle();
}
