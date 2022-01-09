void setup() {
  pinMode(12,INPUT);
  Serial.begin(9600);
}
ボイドAE_TSL2572::SetGain(uint8_t a) {
gain_step = a;
}

ボイドループ() {
  int値 =デジタル読み取り(12 ));
シリアル。出力 (値)
  遅延(100);
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



# 「アルドゥイノ.h」が含まれます。
# <Wire.h>を含む

クラスAE_TSL2572 {
  公共：
    AE_TSL2572();
    無効リセット();
    無効なセットゲイン(uint8_t gain_step);
    ボイドセットゲインオート();
    無効なセットリミット();
    無効セット整数時(uint8_t atime_cnt);
    ブールチェックID();
    フロートゲットラックス();
    uint16_t・ゲラックス16();
    uint8_tゲットゲインステップ();
    uint8_t統合時間を取得します。();

  プライベート：
    無効読取りAdc();
     uint8_t・リードレグ(uint8_t・レグ);
    void WriteReg(uint8_t reg、uint8_tダット)。 
    無効書き込み関数の時間();
    ボイドライトゲイン();
    フロートカルクラックス();

    //ルクスを計算するために使用
    uint16_t adc0, adc1;
    uint8_t時間;
    再びフロート;

    設定に使用
     uint8_t atime_cnt、gain_step;

    オートゲインに使用
    limit_lower,limit_upper uint16_t。
    フロートcoefficient_lower = 0.05;
    浮動小数点coefficient_upper = 0.75;

    //TSL2572 レジスタ セット
    const uint8_t TSL2572_ADR      = 0x39;
    定数 uint8_t TSL2572_COMMAND = 0x80;
    const uint8_t TSL2572_TYPE_REP = 0x00;
    const uint8_t TSL2572_TYPE_INC = 0x20;
    const uint8_t TSL2572_ALSIFC   = 0x66;

    const uint8_t TSL2572_SAI   = 0x40;
     uint8_t TSL2572_AIEN = 0x10;
    const uint8_t TSL2572_WEN   = 0x80;
    const uint8_t TSL2572_AEN   = 0x02;
    const uint8_t TSL2572_PON   = 0x01;

    TSL2572_ENABLE = 0x00 uint8_t定数;
    const uint8_t TSL2572_ATIME    = 0x01;
    const uint8_t TSL2572_WTIME    = 0x03;
    const uint8_t TSL2572_AILTL    = 0x04;
    const uint8_t TSL2572_AILTH    = 0x05;
    const uint8_t TSL2572_AIHTL    = 0x06;
    const uint8_t TSL2572_AIHTH    = 0x07;
    const uint8_t TSL2572_PRES     = 0x0C;
    コンスuint8_t TSL2572_CONFIG = 0x0D;
    const uint8_t TSL2572_CONTROL  = 0x0F;
    const uint8_t TSL2572_ID       = 0x12;
    const uint8_t TSL2572_STATUS   = 0x13;
    TSL2572_C0DATA = 0x14 uint8_t ;
    const uint8_t TSL2572_C0DATAH  = 0x15;
    const uint8_t TSL2572_C1DATA   = 0x16;
    定数 uint8_t TSL2572_C1DATAH = 0x17;
};
