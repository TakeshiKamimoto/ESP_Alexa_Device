/*
 * This is developed from sample sketch 'EspalexaBasic'. (2021.7.8) T.Kamimoto
 */ 
#include <Espalexa.h>
#include "WiFiManager.h"  // added for WiFi Manager
#include <IRremoteESP8266.h>  // added for IR
#include <IRsend.h>           // added for IR
#define IR_LED        4    // added for IR
#define LED_OB        16     // LED indicator on board

// prototypes
void blinkingLED(int cycle, int msec);     // added for LED indicator //

//callback functions
void firstLightChanged(uint8_t brightness);

IRsend irsend(IR_LED);   // added for IR
Espalexa espalexa;

int current_value = 0;   // added. Executed Brightness is stored in this variable, which will be compared with next Brightness.

void setup()
{
  Serial.begin(115200);
  pinMode(LED_OB, OUTPUT);     // added for LED indicator //
  irsend.begin();   // added for IR

  WiFiManager wm;   // added for WiFi Manager
  if (!wm.autoConnect("ESP8266")) {     // added for WiFi Manager
    Serial.println("Cannot connect to WiFi. Please check data and reset the ESP.");
    delay(2500);
    ESP.restart();
    delay(5000);
  } 
  blinkingLED(5,50);  // for indication that WiFi is connected. WiFi接続が完了したらLEDを５回点滅させる。
  
  espalexa.addDevice("Light", firstLightChanged); //simplest definition, default state off
  espalexa.begin();
}
 
void loop()
{
  for (int i=0; i<2000; i++) {
   espalexa.loop();
   delay(1);
  }
  blinkingLED(1,10); // 1000ループ毎にLEDを瞬時点灯させる。
}

//our callback functions
void firstLightChanged(uint8_t brightness) {
    Serial.print("Device 1 changed to ");
    
    //do what you need to do here

    if ((brightness > 0) && (current_value == 0)) {   // Turn ON
      Serial.print("ON, brightness ");
      Serial.println(brightness);
      irsend.sendNEC(0x41B6D52A,32); // ON command
      blinkingLED(1,100);     // added for LED indicator //
    }
    else if ((brightness > current_value) && (current_value != 0)) {   // Dim UP
      Serial.print("ON, brightness ");
      Serial.println(brightness);
      irsend.sendNEC(0x41B65DA2,32); // INC command
      blinkingLED(1,50);     // added for LED indicator //
    }
    else if ((brightness < current_value) && (brightness != 0) ) {   // Dim DOWN
      Serial.print("ON, brightness ");
      Serial.println(brightness);
      irsend.sendNEC(0x41B6DD22,32); // DEC command
      blinkingLED(2,50);     // added for LED indicator //
    }
    else  {
      Serial.println("OFF");      //  Turn OFF
      Serial.print("OFF, brightness ");
      Serial.println(brightness);
      irsend.sendNEC(0x41B6D52A,32); // OFF command　シーリングライト(NEC製）の消灯コマンドは点灯コマンドと同じ（オルタネートスイッチ）。
      blinkingLED(1,500);     // added for LED indicator //
    }
    
    current_value = brightness;  // added. To memorize current brightness.　Alexaから送信された現在の照度を記憶しておく。（消灯時にはゼロになる）
}

void blinkingLED(int cycle, int msec) {     // added for LED indicator //
   for (int i=0; i<cycle; i++) {
    digitalWrite(LED_OB, HIGH);    
    delay(msec);               
    digitalWrite(LED_OB, LOW);     
    delay(msec);              
   }
}
