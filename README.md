# Bp35c0_broute
ESP32(ESP-WROOM-32) Arduino IDE用  
ロームCP35C0を使用したBルート電力取得用ライブラリです。  
シンプルに、定時積算電力量の取得に特化しています。  

# version
version 0.1

# ライセンス  
License MIT

# 使い方
* このライブラリを Arduino IDE にインクルードしてください。
* 以下の通り BP35C0-T01とESP32を接続してください。
  * BP35C0-T1 
    * CN1
      * 1 GNDと接続
      * 2 空
      * 3 空
      * 4 ESP32の3V3と接続
      * 5 ESP32の3V3と接続
      * 6 空
      * 7 GNDと接続
      * 8 GNDと接続
      * 9 GNDと接続
    * CN2
      * 1 GNDと接続
      * 2 空
      * 3 空
      * 4 ESP32のUART TXと接続(ex. GPIO 17 に接続)
      * 5 ESP32のUART RXと接続(ex. GPIO 16 に接続)
      * 6 空
      * 7 ESP32の3V3と接続
      * 8 空
      * 9 GNDと接続
    * CN3
      * すべて空
* Arduino IDE からスケッチ例を参照してください。

# 注意事項

* ESP32(ESP-WROOM-32)でのみ動作確認しています。
* BP35A1 では動きません（ちょっと手直しすれば動くようになるはず）

# Bp35c0_broute ライブラリ説明

## コンストラクタ

* Bp35c0_broute(Stream &uart)
  コンストラクタ。引数に BP35C0に接続しているUART(HardwareSerial)のインスタンスを渡します。

## public関数

* bool available(void)  
  BP35C0 が使用できるかをチェックします。  
  * 戻り値
    * true=使用可能
    * false=使用不可能

* bool open(const char *b_route_id, const char *b_route_pass)  
  BP35C0 をオープンし、スマートメーターからのデータを受信できる状態にします。  
  * 引数:  
    * b_route_id=Bルートサービス用ID
    * b_route_pass=Bルートサービス用パスワード  
   (※電力会社から受領したもの)  
  * 戻り値
    * true=正常終了
    * false=異常

* void waitForRecieve(char *downsideDatetime, float *downsidePower, char *upsideDatetime, float *upsidePower)  
  スマートメーターから積算消費電力のデータを受信します。この関数は、データ受信するまで戻ってきません。  
  * 引数: データを取得するためのポインタ  
    * downsideDatetime=正方向計測日時の文字列 (YYYYMMDDHHMMSS)  
      （※) 十分な領域をあらかじめ確保しておいてください。
    * downsidePower=積算電力量(kWh)  
    * upsideDatetime=逆方向計測日時の文字列(YYYYMMDDHHMMSS)   
      （※) 十分な領域をあらかじめ確保しておいてください。
    * upsidePower=積算電力量(kWh)  
      

* bool close(void)  
  BP35C0 をクローズします。  
  * 戻り値
    * true=正常終了
    * false=異常  

* bool isOpen(void)  
  BP35C0 がオープンされているか？  
  * 戻り値
    * true=オープンされている
    * false=オープンされていない  

* void setDebugSerial(Stream &debug)  
  引数にSerialオブジェクトを渡すと、そのSerialにデバッグ用の出力を行うようになります。  

# リンク
* BP35C0  
  https://www.rohm.co.jp/products/wireless-communication/specified-low-power-radio-modules/bp35c0-product

* BP35C0-T01 仕様書  
  https://www.rohm.co.jp/documents/11546/4947397/bp35C0-T01_hardware_spec_v100_j.pdf

* ECHONET Lite規格書  
  https://echonet.jp/spec_v112_lite/

# 参考にしたサイト
以下のサイトを参考にさせていただきました（超感謝）
* Wi-SUN対応無線モジュール（BP35C0-T01）とラズパイを繋ぐ。  
  https://qiita.com/katsumin/items/acd567e9b8e81c691ed4

* スマートメーターとRaspberry Piで電力計を作った  
  http://chappnet.hateblo.jp/entry/2015/08/23/112738


# history
* ver. 0.1: 初回公開

#メモ
