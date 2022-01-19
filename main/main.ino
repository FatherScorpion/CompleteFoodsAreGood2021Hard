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

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
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
    while (1) {}
  }
}

void watchFire(){3
  if(TSL2572.GetAdc0()>50){
    if(!shooting){
        Serial.println("Fire"); 
        shooting=true;
    }
  }else{
    shooting=false;  
  }
}

void loop() {
  //TSL2572.GetLux16()で照度を取得
  //TSL2572.GetLux16();
  //Serial.print(TSL2572.GetAdc0(), DEC);
  //Serial.println("Lx");
  watchFire();
  //自動ゲイン調整
  TSL2572.SetGainAuto();

  digitalWrite(LED_BUILTIN, HIGH);
}
