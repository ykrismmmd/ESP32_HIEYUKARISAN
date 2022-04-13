#include "stdlib.h" //一般ユーティリティに関する型、マクロ、関数宣言、定義ライブラリ読み込み
#include "Arduino.h" //esp32だけどarduinoのライブラリ活用
//#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h" //DFPlayerのライブラリ読み込み
#include "Time.h" //時刻を取り扱うライブラリ読み込み
#include "WiFi.h" //WiFiライブラリ読み込み
#include "WiFiMulti.h" //WfFiMultiライブラリ読み込み
#include "FS.h"
#include "SPI.h" //SPI通信用ライブラリ
#include "DNSServer.h" //web画面用ライブラリ
#include "WebServer.h" //web画面用ライブラリ
#include "SPIFFS.h" //SPIFFS領域取り扱い用ライブラリ

//SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
HardwareSerial myHardwareSerial(1);
HardwareSerial Serial_df(2);
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value); //8ビットの符号なし整数型

/*******************************************************************
web画面からのWiFi設定用変数宣言
********************************************************************/
//APモードでの初期設定
IPAddress apIP(192,168,1,100);
WebServer webServer(80);
const char* WIFIMGR_ssid = "HIEYUKARI_SETTEI_ESP32"; //APモードでのSSID名
const char* WIFIMGR_pass = "xxxxxxxx"; //APモードでのパスワードWiFi.softAP(WIFIMGR_ssid)にてパスワードなしで起動するので適当な文字列でok
DNSServer dnsServer;

//クライアントモードでの初期設定
const String defaultSSID = "HIEYUKARI"; //初期SSID
const String defaultPASSWD = "HIEYUKARI"; //初期パスワード
String ssid = defaultSSID; //ライブラリ変数ssidにdefaultSSIDの値を代入
String passwd = defaultPASSWD; //ライブラリ変数passwdにdefaultPASSWDの値を代入
/*
WiFi環境がない場合は、スマートフォンでテザリングして初期設定する。
1回wifiをつながないと次の処理が走らない。
クライアントモード初期SSID、パスワードでテザリングAPを用意してもらい、つないでもらう。
1回立ち上がればwifiは切断してもよい。
ただし、再起動した場合は同じ操作が必要になる。
*/


// scan SSID
#define SSIDLIMIT 30 //ライブラリで定義された変数SSIDLIMITの値を30に設定
String ssid_rssi_str[SSIDLIMIT]; //30文字まで格納できる配列を定義
String ssid_str[SSIDLIMIT]; //30文字まで格納できる配列を定義

// SPIFFS config file名設定
const char* configfile = "/contig.txt";

/*******************************************************************
変数宣言
********************************************************************/
double photoanalogValue; //フォトトランジスタアナログ変換した値を保持する変数
struct tm timeInfo; //時刻を格納するオブジェクト
char s[20]; //時刻文字格納用
long brighttimes; //明るさ継続カウント用
int randamno; //音声ランダム再生用
long sdfolderco; //TFカード内再生フォルダ番号格納用
long sdfileco; //TFカード内再生ファイル番号格納用
long brightjudgespan; //明るさ判定処理間隔時間設定
long voicelength; //音声再生後待ち時間　最長の音声の時間に合わせて調整

