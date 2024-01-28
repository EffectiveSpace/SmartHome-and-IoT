//Подключение библиотек
//********************************
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//********************************
//Создание констант и переменных
#define HUM A0
#define PUMP D3
#define LED D4
#define APSSID "NodeMCU"
#define APPSK "12345678"

const int minValue = 286; //датчик опущен в воду
const int maxValue = 712; //датчик полностью сухой
int h = 0;
bool flag = false;
uint32_t timer1;

//Создаем объекты
//********************************
ESP8266WebServer server(80);
//********************************

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(HUM, INPUT);
  pinMode(PUMP, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(PUMP, 1);
  digitalWrite(LED, 1);
  //Настройка wifi
  WiFi.mode(WIFI_AP);
  WiFi.softAP(APSSID, APPSK);
  Serial.println(WiFi.softAPIP());
  //Регистрация обработчиков сервера и запуск сервера
  server.on("/", start_page);
  server.on("/on_pump", pump_on);
  server.on("/off_pump", pump_off);
  server.on("/on_led", led_on);
  server.on("/off_led", led_off);
  server.on("/mode_on", mode_on);
  server.on("/mode_off", mode_off);
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient(); //проверка запроса от клиента (браузера)
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
  /* 
  */
}


void start_page(){
  server.send(200, "text/html", sendHtml(h, flag));
}

void mode_on(){
  flag = true;
  server.send(200, "text/html", sendHtml(h, flag));
}

void mode_off(){
  flag = false;
  server.send(200, "text/html", sendHtml(h, flag));
}
void led_on(){
  digitalWrite(LED, 0);
  server.send(200, "text/html", sendHtml(h, flag));
}

void led_off(){
  digitalWrite(LED, 1);
  server.send(200, "text/html", sendHtml(h, flag));
}

void pump_on(){
  digitalWrite(PUMP, 0);
  server.send(200, "text/html", sendHtml(h, flag));
}

void pump_off(){
  digitalWrite(PUMP, 1);
  server.send(200, "text/html", sendHtml(h, flag));
}


String sendHtml(int h, bool flag){
  String html = "<html><title>SMART_FARM</title><body bgcolor=\"green\">";
  html+="<h2 align = \"center\">SMART-FARM</h2><hr width=50%/>";
  html+="<p align = \"center\">Hum_ground:"+(String)h+"%</p>";
  html+="<hr width=50%/>";
  html+="<p align = \"center\">LED:<a href=\"/on_led\"><button>ON</button></a> | <a href=\"/off_led\"><button>OFF</button></a></p>";
  html+="<hr width=50%/>";
  html+="<p align = \"center\">PUMP:<a href=\"/on_pump\"><button>ON</button></a> | <a href=\"/off_pump\"><button>OFF</button></a></p>";
  html+="<hr width=50%/>";
  if (flag){
    html += "<p align = \"center\">MODE_AUTO:<a href=\"/mode_off\"><button>OFF</button></a></p>";
  }
  else {
    html += "<p align = \"center\">MODE_AUTO:<a href=\"/mode_on\"><button>ON</button></a></p>";
  }
  html+="</body></html>";
  return html;
}
