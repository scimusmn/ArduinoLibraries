#include "arduino.h"
#include "averager.h"
#include "timeOut.h"
#include "initList.h"

float analogVoltageScale = 3.3 / 3.6;

float analogResolution = 1024.;

class analogButton {
public:

  // variables for averaging.
  int taps;
  float r1;
  float * r2;
  float * voltages;
  int match;
  bool fired;
  int pin;
  averager current;
  TimeOut debounce;

  //void (*pressCB)(int);
  timeOutFunc* pressCB;

  analogButton(int num = 10){
    taps = 0;
    r1 = 0;
    pin =0;
  }

  template<typename C>
  void setup(int pn, std::initializer_list<float> R2, const C & cb){
    setResistors(R2);
    setCallback(pn, cb);
  }

  void setResistors(std::initializer_list<float> R2){
    taps = R2.size();

    voltages = new float[taps];

    Serial.println(taps);

    for( int i = 0; i < taps; i++){
      voltages[i] = (R2[i]) / (R2[i] + 1) ;
      Serial.println(voltages[i]);
    }
  }

  void setup(int pn, float * R2, void (*cb)(int)){
    setResistors(R2);
    setCallback(pn, cb);
  }

  void setResistors(float * R2){
    taps = sizeof(R2);

    voltages = new float[taps];

    Serial.println(taps);

    for( int i = 0; i < taps; i++){
      voltages[i] = (R2[i]) / (R2[i] + 1) ;
      Serial.println(voltages[i]);
    }
  }

  template<typename C> 
  void setCallback(int pn, const C & cb){
    pin = pn;
    pressCB = cb;
  }

  void idle(){
    current.addSample(analogRead(pin)/(analogResolution * analogVoltageScale));

    if(match >= 0 && abs(current() - voltages[match]) > .02){
      fired = false;
      debounce.clear();
      match = -1;
    }

    for(int j = 0; j < 4; j++){
      if(abs(current() - voltages[j]) < .02 && !debounce.running && !fired && match < 0){
        //Serial.println("Saw a match");
        match = j;
        debounce.clear();
        debounce.set([&, j](){
            fired = true;
            if(pressCB) pressCB(j);
        }, 50);
      }
    }
    

    debounce.idle();
  }
};
