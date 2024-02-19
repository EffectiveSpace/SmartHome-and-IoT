#include <ThingerESP8266.h>
#include "arduino_secrets.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
ESP8266WebServer server(80);

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

  WiFi.mode(WIFI_AP);
  WiFi.softAP(LOGIN, PASSWORD);
  Serial.println(WiFi.softAPIP());
  server.on("/", start_page);
  server.on("/on", relay_on);
  server.on("/off", relay_off);
  server.on("/auto_on", auto_on);
  server.on("/auto_off", auto_off);
  server.begin();
  
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
  server.handleClient();
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

void start_page(){
  server.send(200, "text/html", sendHtml(t, auto_mode));
}

void relay_on(){
  digitalWrite(RELAY, HIGH);
  server.send(200, "text/html", sendHtml(t, auto_mode));
}

void relay_off(){
  digitalWrite(RELAY, LOW);
  server.send(200, "text/html", sendHtml(t, auto_mode));
}

void auto_on(){
  auto_mode = true;
  server.send(200, "text/html", sendHtml(t, auto_mode));
}

void auto_off(){
  auto_mode = false;
  server.send(200, "text/html", sendHtml(t, auto_mode));
}

String sendHtml(float t, bool auto_mode){
  String html = "<html>";
  html+= "<title>SCB</title><body>";
  html+= "<h1 align = \"center\" style = \"color: green\">SMART_CAT_BED</h1><hr width = 50% color = \"green\">";
  html+= "<h2 align = \"center\">TEMPERATURE:" + (String)t + "deg</h2><hr width = 50% color = \"green\">";
  html+= "<h2 align = \"center\">HEATING</h2>";
  html+= "<p align = \"center\"><a href = \"/on\"><button style = \"width:50px; height: 30px; background:purple; color:white\">ON</button></a> | <a href = \"/off\"><button style = \"width:50px; height: 30px; background:purple; color:white\">OFF</button></a></p>";
  html+= "<hr width = 50% color = \"green\">";
  html+= "<h2 align = \"center\">AUTO_MODE</h2>";
  if (auto_mode){
    html+= "<p align = \"center\"><a href = \"/auto_off\"><button style = \"width:50px; height: 30px; background:purple; color:white\">OFF</button></a></p>";
  }
  else{
    html+= "<p align = \"center\"><a href = \"/auto_on\"><button style = \"width:50px; height: 30px; background:purple; color:white\">ON</button></a></p>";
  }
  html+= "<hr width = 50% color = \"green\">";
  html+= "<p align = \"center\">made by \"Andromeda Stars\" team</p>";
  html+="</body></html>";
  return html;
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
