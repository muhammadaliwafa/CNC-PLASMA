/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>




bool ledState = 0;
const int ledPin = 2;
int32_t batas_bawah = -6000;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

#include "WebPage.h"
#include "WebServer.h"

float pulsePerUnit = 114.285;
int32_t currEncoder=0, target=0;
bool runFinish = false;
String statusTHC="2";
int V_Arc;
bool en = false, en_cmd=false;
int titik_nyala = 200, selisih = 10;
int up_boundary;
bool homing = false;
void notifyClients() {
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String msg;
    
    charToString((char*)data, msg);
//    Serial.println(msg);
    if(msg=="1"){
      int32_t temp = target+114;
      if(!(temp>0))
      target += 114;
    }else if(msg=="0"){
      int32_t temp = target-114;
      if(!(temp<batas_bawah))
      target -= 114;
    }else if(msg=="2"){//tambah titik nyala
      en_cmd = true;
    }else if(msg=="3"){
      en_cmd = false;
    }
    else if(msg=="a"){//tambah titik nyala
      titik_nyala +=3;
      up_boundary = titik_nyala + selisih;
      ws.textAll("sp"+String(titik_nyala));
    }else if(msg=="b"){
      titik_nyala -=3;
      up_boundary = titik_nyala + selisih;
      ws.textAll("sp" + String(titik_nyala));
    }else if(msg=="c"){
      homing=true;
      target = batas_bawah;
      Serial.println("homing starting");
    }
    else if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
//  // Connect to Wi-Fi
//  WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.println("Connecting to WiFi..");
//  }
//
//  // Print ESP Local IP Address
//  Serial.println(WiFi.localIP());
  wifiConnect();
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D2, OUTPUT);//output ke esp32
  pinMode(A0, INPUT);
  pinMode(D1, INPUT);// input deteksi plasma dihidupkan
  pinMode(D0, INPUT);//deteksi sensor proximity
  pinMode(D7, INPUT); // dari esp flood detection
  digitalWrite(D2, HIGH);
  up_boundary = titik_nyala + selisih;
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
  static uint32_t lsRn;
  uint32_t Tmr = millis();
  static int n;
  static int sum;
  bool x = digitalRead(D7);
//  Serial.println(x);
  if(!homing&&!digitalRead(D7)){
    homing=true;
    target = batas_bawah;
    digitalWrite(D2, HIGH);
  }
  
  if(en_cmd||!digitalRead(D1)){
    en = true;
  }else{
    en = false;
  }
  
  if((Tmr-lsRn)>10){
    lsRn = Tmr;
    n++;
    sum+= analogRead(A0);
    if(n>10){
      V_Arc = sum/10;
      sum=0;
      n=0;
      
      if(en){
        if(runFinish){
          if(V_Arc>up_boundary){//default 160
            int32_t temp = target-35;
            if(!(temp<batas_bawah)){
              target = temp;
              statusTHC = "1";
            }
            
          }else if(V_Arc<titik_nyala){//default 145
            int32_t temp = target+35;
            if(!(temp>0)){
              target = temp;
              statusTHC = "2";
            }
            
          }else{
            statusTHC="3";
          }
          runFinish = false;
        }
      }
      ws.textAll("enc" + String(currEncoder)+"tgt "+ target+"arc"+V_Arc+"st"+statusTHC+","+en);
    }
    
  }

  if(homing){
    if(!digitalRead(D0)){
      Serial.println("homing selesai");
      homing = false;
      currEncoder = -4000;
      target=currEncoder+400;
      digitalWrite(D2, LOW);
      delay(7);
      digitalWrite(D2, HIGH);
    }else if(currEncoder==target){
      homing = false;
      Serial.println("homing gagal");
    }
  }

  
  putar(); 

}

//void pulse_probe(){
//  if(!pulse){
//    return;
//  }
//  static uint32_t lsRn;
//  uint32_t Tmr = millis();
//  if((Tmr-lsRn)>500){
//    
//  }
//}


void putar(){
  if(target==currEncoder||target>0||target<batas_bawah){
    runFinish = true;
    return;
  }
//  Serial.println(currEncoder);
  uint32_t Tmr = micros();
  static uint32_t lsRn;
  static bool cycle=0;
  static bool dir;
  static uint32_t pos;
  if((target-currEncoder)>0){
    dir=0;
  }else{
    dir=1;
  }
  if(cycle==0){
    digitalWrite(D6, dir);
    cycle = 1;
    pos = 0;
    lsRn = micros();
  }
  
  if((micros()-lsRn)>1000){
//    lsRn = Tmr;
    if(dir){
      currEncoder--;
    }else{
      currEncoder++;
    }
    if(currEncoder>0){
      currEncoder=0;
    }
    cycle=0;
    lsRn=micros();
//    Serial.println(currEncoder);
  }else if((micros()-lsRn)>500){
    digitalWrite(D5, HIGH);
//    Serial.println("kerjakan 2");
  }else if((micros()-lsRn)>0){
    digitalWrite(D5, LOW);
//    Serial.println("kerjakan 3");
  }

  
}


void charToString(char S[], String &D)
{
 String rc(S);
 D = rc;
}
