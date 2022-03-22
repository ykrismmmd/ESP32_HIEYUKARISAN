#include "stdlib.h" //一般ユーティリティに関する型、マクロ、関数宣言、定義ライブラリ読み込み
#include "Arduino.h" //esp32だけどarduinoのライブラリ活用
//#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h" //DFPlayerのライブラリ読み込み
#include "Time.h" //時刻を取り扱うライブラリ読み込み
#include "WiFi.h" //WiFiライブラリ読み込み
#include "WiFiMulti.h" //WfFiMultiライブラリ読み込み
#include "SD.h" //TFカード取り扱いのためライブラリ読み込み
#include "FS.h"
#include "SPI.h" //SPI通信用ライブラリ

//SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
HardwareSerial myHardwareSerial(1);
HardwareSerial Serial_df(2);
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value); //8ビットの符号なし整数型

/*******************************************************************
変数宣言
********************************************************************/
double photoanalogValue; //フォトトランジスタアナログ変換した値を保持する変数
struct tm timeInfo; //時刻を格納するオブジェクト
char s[20]; //時刻文字格納用
long brighttimes; //明るさ継続カウント用
int randamno; //音声ランダム再生用
//short sdfileall;//SDカード内全ファイル数格納用
long sdfolderco; //SDカード内再生フォルダ番号格納用
long sdfileco; //SDカード内再生ファイル番号格納用
long brightjudgespan; //明るさ判定処理間隔時間設定
long voicelength; //音声再生後待ち時間　最長の音声の時間に合わせて調整

/********************************************************************
無線LAN設定
********************************************************************/

WiFiMulti wifiMulti;//wifimultiから関数呼び出し

