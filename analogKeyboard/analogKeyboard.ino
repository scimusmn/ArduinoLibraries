/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <bluefruit.h>
#include "averager.h"
#include "timeOut.h"
#include "analogButtons.h"

BLEDis bledis;
BLEHidAdafruit blehid;

const int numPlayers = 4;

analogButton players[numPlayers];

float R2[4] = {.51,1,2,3.3};

char playerKeys[numPlayers][4] = {
  {'a', 'b', 'c', 'd'},
  {'e', 'f', 'g', 'h'},
  {'i', 'j', 'k', 'l'},
  {'m', 'n', 'o', 'p'},
};

TimeOut turnoff[numPlayers];

TimeOut deboug;

void debugInt(){
  Serial.println(players[0].current());
  
  deboug.set(debugInt,500);
}

void setup() 
{
  Serial.begin(115200);

  Bluefruit.begin();
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(-20);
  Bluefruit.setName("MindMattersButtonPad");
  Bluefruit.autoConnLed(false);

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather 52");
  bledis.begin();

  for(int i = 0; i < numPlayers; i++){
    players[i].setup(A0 + i, R2, [&, i](int which){
      Serial.println(which);
      blehid.keyPress(playerKeys[i][which]);
      turnoff[i].set([](){
        blehid.keyRelease();
      }, 5);
    });
  }

  /* Start BLE HID
   * Note: Apple requires BLE device must have min connection interval >= 20m
   * ( The smaller the connection interval the faster we could send data).
   * However for HID and MIDI device, Apple could accept min connection interval 
   * up to 11.25 ms. Therefore BLEHidAdafruit::begin() will try to set the min and max
   * connection interval to 11.25  ms and 15 ms respectively for best performance.
   */
  blehid.begin();

  // Set callback for set LED from central
  blehid.setKeyboardLedCallback(set_keyboard_led);

  /* Set connection interval (min, max) to your perferred value.
   * Note: It is already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
   * min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms 
   */
  /* Bluefruit.setConnInterval(9, 12); */

  // Set up and start advertising
  startAdv();

  analogReadResolution(12);

  analogResolution = 4096;
  analogVoltageScale = 3.3/3.6;

  for( int i = 0; i < 4; i++){
    float volt = 3.3 * r2s[i] / (r2s[i] + r1);
    singleVals[i] = volt / 3.6;
    //Serial.println(singleVals[i]);
  }

  debugInt();
}

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(3);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void loop() 
{
  // Request CPU to enter low-power mode until an event/interrupt occurs
  waitForEvent();

  for( int i = 0; i < numPlayers; i++){
    players[i].idle();
  }

//  for(int i = 0; i < 4; i++){
//    players[i].addSample(analogRead(A0 + i)/4096.);
//
//    if(match[i] >= 0 && abs(players[i]() - singleVals[match[i]]) > .02){
//      fired[i] = false;
//      debounce[i].clear();
//      match[i] = -1;
//    }
//
//    for(int j = 0; j < 4; j++){
//      if(abs(players[i]() - singleVals[j]) < .02 && !debounce[i].running && !fired[i] && match[i] < 0){
//        //Serial.println("Saw a match");
//        match[i] = j;
//        debounce[i].clear();
//        debounce[i].set([&, i, j](){
//            fired[i] = true;
//            Serial.println(j,DEC);
//            blehid.keyPress(playerKeys[i][j]);
//            delay(5);
//            blehid.keyRelease();
//        }, 50);
//      }
//    }
//    
//
//    debounce[i].idle();
//  }

  deboug.idle();
}

/**
 * Callback invoked when received Set LED from central.
 * Must be set previously with setKeyboardLedCallback()
 *
 * The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
 *    Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
 */
void set_keyboard_led(uint8_t led_bitmap)
{
  // light up Red Led if any bits is set
  if ( led_bitmap )
  {
    //ledOn( LED_RED );
  }
  else
  {
    //ledOff( LED_RED );
  }
}
