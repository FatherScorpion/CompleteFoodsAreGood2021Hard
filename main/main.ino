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
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "AE_TSL2572.h"
AE_TSL2572 TSL2572;

//ゲイン　0～ 5 (x0.167 , x1.0 , x1.33 , x8 , x16 , x120)
byte gain_step = 0;

//積分時間のカウンタ(0xFFから減るごとに+2.73ms)
//0xFF：  1サイクル(約 2.73ms)
//0xDB： 37サイクル(約  101ms)
//0xC0： 64サイクル(約  175ms)
//0x00：256サイクル(約  699ms)
byte atime_cnt = 0xFF;
bool shooting=false;

int type = 0;
int backColor = 0;
int textColor = 0;
int magazineCapacity = 30;
double reloadDist = 0;

const char SSID[] = "APEX-COMP"; // ESP32ap
const char PASS[] = "kanzen"; // 12345678

WebServer server(80);

void handleTest(){ // ArduinoJsonのテスト
  DynamicJsonDocument doc(200);
  String body = server.arg("plain");
  Serial.println(body);
  deserializeJson(doc, body);

  const char* json_text = doc["hoge"];
  Serial.println(json_text);
  json_text = doc["tibi"];
  Serial.println(json_text);
  
  char json_string[255];
  serializeJson(doc, json_string, sizeof(json_string));
  
  server.send(200,"application/json",json_string);
}

void handleUpdate(){ // ArduinoJsonのテスト
  DynamicJsonDocument doc(200);
  String input = server.arg("plain");
  Serial.println(input);
  DeserializationError error = deserializeJson(doc, input);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    server.send(400,"text/plain",error.c_str());
    return;
  }

  type = doc["type"]; // 1
  backColor = doc["backColor"]; // 1
  textColor = doc["textColor"]; // 1
  magazineCapacity = doc["magazineCapacity"]; // 30
  reloadDist = doc["reloadDist"]; // 10

  Serial.print("type:");
  Serial.println(type);
  Serial.print("textColor:");
  Serial.println(textColor);
  Serial.print("backColor:");
  Serial.println(backColor);
  Serial.print("magazineCapacity:");
  Serial.println(magazineCapacity);
  Serial.print("reloadDist:");
  Serial.println(reloadDist);
  
  server.send(200,"text/plain","success");
}

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

  WiFi.softAP(SSID, PASS);
  delay(100);

  IPAddress ip=WiFi.softAPIP();
  Serial.println("Wi-Fi start.");
  Serial.printf("SSID: ");
  Serial.println(SSID);
  Serial.printf("PASS: ");
  Serial.println(PASS);
  Serial.printf("IPAddress: ");
  Serial.println(ip);

  server.on("/", HTTP_POST, handleTest);
  server.on("/update", HTTP_POST, handleUpdate);
  server.begin();

  Serial.println("http Server Setup Complete.");
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

void loop() {
  server.handleClient();
  //TSL2572.GetLux16()で照度を取得
  //TSL2572.GetLux16();
  //Serial.print(TSL2572.GetAdc0(), DEC);
  //Serial.println("Lx");
  watchFire();
  //自動ゲイン調整
  TSL2572.SetGainAuto();
}