bool wifiConfigWithSD(fs::FS &fs, const char *path) {
  //wifiConfigWithSD(SD, "/wifi.txt"); の"/wifi.txt"という文字列をポインタで定義
  //pathを指定したら、変数pathの中身を見るのではなく、*pathで指定されている/wifi.txtを直接参照する
  //File file = fs.open(path, "r");

  SD.begin(); //TFカード通信開始

  File file = fs.open(path);
  if (!file) { //ファイルエラーが有る場合
    log_e("Can't Open File %s", path);
    Serial.println(F("wifi設定ファイルを開けませんでした"));
    return false;
  }
  while (file.available()) {
    String line = file.readStringUntil('\n'); //改行"\n"で1行終了
    line = line.substring(0, line.indexOf('#')); //0文字目が#だったら以降を変数lineに格納する
    String ssid = line.substring(0, line.indexOf(' ')); ssid.trim(); //C++ 0文字目から半角スペースまでの文字を切り出す
    String password = line.substring(line.indexOf(' ')); password.trim(); //c++ 半角スペースから始まる文字列を切り出す
    if (ssid.length() != 0) {
      log_d("WiFiMulti += SSID: %s", ssid.c_str());
      Serial.println(F("SSIDの読み込みに成功しました"));
      wifiMulti.addAP(ssid.c_str(), password.c_str()); //WifimultiモジュールのaddAPにSSIDとpasswordを渡す
    }
  }

  WiFi.mode(WIFI_STA); //Wifi子局モード

  log_i("WiFiMulti Connecting...");
  Serial.println(F("WiFiMulti接続試行中"));

  if (wifiMulti.run() != WL_CONNECTED) {
    log_e("WiFi Connection Failed :(");
    Serial.println(F("WiFi接続に失敗しました"));
    return false;
  }
  log_i("WiFi Connected :)");
  Serial.println(F("Wifi接続に成功しました"));
  log_i("SSID: %s\tLocal IP: %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
  
  file.close();
  return true;
}

/******************************************************************************************************************
セットアップ
*******************************************************************************************************************/
void setup()
{
  //USB シリアル通信設定
  Serial.begin(115200);//USBシリアルポート速度設定
  Serial_df.begin(9600);//TFカード通信速度設定

  //ピン設定
  pinMode(A6,INPUT); //ADC6(D34)を入力へ　フォトトランジスタ用　NJL7502L
  analogSetAttenuation(ADC_11db); //内蔵アッテネータを11dBに、測定範囲0-3.3V

  /*あかねちゃんやで*/

  wifiConfigWithSD(SD, "/wifi.txt"); //TFからwifi情報読み込み、接続

  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");//NTPの設定
  Serial.println(F("NTP時刻同期を行いました"));

/*DFPlayemini設定*************************************************************************************************/
  //DFplayer mini とのシリアル通信設定
  myHardwareSerial.begin(9600, SERIAL_8N1, 14, 12); //14番ピンがRX,12番ピンがTX

  //USBシリアルにメッセージ印字
  Serial.println();
  //Serial.println(F("DFRobot DFPlayer Mini Demo")); //(F(を指定するとSRAMじゃなくてフラッシュメモリに格納する
  Serial.println(F("DFPlayerを開始しています (3~5secかかることがあります)"));

  //DFPlayerエラーチェック
  //if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
  if (!myDFPlayer.begin(myHardwareSerial)) {  //!は論理否定　Use softwareSerial to communicate with mp3.
    Serial.println(F("開始できませんでした"));
    Serial.println(F("1.接続を確認してください"));
    Serial.println(F("2.TFカードを挿入してください"));
    while (true) {
    //while(){}
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Miniの起動に成功しました")); //dfplayer起動成功

  myDFPlayer.volume(25);  //音量設定. From 0 to 30

/**************************************************************************************************************/

}

/****************************************************
 メイン処理
 ****************************************************/
void loop()
{
  timeview(); //現在内部時刻表示
  akarusa(); //明るさ取得処理
  voiceplay();
  //random1(); //ランダム値印字処理（実験用）

  /*DFPlayerバッファ初期化 入れないとmyDFPlayer.playFolderファイル数が-1と表示される*/
  myDFPlayer.readType(); //DFPlayerバッファ初期化
  myDFPlayer.read(); //DFPlayerバッファ初期化

  //delay(5000);
}

/****************************************************
 ランダム値印字処理（実験用）
 ****************************************************/
void random1(){
  /*ランダムテスト*/
  srand((unsigned int)time(NULL)); //乱数を時刻によって初期化
  randamno = rand() % 256; //0～255の乱数を発生
  Serial.println(F("ランダム値"));
  Serial.println(randamno); 
  //randamno = strtol(randamno, NULL, 16); //16進数へ変換
  //Serial.println(randamno);
}

/****************************************************
 明るさ取得処理
 ****************************************************/
void akarusa(){
  /*ホトトランジスタ出力電圧取得・シリアルへ出力*/
    photoanalogValue = analogRead(A6);//ADC6のアナログ変換値を取得
    Serial.println(F("ホトトランジスタ変換値"));
    Serial.println(photoanalogValue);//アナログ変換値を出力
    //delay(200);//200ms感覚でシリアル出力
}


/****************************************************
 ボイス再生処理
 ****************************************************/
void voiceplay(){
  brightjudgespan = 1000; //明るさ判定処理間隔時間設定
  voicelength = 5000;  //音声再生後待ち時間　最長の音声の時間に合わせて調整

  Serial.println(timeInfo.tm_hour);
  Serial.println("時台です");

    if (photoanalogValue > 3000){ //明るさ判定
      if (brighttimes < 1){//明るい継続判定0回なら処理

        sdfileco = myDFPlayer.readFileCountsInFolder(timeInfo.tm_hour);//当該時刻名のフォルダ内にあるファイル数を取得
        Serial.println(F("ファイル数は"));
        Serial.println(sdfileco);

        delay(100); //Serial.println(sdfileco);とSerial.println(randamno);の印字入れ替わり防止

        srand((unsigned int)time(NULL)); //ランダム関数を時刻によって初期化
        randamno = rand() % sdfileco; //0～sdfileco間のランダムな値を生成
        Serial.println(F("ランダム値は"));
        Serial.println(randamno);

        delay(100);

        myDFPlayer.playFolder(timeInfo.tm_hour, randamno); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
        myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる
        Serial.println(F("明るさ検知1回目なので音声再生しました"));

        delay(voicelength); //n秒以内は次の判定しない

      }else if(brighttimes == 5){
        myDFPlayer.playFolder(90, 0); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
        myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる

        Serial.println(F("明るさ検知n回目なので音声再生しました"));
        Serial.println(F("明るさ継続回数"));
        Serial.println(brighttimes);

        delay(voicelength); //n秒以内は次の判定しない

      } else {

        Serial.println(F("明るさ検知2回目以上なので音声再生しませんでした"));
        Serial.println(F("明るさ継続回数"));
        Serial.println(brighttimes);

        delay(brightjudgespan); //n秒以内は次の判定しない

      }
    
      brighttimes = brighttimes + 1;
      Serial.println(F("明るさ継続回数"));
      Serial.println(brighttimes);

    } else if(photoanalogValue < 3000) {//photologValueが基準値を下回ったら再生停止
      myDFPlayer.stop();
      brighttimes = 0;
      Serial.println(F("明るさ継続回数"));
      Serial.println(brighttimes);
      delay(500); //n秒以内は次の判定しない

    }else{
      
    }
}

/****************************************************
 現在内部時刻表示
 ****************************************************/
void timeview(){
    //configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");//NTPの設定
    getLocalTime(&timeInfo);//tmオブジェクトのtimeInfoに現在時刻を入れ込む
    sprintf(s, " %04d/%02d/%02d %02d:%02d:%02d",
            timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday,
            timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);//人間が読める形式に変換
    //delay(1000);
    Serial.println(F("現在内部時刻は"));//
    Serial.println(s);//時間をシリアルモニタへ出力
    Serial.println(F("です"));
}

/****************************************************
 DFPlayer用エラーメッセージ
 ****************************************************/
void printdfpDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

}
