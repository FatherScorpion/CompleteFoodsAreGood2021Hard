//---------------------------------------------------//
//  AE-TSL2572 <-> Arduino UNO
//      Vin    <->    5V
//      GND    <->    GND
//      SCL    <->    A5
//      SDA    <->    A4
//---------------------------------------------------//
// J1 : ショート
//---------------------------------------------------//

#include <Wire.h>
#include "AE_TSL2572.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h>
AE_TSL2572 TSL2572;

const char* ssid = "APEX-COMP";
const char* password = "kanzen";

WiFiServer server(80);;
int type = 0; // デザインタイプ
int magazineCapacity = 30; // マガジン容量
int textColor = 0; // 文字色
int backColor = 0; // 背景色
double reloadDist = 10; // リロード検知距離

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

//ゲイン　0～ 5 (x0.167 , x1.0 , x1.33 , x8 , x16 , x120)
byte gain_step = 0;

//積分時間のカウンタ(0xFFから減るごとに+2.73ms)
//0xFF：  1サイクル(約 2.73ms)
//0xDB： 37サイクル(約  101ms)
//0xC0： 64サイクル(約  175ms)
//0x00：256サイクル(約  699ms)
byte atime_cnt = 0xFF;
bool shooting=false;

void setup() {
  Serial.begin(115200);

  Wire.begin();
  if (TSL2572.CheckID()) {
    //ゲインを設定
    TSL2572.SetGain(gain_step);
    //積分時間を設定
    TSL2572.SetIntegralTime(atime_cnt);

    //計測開始
    TSL2572.Reset();
    delay(100);
  }
  else {
    Serial.println("Failed. Check connection!!");
  }

  WiFi.softAP(ssid,password);
  delay(100);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //
  //サーバーセットアップ
  //
  /* Set up mDNS */
  if (!MDNS.begin("esp32")) {
      Serial.println("Error setting up MDNS responder!");
      while(1) {
          delay(1000);
      }
  }
  Serial.println("mDNS responder started");
  /* Start Web Server server */
  server.begin();
  Serial.println("Web server started");

  /* Add HTTP service to MDNS-SD */
  MDNS.addService("http", "tcp", 80);
}

void watchFire(){
  if(TSL2572.GetAdc0()>25){
    if(!shooting){
        Serial.println("Fire"); 
        shooting=true;
    }
  }else{
    shooting=false;  
  }
}

void handleClient(){
  WiFiClient client = server.available();
    if (!client) {
        return;
    }
    Serial.println("");
    Serial.println("New client");
    if (client) {                   
      Serial.println("new client");         
      /* check client is connected */           
      while (client.connected()) {     
          /* client send request? */     
          if (client.available()) {
              /* request end with '\r' -> this is HTTP protocol format */
              String req = client.readStringUntil('\r');
              /* First line of HTTP request is "GET / HTTP/1.1"  
                here "GET /" is a request to get the first page at root "/"
                "HTTP/1.1" is HTTP version 1.1
              */
              /* now we parse the request to see which page the client want */
              int addr_start = req.indexOf(' ');
              int addr_end = req.indexOf(' ', addr_start + 1);
              if (addr_start == -1 || addr_end == -1) {
                  Serial.print("Invalid request: ");
                  Serial.println(req);
                  return;
              }
              req = req.substring(addr_start + 1, addr_end);
              Serial.print("Request: ");
              Serial.println(req);
              client.flush();
          
              String s;
              /* if request is "/" then client request the first page at root "/" -> we process this by return "Hello world"*/
              if (req == "/")
              {
                  IPAddress ip = WiFi.localIP();
                  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
                  s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP32 at ";
                  s += ipStr;
                  s += "</html>\r\n\r\n";
                  Serial.println("Sending 200");
              }
              else
              {
                  /* if we can not find the page that client request then we return 404 File not found */
                  s = "HTTP/1.1 404 Not Found\r\n\r\n";
                  Serial.println("Sending 404");
              }
              /* send response back to client and then close connect since HTTP do not keep connection*/
              client.print(s);
              client.stop();
          
            }
        }          
    }
    Serial.println("Done with client");
}

void loop() {
  //TSL2572.GetLux16()で照度を取得
  //TSL2572.GetLux16();
  //Serial.print(TSL2572.GetAdc0(), DEC);
  //Serial.println("Lx");
  watchFire();
  //自動ゲイン調整
  TSL2572.SetGainAuto();
  handleClient();
}
