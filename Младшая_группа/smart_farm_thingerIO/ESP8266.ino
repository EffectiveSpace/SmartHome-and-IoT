#define THINGER_SERIAL_DEBUG

#include <ThingerESP8266.h>
#include "arduino_secrets.h"

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
//********************************
//Создание констант и переменных
#define HUM A0
#define PUMP D3
#define LED D4

const int minValue = 286; //датчик опущен в воду
const int maxValue = 712; //датчик полностью сухой
int h = 0;
bool flag = false;
uint32_t timer1;

void setup() {
  // open serial for monitoring
  Serial.begin(115200);
  // set builtin led as output
  pinMode(HUM, INPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(PUMP, 1);
  digitalWrite(LED, 1);
  
  // add WiFi credentials
  thing.add_wifi(SSID, SSID_PASSWORD);

  // digital pin control example (i.e. turning on/off a light, a relay, configuring a parameter, etc)
  thing["led"] << digitalPin(LED);
  thing["pump"] << digitalPin(PUMP); 
  thing["auto"] << inputValue(flag);
  
  // resource output example (i.e. reading a sensor value)
  thing["hum"] >> outputValue(h);

  // more details at http://docs.thinger.io/arduino/
}

void loop() {
  thing.handle();
  
  if (millis()-timer1 > 2000){
    timer1 = millis();
    h = analogRead(HUM);
    h = map(h, maxValue, minValue, 0, 100);
    if (flag){
      if (h<30){
        digitalWrite(PUMP, 0);
        delay(600);
      }
      digitalWrite(PUMP, 1);
    }
  }  
}
