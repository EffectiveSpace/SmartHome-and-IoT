
#include <ThingerESP8266.h>
#include "arduino_secrets.h"

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
#define RELAY D2
#define TERM A0
//------------------------------------------------------------
#define RT0 100000 //сопротивление в Омах внешнего резистора
#define B 3977 //коэффициент характеризующий материал термистора
#define VCC 3.3 //опорное напряжение платы NodeMCU
#define R 100000 //сопротивление термистора при 25 град Цельсия
//------------------------------------------------------------
float RT, VR, ln, TX, T0 = 25.0, VRT;
uint32_t timer1;
float t, set_point = 30, delta_t = 5;
bool auto_mode = false;

void setup() {
  T0 = T0 + 273.15; // температура в Кельвинах
  pinMode(TERM, INPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  
  // add WiFi credentials
  thing.add_wifi(SSID, SSID_PASSWORD);

  // digital pin control example (i.e. turning on/off a light, a relay, configuring a parameter, etc)
  thing["relay"] << digitalPin(RELAY);
  thing["set_point"] << inputValue(set_point);
  thing["delta_t"] << inputValue(delta_t);
  thing["auto"] << inputValue(auto_mode);

  // resource output example (i.e. reading a sensor value)
  thing["temp"] >> outputValue(t);

  // more details at http://docs.thinger.io/arduino/
}

void loop() {
  thing.handle();
  if (millis()-timer1 > 1000){
    timer1 = millis();
    t = read_temp();
    if (auto_mode){
      if (t <= set_point-delta_t){
        digitalWrite(RELAY, HIGH);   
      }
      else if (t >= set_point+delta_t){
        digitalWrite(RELAY, LOW);
      }
    }
  }
}

//функция для чтения температуры с терморезистора
float read_temp(){
  VRT = analogRead(TERM);
  VRT = (VRT/1023)*VCC;
  VR = VCC - VRT;
  RT = VRT/(VR/R);
  ln = log(RT/RT0);
  TX = (1 / ((ln / B) + (1 / T0))); // пересчитанное значение температуры в Кельвинах
  TX = TX - 273.15; // искомая температура в градусах
  Serial.print("Temperature: ");
  Serial.print(TX);
  Serial.println("C");
  return TX;
}
