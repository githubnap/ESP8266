/*
  yes = 1101
  no = 1110
  + = 1010
  - = 1100

*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);

#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN         16           // Configurable, see typical pin layout above
#define SS_PIN          0          // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#include <time.h>
#define timezone  7*3600
#define dst  0

#define WIFI_SSID       "KORATWATERLAND"
#define WIFI_PASSWORD   "0862445402"
//#define WIFI_SSID       "ON_Home"
//#define WIFI_PASSWORD   "044349220"
//#define FIREBASE_HOST "test-178e4.firebaseio.com"
//#define FIREBASE_KEY "Cy4ZEdOd02GaVEYXO4ynBHQPCFiwZn7J7qWsTzvs"

#define FIREBASE_HOST "kwl-oc-money.firebaseio.com"
#define FIREBASE_KEY "uyY8G0c23Rrct8yl74XDoh7Q5XySIDCC2U31khiM"

#define b1 1 //tx
#define b2 2 //rx
#define b3 3 //D8
#define b4 15 //D4

unsigned long timer_this;

#define beepPin 3

int id_now = 0;             // ID ที่ได้มาจากการแสกน RFID
String id_now_name = "";
bool P_or_M;                // ไว้ดูว่า เป็นค่าบวกหรือค่าลบ ของเงินที่จะเติมไป
int page = 0;               // ลำดับ Step ของการบวนการต่างๆ
bool page_check = true;     // ตัวแปรที่ไว้ใช้ครั้งเดียวในแต่ละหน้า

int money_1 = 0;
int money_2 = 0;
int money_3 = 0;
int money_cal = 0;
int money_now = 0;
int money_value = 0;

String Header = "";
uint8_t Number_Tracking = 1;       //เครื่องที่เท่าไหร่ (เครื่องอยู่ตามที่ต่างๆ)
String Header_Nameid = "";

String Input;

void dump_byte_array(byte *buffer, byte bufferSize);
void beep(int beepTime);
void push_Header();
void Exit();
String ReadInput();
unsigned long TimeNow();
unsigned int TimeCheck();

void setup() {

  Serial.begin(9600);

  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);



  lcd.begin();
  lcd.backlight();
  lcd.print("Wait");
  delay(500);
  lcd.clear();

  lcd.print("Wifi: ");
  lcd.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lcd.setCursor(0, 1);
  lcd.print("connecting");
  int j = 10;
  while (WiFi.status() != WL_CONNECTED) {
    if (j >= 16) {
      lcd.setCursor(10, 1);
      lcd.print("       ");
      lcd.setCursor(10, 1);
      j = 10;
    }
    lcd.print(".");
    delay(500);
    j++;
  }

  configTime(timezone , dst, "pool.ntp.org", "time.nist.gov");
  lcd.clear();
  lcd.print("Setting time");
  while (!time(nullptr)) {
    if (j >= 16) {
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      j = 1;
    }
    lcd.print(".");
    delay(500);
    j++;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_KEY);

  while (!Serial);
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);

  lcd.setCursor(0, 1);
  lcd.print("Start");
  delay(1000);
  lcd.clear();

}

bool ch_start_print = true;

void loop() {

  //Header_Nameid = "KWLcard/";

  if (page != 0) {
    ch_start_print = true;
  }

  switch (page) {
    case 0:
      delay(100);


      if (ch_start_print) {
        lcd.clear();
        lcd.print("Find New ID...");
//        lcd.setCursor(15, 1);
//        lcd.print("0");
        ch_start_print = false;
      }

      if (ReadInput() == "0001") {  // กดปุ่ม 1 เพื่อดูยอดขายวัน
        push_Header();
        time_t now = time(nullptr);
        struct tm* p_tm = localtime(&now);
        lcd.setCursor(0, 1);
        lcd.print("              ");
        lcd.setCursor(0, 1);
        lcd.print("Day:" + String(p_tm -> tm_mday) + "= " );
        lcd.print(String(Firebase.getInt(Header + "Balance/Day:" + String(p_tm -> tm_mday) + "/Sell_Day")));
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("              ");
        lcd.setCursor(0, 1);
      }

        if (ReadInput() == "0010") {  // กดปุ่ม 2 เพื่อดูยอดขายเดือน
        push_Header();
        time_t now = time(nullptr);
        struct tm* p_tm = localtime(&now);
        lcd.setCursor(0, 1);
        lcd.print("              ");
        lcd.setCursor(0, 1);
        lcd.print("Month:" + String(p_tm -> tm_mon + 1) + "= " );
        lcd.print(String(Firebase.getInt(Header + "Balance/Sell_Month")));
        delay(1000);
        lcd.setCursor(0, 1);
        lcd.print("              ");
        lcd.setCursor(0, 1);
      }



//      lcd.setCursor(12, 0);
//      lcd.print(digitalRead(b1));
//      lcd.print(digitalRead(b2));
//      lcd.print(digitalRead(b3));
//      lcd.print(digitalRead(b4));
      //lcd.setCursor(0, 0);

      if ( ! mfrc522.PICC_IsNewCardPresent())
        return;
      if ( ! mfrc522.PICC_ReadCardSerial())
        return;

      beep(100);


      dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
      page = 1;


      break;

    case 1:
      delay(100);
      money_1 = 1;
      money_2 = money_3 = -1;
      money_cal = money_value = 0;
      //Header_Nameid += id_now;
      page_check = true;
      push_Header();
      money_now = 0;


      lcd.clear();
//      lcd.setCursor(12, 0);
//      lcd.print(digitalRead(b1));
//      lcd.print(digitalRead(b2));
//      lcd.print(digitalRead(b3));
//      lcd.print(digitalRead(b4));
      lcd.setCursor(0, 0);

      lcd.setCursor(0, 1);
      lcd.print("ID:");
      lcd.print(String(id_now));
      lcd.setCursor(0, 0);
      Firebase.setInt("DataBase/Staff_Name/New_ID",id_now);       //แสกนบัตรใหม่แล้วเพิ่มเข้าไป
      lcd.print("ID:");
      id_now_name = Firebase.getString("DataBase/Staff_Name/" + String(id_now));
      lcd.print(id_now_name);


      lcd.setCursor(0, 1);
      lcd.print("              ");
      lcd.setCursor(0, 1);
      money_now = Firebase.getInt(Header + "Staff/" + id_now_name + "/remain");//Header_Nameid);
      timer_this = millis() / 1000;

      if (Firebase.failed()) {
        lcd.print("Cann't Connect");
        beep(1000);
        delay(100);
        return;
      } else {
        lcd.print(money_now);
        lcd.print("$");
      }

      page = 2 ;

      break;

    case 2:
      delay(100);

      if ( TimeCheck() >= 3) {
        //page = 0;
        
        Exit();
      }

      if (ReadInput() == "1100") {  // -
        lcd.setCursor(0, 1);
        lcd.print("              ");
        lcd.setCursor(0, 1);
        P_or_M = false;
        lcd.setCursor(0, 1);
        lcd.print("-");
        page = 3 ;
        beep(100);

      } else if (ReadInput() == "1010") {  // +
        bool ch_plus = Firebase.getBool("DataBase/Plus");
        
        if(ch_plus == true){
          lcd.setCursor(0, 1);
          lcd.print("              ");
          lcd.setCursor(0, 1);
          lcd.print("Allow ^_^");
          delay(1000);
          
          lcd.setCursor(0, 1);
          lcd.print("              ");
          lcd.setCursor(0, 1);
          P_or_M = true;
          lcd.setCursor(0, 1);
          lcd.print("+");
          page = 3 ;
          beep(100);
          Firebase.setBool("DataBase/Plus",false);
        }else{
          lcd.clear();
          lcd.print("Lock !!");
          beep(100);
          beep(100);
          beep(100);
          delay(1000);
          Exit();
        }
      }
      break;

    case 3:
      delay(100);                   //ตัวเลขหลักที่ 1
//      lcd.setCursor(15, 1);
//      lcd.print("3");

      if (page_check) {
        //lcd.setCursor(1, 1);
        //lcd.print(money_1);
        page_check = false;
        delay(500);
      }

      Input = "0000";
      Input = ReadInput();

      if ( Input == "0001") { // 1
        money_1 = 1;
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page = 4;
        money_value = 1;
        page_check = true;
        beep(100);
        delay(100);


      } else if ( Input == "0010") { // 2
        money_1 = 2;
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page = 4;
        money_value = 1;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0011") { // 3
        money_1 = 3;
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page = 4;
        money_value = 1;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0100") { // 4
        money_1 = 4;
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page = 4;
        money_value = 1;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0101") { // 5
        money_1 = 5;
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page = 4;
        money_value = 1;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0110") { // 6
        money_1 = 6;
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page = 4;
        money_value = 1;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0111") { // 7
        money_1 = 7;
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page = 4;
        money_value = 1;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1000") { // 8
        money_1 = 8;
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page = 4;
        money_value = 1;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1001") { // 9
        money_1 = 9;
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page = 4;
        money_value = 1;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1110") { // No
        lcd.print(" ");
        lcd.setCursor(5, 1);
        lcd.print("Cancler");
        beep(100);
        delay(300);
        Exit();
      }
      break;

    case 4:
      delay(50);                   //ตัวเลขหลักที่ 2
//      lcd.setCursor(15, 1);
//      lcd.print("4");

      if (page_check) {
        lcd.setCursor(2, 1);
        lcd.print("$");
        page_check = false;
        delay(500);
      }



      Input = "0000";
      Input = ReadInput();

      if ( Input == "0001") { // 1
        money_2 = 1;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0010") { // 2
        money_2 = 2;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0011") { // 3
        money_2 = 3;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0100") { // 4
        money_2 = 4;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0101") { // 5
        money_2 = 5;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0110") { // 6
        money_2 = 6;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0111") { // 7
        money_2 = 7;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1000") { // 8
        money_2 = 8;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1001") { // 9
        money_2 = 9;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1011") { // 0
        money_2 = 0;
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        page = 5;
        money_value = 2;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1110") { // No
        lcd.print(" ");
        lcd.setCursor(5, 1);
        lcd.print("Cancler");
        beep(100);
        delay(300);
        Exit();
      } else if ( Input == "1101") { // Yes
        page = 6;
        beep(100);
        delay(200);
      }
      break;

    case 5:
      delay(50);                   //ตัวเลขหลักที่ 3
//      lcd.setCursor(15, 1);
//      lcd.print("5");

      if (page_check) {
        lcd.setCursor(3, 1);
        lcd.print("$");
        page_check = false;
        delay(500);
      }

      Input = "0000";
      Input = ReadInput();

      if ( Input == "0001") { // 1
        money_3 = 1;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0010") { // 2
        money_3 = 2;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0011") { // 3
        money_3 = 3;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0100") { // 4
        money_3 = 4;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0101") { // 5
        money_3 = 5;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0110") { // 6
        money_3 = 6;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "0111") { // 7
        money_3 = 7;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1000") { // 8
        money_3 = 8;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1001") { // 9
        money_3 = 9;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1011") { // 0
        money_3 = 0;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        page = 6;
        money_value = 3;
        page_check = true;
        beep(100);
        delay(100);

      } else if ( Input == "1110") { // No
        lcd.print(" ");
        lcd.setCursor(5, 1);
        lcd.print("Cancler");
        beep(100);
        delay(300);
        Exit();
      } else if ( Input == "1101") { // Yes
        beep(100);
        page = 6;
        delay(200);
      }
      break;

    case 6:                                   // หน้ายืนยัน

//      lcd.setCursor(15, 1);
//      lcd.print("6");

      lcd.setCursor(6, 1);
      lcd.print("Y/N");
      delay(200);

      Input = "0000";
      Input = ReadInput();

      if ( Input == "1101") { // Yes
        page = 7;
        beep(100);
        delay(100);

      } else if ( Input == "1110") { // No
        lcd.print(" ");
        lcd.setCursor(5, 1);
        lcd.print("Cancler");
        beep(100);
        delay(300);
        Exit();
      }
      break;

    case 7:                                   //หน้าคำนวน และ อัพค่าต่างๆขึ้นไป

//      lcd.setCursor(15, 1);
//      lcd.print("7");

      lcd.setCursor(0, 1);
      lcd.print("               ");
      lcd.setCursor(0, 1);
      lcd.print("Processing");



      switch (money_value) {
        case 1:
          money_cal = money_1;
          break;

        case 2:
          money_cal = (money_1 * 10) + money_2;
          break;

        case 3:
          money_cal = (money_1 * 100) + (money_2 * 10) + money_3;
          break;

        default:
          lcd.clear();
          lcd.print("Error");
          break;
      }

      if (money_cal == 999) {
        money_cal = 1000;
      }

      bool ok = true;

      switch (P_or_M) {
        case true:
          money_now += money_cal;
          ok = true;
          break;

        case false:

          if (money_now - money_cal < 0) {
            lcd.clear();
            lcd.print("Not Enough!!!");
            delay(1000);
            ok = false;
            Exit();
          } else {
            money_now -= money_cal;
            int cal_i = Firebase.getInt(Header +  "Staff/" + id_now_name + "/sell");
            cal_i += money_cal;
            Firebase.setInt(Header +  "Staff/" + id_now_name + "/sell",  cal_i ); //ยอดใช้จ่ายรวมต่อเดือนของคนนั้น
            ok = true;
          }
          break;
      }

      if (ok) {
        //Header_Nameid += id_now;
        Firebase.setInt(Header + "Staff/" + id_now_name + "/remain", money_now);

        lcd.setCursor(0, 1);
        lcd.print("               ");
        lcd.setCursor(0, 1);


        if (Firebase.failed()) {
          lcd.print("Cann't Connect");
          beep(1000);
          delay(100);
          return;
        } else {
          History();            //เก็บ History
          lcd.print("Done");
          beep(200);
          delay(100);
          beep(100);
        }



        Exit();
      }

      page = 0;
      break;
  }

}

void changPage() {
  page = 2;
}

void beep(int beepTime) {
  pinMode(beepPin, OUTPUT);
  digitalWrite(beepPin, HIGH);
  delay(beepTime);
  digitalWrite(beepPin, LOW);
  pinMode(beepPin, INPUT_PULLUP);
}

void dump_byte_array(byte * buffer, byte bufferSize) {
  String keep_ID = "";
  for (byte i = 0; i < bufferSize; i++) {
    //Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    //Serial.print(buffer[i]);
    keep_ID += buffer[i];
  }
  id_now = keep_ID.toInt();
  //Serial.print(keepID);
  keep_ID = "";
}

void History() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  int count_number = Firebase.getInt(Header + "Balance/" + "Day:" + String(p_tm -> tm_mday) + "/Log/Count");  // เก็บจำหนวนครั้ง
  count_number++;
  Firebase.setInt(Header + "Balance/" + "Day:" + String(p_tm -> tm_mday) + "/Log/Count" , count_number);

  String History_Log = "";
  History_Log += String(count_number);
  History_Log += ")";
  History_Log += "  ";
  History_Log += String(p_tm -> tm_hour);
  History_Log += ":";
  History_Log += String(p_tm -> tm_min);
  History_Log += " ID=";
  History_Log += String(id_now_name);
  History_Log += " Value=";

  if (P_or_M) {                                           // เช็คว่าลบหรือบวก
    int count_purchase = Firebase.getInt(Header + "Balance/Fill");
    Firebase.setInt(Header + "Balance/Fill" , count_purchase + money_cal);

    History_Log += "+";
  } else {

    int count_sellDay = Firebase.getInt(Header + "Balance/Day:" + String(p_tm -> tm_mday) + "/Sell_Day"); // ยอดขายทั้งหมดต่อวัน
    Firebase.setInt(Header + "Balance/Day:" + String(p_tm -> tm_mday) + "/Sell_Day" , count_sellDay + money_cal);

    int count_sell = Firebase.getInt(Header + "Balance/Sell_Month"); // ยอดขายทั้งหมดต่อเดือน
    Firebase.setInt(Header + "Balance/Sell_Month" , count_sell + money_cal);

    History_Log += "-";
  }
  History_Log += String(money_cal);
  Firebase.pushString(Header + "Balance/" + "Day:" + String(p_tm -> tm_mday) + "/Log", History_Log);
}

void push_Header() {
  time_t now = time(nullptr);                              // ดึงเวลามาจากเน็ต
  struct tm* p_tm = localtime(&now);
  Header = "Year:";
  Header += String((p_tm -> tm_year) - 100 + 2000);        //คำนวนปี
  Header += "/";
  Header += "Month:";
  Header += String(p_tm -> tm_mon + 1);                   //คำนวนเดือน
  Header += "/";
}

void Exit() {
  //โชว์ยอดเงินคงเหลือ
  lcd.clear();
  //lcd.setCursor(0, 0);
  lcd.print("ID:");
  lcd.print(id_now_name);
  lcd.setCursor(0, 1);

  // money_now = Firebase.getInt(Header + Header_Nameid);

  if (Firebase.failed()) {
    lcd.print("Cann't Connect");
    delay(100);
    return;
  } else {
    lcd.print(money_now);
    lcd.print("$");
  }

  delay(2000);
  page = 0;

  //  lcd.clear();
  //  lcd.print("Find New ID");
}

String ReadInput() {
  String i = String(digitalRead(b1));
  i += digitalRead(b2);
  i += digitalRead(b3);
  i += digitalRead(b4);

  //  lcd.setCursor(0, 0);
  //  lcd.print("               ");
  //  lcd.setCursor(0, 0);
  //  lcd.print(i);

  return (i);
}

unsigned long TimeNow() {
  return (millis() / 1000);
}

unsigned int TimeCheck() {
  return ( abs(TimeNow() - timer_this ));
}
