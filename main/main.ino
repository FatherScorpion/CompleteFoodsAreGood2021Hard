
//---------------------------------------------------//
//  AE-TSL2572 <-> Arduino UNO
//      Vin    <->    5V
//      GND    <->    GND
//      SCL    <->    A5
//      SDA    <->    A4
//---------------------------------------------------//
// J1 : ショート
//---------------------------------------------------//

#include <SPI.h>
#include "Arduino.h"
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include "image.h"
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include "AE_TSL2572.h"
AE_TSL2572 TSL2572;

#define SENSOR_ADRS   0x40              // GP2Y0E03のI2Cアドレス
#define DISTANCE_ADRS 0x5E              // Distance Value のデータアドレス

//ゲイン　0～ 5 (x0.167 , x1.0 , x1.33 , x8 , x16 , x120)
byte gain_step = 0;

//積分時間のカウンタ(0xFFから減るごとに+2.73ms)
//0xFF：  1サイクル(約 2.73ms)
//0xDB： 37サイクル(約  101ms)
//0xC0： 64サイクル(約  175ms)
//0x00：256サイクル(約  699ms)
byte atime_cnt = 0xFF;
bool shooting=false;
bool reloading=false;

int type = -1;
int backColor = 0;
int textColor = 0;
int magazineCapacity = 30;
double reloadDist = 4;

int befBullets = 30;
int currentBullets = 30;

const char SSID[] = "APEX-COMP"; // ESP32ap
const char PASS[] = "kanzen"; // 12345678

int sikii=100;

UWORD colors[] = {
  WHITE,
  BLACK,
  BLUE,
  MAGENTA,
  YELLOW,
  CYAN,
  RED,
  GREEN
};

WebServer server(80);

SemaphoreHandle_t xMutex = NULL; 

