// Replace with your network credentials
//const char* ssid = "SII-Cikunir";
//const char* password = "admin.admin";




// Sebagai AccessPoint
const char* ssid = "THC"; //kalau gagal konek
const char* password = "12345678";
String ip;

IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress netmask(255, 255, 255, 0);

void wifiConnect(){
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, netmask);
//    WiFi.softAP(configadmin.ssid, configadmin.password);
  WiFi.softAP(ssid, password);
  
//    digitalWrite(pin_led, LOW);

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("IP: ");
  Serial.println(local_ip);
  ip = "192.168.4.1";
}

//g01 f1000 y500