/******************************************************************************************************************
セットアップ
*******************************************************************************************************************/
void setup()
{
  //USB シリアル通信設定
  Serial.begin(115200);//USBシリアルポート速度設定
  //Serial_df.begin(9600);//TFカード通信速度設定

  //ピン設定
  pinMode(A6,INPUT); //ADC6(D34)を入力へ　フォトトランジスタ用　NJL7502L
  analogSetAttenuation(ADC_11db); //内蔵アッテネータを11dBに、測定範囲0-3.3V

/*DFPlayemini設定*************************************************************************************************/
  //DFplayer mini とのシリアル通信設定
  myHardwareSerial.begin(9600, SERIAL_8N1, 14, 12); //14番ピンがRX,12番ピンがTX

  //USBシリアルにメッセージ印字
  Serial.println(); //改行
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

  myDFPlayer.playFolder(99, 0); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
  myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる

  delay(4000);//音声再生完了待ち

/**************************************************************************************************************/

  //SPIFFSマウント
  if(!SPIFFS.begin(true)){
    Serial.println(F("SPIFFSマウント失敗しました"));
    myDFPlayer.playFolder(99, 1); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
    myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる
    delay(4000);//音声再生完了待ち
  }else{
    Serial.println(F("SPIFFSマウント成功しました"));
    myDFPlayer.playFolder(99, 2); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
    myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる
    delay(4000);//音声再生完了待ち
  }

  //SPIFFS領域からreadconfig関数によりssidとpassword読み込み
  readConfigFile();
  
  //webからwifi設定接続処理
  uint8_t retry = 0;
  WiFi.begin(ssid.c_str(), passwd.c_str());
  Serial.print("WiFi接続試行中です");
  myDFPlayer.playFolder(99, 3); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
  myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる
  delay(3000);//音声再生完了待ち

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(200); // 200ms
    retry ++;
    if (retry > 50) { // 200ms x 50 = 10 sec
      Serial.println("wifi接続がタイムアウトしました。APモードに切り替えます");
      myDFPlayer.playFolder(99, 4); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
      myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる
      webconfig(); // enter webconfig
    }
  }
  Serial.println();
  Serial.printf("WiFi接続完了しました。IP address: ");
  Serial.println(WiFi.localIP());
  myDFPlayer.playFolder(99, 5); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
  myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる
  delay(3000);//音声再生完了待ち

  delay(500);

  /*あかねちゃんやで*/

  //wifiConfigWithSD(SD, "/wifi.txt"); //TFからwifi情報読み込み、接続

  //NTPによる時刻同期
  configTime(9 * 3600L, 0, "ntp.nict.jp", "time.google.com", "ntp.jst.mfeed.ad.jp");//NTPの設定
  Serial.println(F("NTP時刻同期を行いました"));
  myDFPlayer.playFolder(99, 6); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
  myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる
  delay(3000);//音声再生完了待ち

  //起動完了メッセージ
  Serial.println(F("起動完了しました"));
  myDFPlayer.playFolder(99, 7); //myDFPlayerのライブラリから、フォルダ番号指定、ファイル番号指定、10進数指定
  myDFPlayer.available();//dfplayerバッファ初期化 playFolderの直後に入れる
  delay(3000);//音声再生完了待ち
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
  Serial.print(F("ランダム値:"));
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

  Serial.print(timeInfo.tm_hour);
  Serial.println("時台です");

    if (photoanalogValue > 3000){ //明るさ判定
      if (brighttimes < 1){//明るい継続判定0回なら処理

        sdfileco = myDFPlayer.readFileCountsInFolder(timeInfo.tm_hour);//当該時刻名のフォルダ内にあるファイル数を取得
        Serial.print(F("ファイル数:"));
        Serial.println(sdfileco);

        delay(100); //Serial.println(sdfileco);とSerial.println(randamno);の印字入れ替わり防止

        srand((unsigned int)time(NULL)); //ランダム関数を時刻によって初期化
        randamno = rand() % sdfileco; //0～sdfileco間のランダムな値を生成
        Serial.print(F("ランダム値:"));
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
    Serial.print(F("現在内部時刻は"));//
    Serial.print(s);//時間をシリアルモニタへ出力
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


/***************************************************************
 * webからwifi設定関連
****************************************************************/
void webconfig() {
  
  Serial.println("Web設定モード: ");

  configserver(); //コンフィグサーバ関数呼び出し
  
  uint8_t configloop = 1;
  while (configloop == 1) {
    dnsServer.processNextRequest();
    webServer.handleClient();
  }

  WiFi.disconnect(true); //wifi切断
  WiFi.mode(WIFI_OFF); //wifi無効化
}

void configserver() {
  WiFi.disconnect(true); //wifi切断
  WiFi.mode(WIFI_OFF); //wifi無効化
  delay(100);
  WiFi.mode(WIFI_AP); //wifi APモードで起動
  WiFi.softAP(WIFIMGR_ssid); // パスワードなしで起動
//   WiFi.softAP(WIFIMGR_ssid,WIFIMGR_pass); // with password  

  delay(200); // Important! This delay is necessary 
  WiFi.softAPConfig(apIP,apIP,IPAddress(255,255,255,0)); 

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", apIP);
  
  webServer.on("/", wifimgr_top);
  webServer.on("/wifiinput", HTTP_GET, wifiinput);
  webServer.on("/wifiset", HTTP_GET, wifiset);
  webServer.on("/reboot", reboot);
  webServer.on("/doreboot", doreboot);
  webServer.onNotFound(wifimgr_top);
  webServer.begin();
}

String maskpasswd(String passwd){
  String maskpasswd = "";

  for (int i=0; i<passwd.length(); i++) maskpasswd = maskpasswd + "*";
  if (passwd.length() == 0) maskpasswd = "(null)";

  return maskpasswd;
}

void wifimgr_top() {

  String html = Headder_str();
  html += "<a href='/wifiinput'>WiFi設定開始</a>";
  html += "<hr><h3>現在の設定</h3>";
  html += "SSID: " + ssid + "<br>";
  html += "passwd: " + maskpasswd(passwd) + "<br>";
  //html += "WiFiによる時刻同期を使用しない<input type=\"checkbox\" name=\"hushiyou\" value=\"hushiyou\"><br>";
  // valueWiFiによる時刻同期を使用しない<br>";
  html += "<hr><p><center><a href='/reboot'>設定を適用して再起動</a></center>";
  html += "</body></html>";
  webServer.send(200, "text/html", html);
}

String Headder_str() {
  String html = "";
  html += "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.3'>";
  html += "<meta http-equiv='Pragma' content='no-cache'>";
  html += "<meta http-equiv='Cache-Control' content='no-cache'></head>";
  html += "<meta http-equiv='Expires' content='0'>";
  html += "<style>";
  html += "a:link, a:visited { background-color: #009900; color: white; padding: 5px 15px;";
  html += "text-align: center; text-decoration: none;  display: inline-block;}";
  html += "a:hover, a:active { background-color: green;}";
  html += "bo32{ background-color: #EEEEEE;}";
  html += "input[type=button], input[type=submit], input[type=reset] {";
  html += "background-color: #000099;  border: none;  color: white;  padding: 5px 20px;";
  html += "text-decoration: none;  margin: 4px 2px;";
  html += "</style>";
  html += "<body>"; 
  html += "<h2>WiFiマネージャ</h2>";
  return html;
}

void InitialConfigFile(){
  Serial.printf("SPIFFS initial file: %s\n", configfile);

  ssid = defaultSSID;
  passwd = defaultPASSWD;
  //hushiyou
  
  WriteConfigFile();  
}

//*******************************************
void WriteConfigFile(){

  ssid.trim();
  passwd.trim();
  
  Serial.printf("SPIFFS writing file: %s\n", configfile);
  File fw = SPIFFS.open(configfile, "w");
  fw.println(ssid);
  fw.println(passwd);
  fw.close();

  delay(100);
}

//*******************************************
void readConfigFile(){
  String numstr;
  Serial.printf("SPIFFS reading file: %s\n", configfile);
  File fr = SPIFFS.open(configfile, "r");
  if (fr) {
    ssid = fr.readStringUntil('\n');
    ssid.trim();
    if (ssid =="") ssid = defaultSSID;
    passwd = fr.readStringUntil('\n');
    passwd.trim();
    if (passwd == "" && ssid == defaultSSID) passwd = defaultPASSWD;
    
    fr.close();

  } else {
    //InitialConfigFile
    Serial.println("read open error");
    Serial.print("SPIFFS data seems clash. Default load...");
    InitialConfigFile();
  }
  
}

void wifiinput() {
  String html = Headder_str();
  html += "<a href='/'>設定メニューへ戻る</a> ";
  html += "<hr><p>";
  html += "<h3>WiFiアクセスポイント設定</h3>";
  html += WIFI_Form_str();
  html += "<br><hr><p><center><a href='/'>キャンセル</a></center>";
  html += "</body></html>";
  webServer.send(200, "text/html", html);
}

//*******************************************
String WIFI_Form_str(){

  Serial.println("wifi scan start");

  // WiFi.scanNetworks will return the number of networks found
  uint8_t ssid_num = WiFi.scanNetworks();
  Serial.println("scan done\r\n");
  
  if (ssid_num == 0) {
    Serial.println("no networks found");
  } else {
    Serial.printf("%d networks found\r\n\r\n", ssid_num);
    if (ssid_num > SSIDLIMIT) ssid_num = SSIDLIMIT;
    for (int i = 0; i < ssid_num; ++i) {
      ssid_str[i] = WiFi.SSID(i);
      String wifi_auth_open = ((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      ssid_rssi_str[i] = ssid_str[i] + " (" + WiFi.RSSI(i) + "dBm)" + wifi_auth_open;
      ssid_rssi_str[i] = ssid_str[i] + wifi_auth_open;
      Serial.printf("%d: %s\r\n", i, ssid_rssi_str[i].c_str());
      delay(10);
    }
  }
  
  String str = "";
  str += "SSID<br>";
  str += "<form action='/wifiset' method='get'>";
  str += "<select name='ssid' id ='ssid'>";
  for(int i=0; i<ssid_num; i++){
    str += "<option value=" + ssid_str[i] + ">" + ssid_rssi_str[i] + "</option>";
  }
  str += "<option value=" + ssid + ">" + ssid + "(current)</option>";
  if (ssid != defaultSSID){
    str += "<option value=" + defaultSSID + ">" + defaultSSID + "(default)</option>";
  }
  str += "</select><br>\r\n";
  str += "Password<br><input type='password' name='passwd' value='" + passwd + "'>";
  str += "<br><input type='submit' value='設定'>";
  str += "</form><br>";
  str += "<script>document.getElementById('ssid').value = '"+ ssid +"';</script>";
  return str;
}


void wifiset(){
  
  ssid = webServer.arg("ssid");
  passwd = webServer.arg("passwd");
  ssid.trim();
  passwd.trim();
 
  WriteConfigFile();
  
  // 「/」に転送
  webServer.sendHeader("Location", String("/"), true);
  webServer.send(302, "text/plain", "");
}

void reboot() {
  String html = Headder_str();
  html += "<hr><p>";
  html += "<h3>再起動実行確認</h3><p>";
  html += "再起動してよろしいですか?<p>";
  html += "<center><a href='/doreboot'>はい</a> <a href='/'>いいえ</a></center>";
  html += "<p><hr>";
  html += "</body></html>";
  webServer.send(200, "text/html", html);
}

void doreboot() {
  String html = Headder_str();
  html += "<hr><p>";
  html += "<h3>再起動中</h3><p>";
  html += "The setting WiFi connection will be disconnected...<p>";
  html += "<hr>";
  html += "</body></html>";
  webServer.send(200, "text/html", html);

  // reboot esp32
  Serial.println("reboot esp32 now.");
    
  delay(2000); // hold 2 sec
  ESP.restart(); // restart ESP32
}

/*
void wifihushiyou{
  
}
*/