void drawText(int bullets){
  String str;
  char c[256];
  static int befBullets;
  if(bullets==magazineCapacity) Paint_Clear(colors[backColor]);
  switch(type){
    case 0:
      if(bullets<10){
        str="00"+String(bullets);
      }else if(bullets<100){
        str="0"+String(bullets);
      }else{
        str=String(bullets);
      }
      strcpy(c, str.c_str());
      Paint_DrawString_EN(95, 110, c,  &Font24,  colors[backColor], colors[textColor]);
      break;

    case 1:
      if(bullets==magazineCapacity){
        Paint_DrawRectangle(20,110,220,130,colors[textColor],DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
        befBullets=magazineCapacity;
      }else{
        if(magazineCapacity==0) break;
        int befX=20+((200*befBullets)/magazineCapacity);
        int nowX=20+((200*bullets)/magazineCapacity);
        Paint_DrawRectangle(nowX,112,befX,130,colors[textColor],DOT_PIXEL_1X1,DRAW_FILL_FULL);
        befBullets=bullets;
      }
      break;
      
    default:
      Paint_DrawString_EN(78, 110, "ERROR",&Font24,  colors[backColor], colors[textColor]);
  }
  if(bullets==0){
    Paint_DrawString_EN(78, 140, "EMPTY",&Font24,  colors[backColor], colors[textColor]);
  }
}

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

void handleDistance(){
  DynamicJsonDocument doc(200);
  double dist = checkDist();
  doc["distance"] = dist;
  
  char json_string[255];
  serializeJson(doc, json_string, sizeof(json_string));

  Serial.print("distance:");
  Serial.println(dist);

  server.send(200, "application/json", json_string);
}

void handleUpdate(){
  sikii=TSL2572.GetAdc0();
  Serial.print("sikii: ");
  Serial.println(sikii);
  
  DynamicJsonDocument doc(500);
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
  drawText(magazineCapacity);
  if (xSemaphoreTake( xMutex, ( portTickType ) 100 ) == pdTRUE) {
    currentBullets = magazineCapacity;
    xSemaphoreGive(xMutex);
  }
  befBullets=magazineCapacity;
  reloadDist = doc["reloadDistance"]; // 10

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

void watchFire(){
  int num=TSL2572.GetAdc0();
  //Serial.println(num);
  if(num>=sikii+30){
    if(!shooting){
        if(currentBullets>0){
          currentBullets--;
        }
        Serial.print("Fire: ");
        Serial.println(currentBullets); 
        Serial.print("num: ");
        Serial.println(num);
        shooting=true;
    }
  }else{
    shooting=false;  
  }
}

void subProcess(void * pvParameters) { //描画用のサブプロセス
  while(1){
    if(type!=-1){
      if (xSemaphoreTake( xMutex, ( portTickType ) 100 ) == pdTRUE) {
        if(currentBullets!=befBullets){
          int tmp=currentBullets;
          xSemaphoreGive(xMutex);
          befBullets=tmp;
          drawText(tmp);
        }else{
          xSemaphoreGive(xMutex);
        }
      }
    } 
    delay(100);
  }
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
  server.on("/getDistance", HTTP_GET, handleDistance);
  server.begin();

  Serial.println("http Server Setup Complete.");

  Config_Init();
  LCD_Init();
  LCD_SetBacklight(1000);
  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, BLACK);
  Paint_Clear(GRAY);
  Paint_DrawCircle(120,120, 120, BLUE ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
  Paint_DrawLine  (120, 0, 120, 12,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  Paint_DrawLine  (120, 228, 120, 240,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  Paint_DrawLine  (0, 120, 12, 120,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  Paint_DrawLine  (228, 120, 240, 120,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  Paint_DrawImage(gImage_70X70, 85, 25, 70, 70); 
  Paint_DrawString_EN(10,140, "Complete Foods",   &Font20,CYAN,  MAGENTA);
  Paint_DrawString_EN(95,155, "Are Good.", &Font20,CYAN, MAGENTA);
  Paint_DrawString_EN(123, 123, "2021~2022",&Font16,  BLACK, GREEN);
  Paint_DrawLine  (120, 120, 70, 70,YELLOW ,DOT_PIXEL_3X3,LINE_STYLE_SOLID);
  Paint_DrawLine  (120, 120, 176, 64,BLUE ,DOT_PIXEL_3X3,LINE_STYLE_SOLID);
  Paint_DrawLine  (120, 120, 120, 210,RED ,DOT_PIXEL_2X2,LINE_STYLE_SOLID); 
  Paint_Clear(colors[backColor]);
  Paint_DrawString_EN(78, 110, "READY",&Font24,  colors[backColor], colors[textColor+1]);
  Serial.println("LCD done");

  xMutex = xSemaphoreCreateMutex();
  xSemaphoreGive(xMutex);
  xTaskCreatePinnedToCore(subProcess, "subProcess", 4096, NULL, 1, NULL, 0); //Core 0でタスク開始
}

double checkDist(){
  double  ans  ;
  byte c[2] ;

  Wire.beginTransmission(SENSOR_ADRS) ;        // 通信の開始処理
  Wire.write(DISTANCE_ADRS) ;                  // 距離値を格納したテーブルのアドレスを指定する
  ans = Wire.endTransmission() ;               // データの送信と終了処理
  if (ans == 0) {
       ans = Wire.requestFrom(SENSOR_ADRS,2) ; // GP2Y0E03にデータ送信要求をだし、2バイト受信する
       c[0] = Wire.read()  ;                   // データの11-4ビット目を読み出す
       c[1] = Wire.read()  ;                   // データの 3-0ビット目を読み出す
       ans = ((c[0]*16.0+c[1]) / 16.0) / 4.0 ;       // 取り出した値から距離(cm)を計算する
       //Serial.print(ans) ;                     // シリアルモニターに表示させる
       //Serial.println("cm") ;
  } else {
       Serial.print("ERROR NO.=") ;            // GP2Y0E03と通信出来ない
       Serial.println(ans) ;
  }
  return ans;
}

void loop() {
  if(checkDist()<=reloadDist+2){
    if(reloading){
     if (xSemaphoreTake( xMutex, ( portTickType ) 100 ) == pdTRUE) {
       currentBullets = magazineCapacity;
       xSemaphoreGive(xMutex);
     }
     reloading=false;
     Serial.println("RELORD");
    }
  }else{
    reloading=true;
  }
  server.handleClient();
  //自動ゲイン調整
  TSL2572.SetGainAuto();

  watchFire();
  //checkDist();
}
