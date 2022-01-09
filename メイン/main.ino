void setup() {
  pinMode(12,INPUT);
  Serial.begin(9600);
}
ボイドAE_TSL2572::SetGain(uint8_t a) {
gain_step = a;
}

ボイドAE_TSL2572::ライトゲイン() {
  スイッチ(gain_step) {
    ケース0x00:// 1/6
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONTROL、0x00);
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONFIG、0x04);
再び = 0.1667;
      壊す;
    ケース0x01:// x1
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONTROL、0x00);
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONFIG,0x00);
再び = 1.0;
      壊す;
    ケース0x02:// x8/6
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONTROL、0x01);
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONFIG、0x04);
再び = 1.333;
      壊す;
    ケース0x03:  // x8
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONTROL、0x01);
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONFIG,0x00);
再び = 8.0;
      壊す;
    ケース0x04:  // x16
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONFIG,0x00);
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONTROL、0x02);
再び = 16.0;
      壊す;
    ケース0x05:  // x120
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONFIG,0x00);
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONTROL、0x03);
再び = 120.0;
      壊す;
    デフォルト:  //設定 x1
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONTROL、0x00);
      書き込み (TSL2572_COMMAND |TSL2572_TYPE_INC |TSL2572_CONFIG,0x00);
再び = 1.0;
      壊す;
  }
}

void loop() {
  int value = digitalRead(12);
  Serial.println(value);
  delay(100);
}


