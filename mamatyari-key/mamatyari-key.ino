#include "CheapStepper.h"
#include "FPS_GT511C3.h"

// ステップモータの端子を設定
CheapStepper stepper (8, 9, 10, 11);
// 指紋認証デバイスの端子を設定
FPS_GT511C3 fps(4, 5);

// モーターの回転方向
boolean moveClockwise = true;

// 指紋認証デバイスのwaked-upとして動作する線を接続する端子
// 指が触れたとき2.64V以上が返ってくるのでアナログ端子で読み込み
int touchPin = A0;

/* ----- 起動時の初期化 ----- */
void setup() {
  // ステップモータの速度を設定
  stepper.setRpm(20);
  // 指紋認証デバイスの初期化
  fps.Open();
  fps.SetLED(false);
}

/* ----- メインループ ----- */
void loop() {
  // 指紋認証デバイスに指が触れたとき反応する端子を読み取り
  int touchSensorValue = analogRead(touchPin);
  // 指が触れていたら認証開始
  if (touchSensorValue > 512) {
    if (verifyFingerPrint()) {
      openKey();
    }
  }
  // 一秒間スリープ
  delay(1000);
}

/* ----- 指紋認証を実行する関数 ----- */
bool verifyFingerPrint() {
  bool valid = false;
  // 指紋認証デバイスのLEDをつける
  fps.SetLED(true);
  if (fps.IsPressFinger()) {
    // 指紋を読み取り
    fps.CaptureFinger(false);
    // 指紋の分類
    int id = fps.Identify1_N();
    // 指が最大200本まで登録される想定で条件分岐
    if (id < 200) {
      valid = true;
    }
    else {
      valid = false;
    }
  }
  // 指紋認証デバイスのLEDを消す
  fps.SetLED(false);
  // 認証に成功した場合trueを返す
  return valid;
}

/* ----- 鍵を開けるためにステップモータを動作させる関数 ----- */
bool openKey() {
  // 回転方向を左向きに設定
  moveClockwise = true;
  // 半回転させる
  stepper.move(moveClockwise, 2048);
  // 回転方向を右向きに設定
  moveClockwise = false;
  // 1/4回転戻す(鍵を閉められる状態にするため)
  stepper.move(moveClockwise, 1792);
  // モーターの電源を切る
  /* 一応CheapStepperにはライブラリから電源切れるようにするプルリクが
  出ていたのでそのうちsetepper.off()みたいな感じで電源きれるようになりそう */
  digitalWrite(8, 0);
  digitalWrite(9, 0);
  digitalWrite(10, 0);
  digitalWrite(11, 0);
} 
