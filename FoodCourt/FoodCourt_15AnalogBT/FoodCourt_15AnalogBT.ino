/*
  yes = 1101
  no = 1110
  + = 1010
  - = 1100

*/
#include <EEPROM.h>

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

//#define WIFI_SSID       "KORATWATERLAND"
//#define WIFI_PASSWORD   "0862445402"

//#define WIFI_SSID       "ON_Home"
//#define WIFI_PASSWORD   "044349220"

//#define WIFI_SSID       "N_iPhone"
//#define WIFI_PASSWORD   "asdfghjkl"

String WIFI_SSID ;
String WIFI_PASSWORD ;

#define FIREBASE_HOST "test-178e4.firebaseio.com"
#define FIREBASE_KEY "Cy4ZEdOd02GaVEYXO4ynBHQPCFiwZn7J7qWsTzvs"

#define b1 1 //tx
#define b2 3 //rx
#define b3 15 //D8
#define b4 2 //D4

unsigned long timer_this;

#define beepPin 15

int id_now = 0;             // ID ที่ได้มาจากการแสกน RFID
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

int Input;
int menu = 1;

char textKey[7][17] = {
  {"abcdefghijklmnop"},
  {"qrstuvwxyz"},
  {"ABCDEFGHIJKLMNOP"},
  {"QRSTUVWXYZ"},
  {"1234567890"},
  {"_. !@#$%^&*()+-="},
  {";:{}[]@<>/|"}
};

byte cursor_text[] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
byte arrow[] = {0x08, 0x04, 0x02, 0x1F, 0x1F, 0x02, 0x04, 0x08};


void dump_byte_array(byte *buffer, byte bufferSize);
void beep(int beepTime);
void push_Header();
void Exit();
int ReadInput();
unsigned long TimeNow();
unsigned int TimeCheck();
void History();
void Setup();
void SetupWifi();
void SetupFireBase();
void SetupWifi_SSID();
//void SetupWifi_Password();

void setup() {

  Serial.begin(9600);
  EEPROM.begin(512);

  //  pinMode(b1, INPUT);
  //  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);

  pinMode(A0, INPUT);

  lcd.begin();
  lcd.backlight();
  lcd.print("Wait");
  delay(500);
  lcd.clear();
  lcd.createChar(0, cursor_text);
  lcd.createChar(1, arrow);

  if (ReadInput() == 1) {
    Setup();
  }
  lcd.clear();

  WIFI_SSID = "";
  for (int i = 0; i < 16; i++)     //ดึงข้อมูล wifi จากหน่วยความจำมา
  {
    WIFI_SSID += char(EEPROM.read(i));
  }

  WIFI_SSID = WIFI_SSID.substring(0, WIFI_SSID.indexOf("_<END>_"));

    WIFI_PASSWORD = "";
  for (int i = 25; i < 50; i++)     //ดึงข้อมูล wifi จากหน่วยความจำมา
  {
    WIFI_PASSWORD += char(EEPROM.read(i));
  }

  WIFI_PASSWORD = WIFI_PASSWORD.substring(0, WIFI_PASSWORD.indexOf("_<ENDPW>_"));

  lcd.print("Wifi:");
  lcd.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
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
}


void loop() {

  Header_Nameid = "KWLcard/";
  Input = 0;

  switch (page) {
    case 0: {
        delay(100);
        lcd.clear();
        lcd.print("Find New ID...");
        lcd.setCursor(15, 1);
        lcd.print("0");

        if ( ! mfrc522.PICC_IsNewCardPresent())
          return;
        if ( ! mfrc522.PICC_ReadCardSerial())
          return;

        beep(100);


        dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        page = 1;

        break;                      //สุดหน้า0
      }

    case 1: {
        delay(100);
        money_1 = 1;
        money_2 = money_3 = -1;
        money_cal = money_value = 0;
        Header_Nameid += id_now;
        page_check = true;
        push_Header();
        money_now = 0;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ID:");
        lcd.print(String(id_now));
        lcd.setCursor(0, 1);

        money_now = Firebase.getInt(Header + Header_Nameid);
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

        break;                        //สุดหน้า1
      }
    case 2: {
        delay(100);

        if ( TimeCheck() >= 5) {
          page = 0;
        }


        Input = ReadInput();

        if (Input == 10) {  //-
          lcd.setCursor(0, 1);
          lcd.print("              ");
          lcd.setCursor(0, 1);
          P_or_M = false;
          lcd.setCursor(0, 1);
          lcd.print("-");
          page = 3 ;
          beep(100);

        } else if (Input == 12) {  //+
          lcd.setCursor(0, 1);
          lcd.print("              ");
          lcd.setCursor(0, 1);
          P_or_M = true;
          lcd.setCursor(0, 1);
          lcd.print("+");
          page = 3 ;
          beep(100);
        }
        break;                        //สุดหน้า2
      }

    case 3: {
        delay(100);                   //ตัวเลขหลักที่ 1
        lcd.setCursor(15, 1);
        lcd.print("3");

        if (page_check) {
          //lcd.setCursor(1, 1);
          //lcd.print(money_1);
          page_check = false;
          delay(500);
        }


        Input = ReadInput();

        if ( Input == 1) { // 1
          money_1 = 1;
          lcd.setCursor(1, 1);
          lcd.print(money_1);
          page = 4;
          money_value = 1;
          page_check = true;
          beep(100);
          delay(100);


        } else if ( Input == 2) { // 2
          money_1 = 2;
          lcd.setCursor(1, 1);
          lcd.print(money_1);
          page = 4;
          money_value = 1;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 3) { // 3
          money_1 = 3;
          lcd.setCursor(1, 1);
          lcd.print(money_1);
          page = 4;
          money_value = 1;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 4) { // 4
          money_1 = 4;
          lcd.setCursor(1, 1);
          lcd.print(money_1);
          page = 4;
          money_value = 1;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 5) { // 5
          money_1 = 5;
          lcd.setCursor(1, 1);
          lcd.print(money_1);
          page = 4;
          money_value = 1;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 6) { // 6
          money_1 = 6;
          lcd.setCursor(1, 1);
          lcd.print(money_1);
          page = 4;
          money_value = 1;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 7) { // 7
          money_1 = 7;
          lcd.setCursor(1, 1);
          lcd.print(money_1);
          page = 4;
          money_value = 1;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 8) { // 8
          money_1 = 8;
          lcd.setCursor(1, 1);
          lcd.print(money_1);
          page = 4;
          money_value = 1;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 9) { // 9
          money_1 = 9;
          lcd.setCursor(1, 1);
          lcd.print(money_1);
          page = 4;
          money_value = 1;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 14) { // No
          lcd.print(" ");
          lcd.setCursor(5, 1);
          lcd.print("Cancler");
          beep(100);
          delay(300);
          Exit();
        }
        break;                        //สุดหน้า3
      }

    case 4: {
        delay(50);                   //ตัวเลขหลักที่ 2
        lcd.setCursor(15, 1);
        lcd.print("4");

        if (page_check) {
          lcd.setCursor(2, 1);
          lcd.print("$");
          page_check = false;
          delay(500);
        }




        Input = ReadInput();

        if ( Input == 1) { // 1
          money_2 = 1;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 2) { // 2
          money_2 = 2;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 3) { // 3
          money_2 = 3;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 4) { // 4
          money_2 = 4;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 5) { // 5
          money_2 = 5;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 6) { // 6
          money_2 = 6;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 7) { // 7
          money_2 = 7;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 8) { // 8
          money_2 = 8;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 9) { // 9
          money_2 = 9;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 0) { // 0
          money_2 = 0;
          lcd.setCursor(2, 1);
          lcd.print(money_2);
          page = 5;
          money_value = 2;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 14) { // No
          lcd.print(" ");
          lcd.setCursor(5, 1);
          lcd.print("Cancler");
          beep(100);
          delay(300);
          Exit();
        } else if ( Input == 13) { // Yes
          page = 6;
          beep(100);
          delay(200);
        }
        break;                        //สุดหน้า4
      }

    case 5: {
        delay(50);                   //ตัวเลขหลักที่ 3
        lcd.setCursor(15, 1);
        lcd.print("5");

        if (page_check) {
          lcd.setCursor(3, 1);
          lcd.print("$");
          page_check = false;
          delay(500);
        }


        Input = ReadInput();

        if ( Input == 1) { // 1
          money_3 = 1;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 2) { // 2
          money_3 = 2;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 3) { // 3
          money_3 = 3;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 4) { // 4
          money_3 = 4;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 5) { // 5
          money_3 = 5;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 6) { // 6
          money_3 = 6;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 7) { // 7
          money_3 = 7;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 8) { // 8
          money_3 = 8;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 9) { // 9
          money_3 = 9;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 0) { // 0
          money_3 = 0;
          lcd.setCursor(3, 1);
          lcd.print(money_3);
          page = 6;
          money_value = 3;
          page_check = true;
          beep(100);
          delay(100);

        } else if ( Input == 14) { // No
          lcd.print(" ");
          lcd.setCursor(5, 1);
          lcd.print("Cancler");
          beep(100);
          delay(300);
          Exit();
        } else if ( Input == 13) { // Yes
          beep(100);
          page = 6;
          delay(200);
        }
        break;                                //สุดหน้า5
      }

    case 6:    {                               // หน้ายืนยัน

        lcd.setCursor(15, 1);
        lcd.print("6");

        lcd.setCursor(6, 1);
        lcd.print("Y/N");
        delay(200);


        Input = ReadInput();

        if ( Input == 13) { // Yes
          page = 7;
          beep(100);
          delay(100);

        } else if ( Input == 14) { // No
          lcd.print(" ");
          lcd.setCursor(5, 1);
          lcd.print("Cancler");
          beep(100);
          delay(300);
          Exit();
        }
        break;                                //สุดหน้า6
      }

    case 7: {                                  //หน้าคำนวน และ อัพค่าต่างๆขึ้นไป

        lcd.setCursor(15, 1);
        lcd.print("7");

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
              ok = true;
            }
            break;
        }

        if (ok) {
          Header_Nameid += id_now;
          Firebase.setInt(Header + Header_Nameid, money_now);

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
        break;                                // สุดหน้า 7
      }

  }


}

void changPage() {
  page = 2;
}


//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv    Setup

void Setup() {
  menu = 1;
  beep(100);
  delay(300);
  while (page_check) {
    delay(100);
    lcd.clear();

    Input = 0;
    Input = ReadInput();

    switch (Input) {
      case 1: {
          menu = 1;
          break;
        }

      case 2: {
          menu = 2;
          break;
        }

      case 3: {
          menu = 3;
          break;
        }

      case 13: { //Yes
          if (menu == 1) {
            SetupWifi();
          } else if (menu == 2) {
            /////////////////////////////////////////////////////////
          } else if (menu == 3) {
            while (page_check) {
              delay(200);
              Input = 0;
              Input = ReadInput();
              lcd.clear();
              lcd.print("Factory Reset ?");
              lcd.setCursor(0, 1);
              lcd.print("Yes/No");
              if (Input == 13) { //Yes
                beep(300);
                for (int i = 0 ; i < 50 ; i++) {
                  EEPROM.write(i, 0);
                }
                EEPROM.commit();
                lcd.clear();
                lcd.print("Reset Done");
                Setup();

              } else if (Input == 14) { //No
                beep(100);
                lcd.clear();
                Setup();
              }
            }
          }

          break;
        }

      case 14: { //No

          break;
        }


    }//switch

    switch (menu) {
      case 1: {
          lcd.print(">1:Setup Wifi");
          lcd.setCursor(0, 1);
          lcd.print(" 2:SetupFireBase");
          break;
        }

      case 2: {
          lcd.print(" 1:Setup Wifi");
          lcd.setCursor(0, 1);
          lcd.print(">2:SetupFireBase");
          break;
        }

      case 3: {
          lcd.print(">3:Reset");                     //ดึงข้อมูล SSID จาก EEporm มา
          break;
        }

    }//switch
  } //while

  page_check = true;



}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^   Setup

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv    Setup Wifi

void SetupWifi() {  //void SetupFireBase()

  WIFI_SSID = "";
  for (int i = 0; i < 16; i++)     //ดึงข้อมูล wifi จากหน่วยความจำมา
  {
    WIFI_SSID += char(EEPROM.read(i));
  }
  WIFI_SSID = WIFI_SSID.substring(0, WIFI_SSID.indexOf("_<END>_"));


    WIFI_PASSWORD = "";
  for (int i = 25; i < 50; i++)     //ดึงข้อมูล wifi จากหน่วยความจำมา
  {
    WIFI_PASSWORD += char(EEPROM.read(i));
  }
  WIFI_PASSWORD = WIFI_PASSWORD.substring(0, WIFI_PASSWORD.indexOf("_<ENDPW>_"));


  menu = 1;
  beep(100);
  delay(300);
  while (page_check) {
    delay(100);
    lcd.clear();

    Input = 0;
    Input = ReadInput();

    switch (Input) {
      case 1: {
          menu = 1;
          break;
        }

      case 2: {
          menu = 2;
          break;
        }

      case 3: {
          menu = 3;
          break;
        }

      case 13: { //Yes
          if (menu == 1) {
            SetupWifi_SSID();
          } else if (menu == 2) {
            SetupWifi_Password();
          }

          break;
        }

      case 14: { //No
          Setup();
          break;
        }


    }//switch

    switch (menu) {
      case 1: {
          lcd.print(">1:SSID");
          lcd.setCursor(0, 1);
          lcd.print(" 2:Password");
          break;
        }

      case 2: {
          lcd.print(" 1:SSID");
          lcd.setCursor(0, 1);
          lcd.print(">2:Password");
          break;
        }

      case 3: {
          lcd.print("SSID:");                     //ดึงข้อมูล SSID จาก EEporm มา
          lcd.print(WIFI_SSID);
          lcd.setCursor(0, 1);
          lcd.print("Pass:");
          lcd.print(WIFI_PASSWORD);
          break;
        }
    }//switch
  }

  page_check = true;

}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^   Setup Wifi

int menu_Keyboard = 0;
String name_setup_SSID;

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv    Setup Wifi SSID
void SetupWifi_SSID() {
  beep(100);
  delay(300);


  menu = 1;
  menu_Keyboard = 0;
  name_setup_SSID = "";

  while (page_check) {
    delay(200);
    lcd.clear();

    Input = 0;
    Input = ReadInput();

    switch (Input) {
      case 0: {
          menu = 0 ;
          break;
        }

      case 1: {
          menu = 1;
          break;
        }

      case 2: {
          menu = 2;
          break;
        }

      case 3: {
          menu = 3;
          break;
        }

      case 4: {
          menu = 4;
          break;
        }

      case 5: {
          menu = 5;
          break;
        }

      case 6: {
          menu = 6;
          break;
        }

      case 7: {
          menu = 7;
          break;
        }

      case 10: { //-
          menu_Keyboard -= 1;
          delay(50);
          break;
        }

      case 12: { //+
          menu_Keyboard += 1;
          delay(50);
          break;
        }

      case 13: { //Yes
          beep(100);
          name_setup_SSID += textKey[menu - 1][menu_Keyboard];
          delay(100);
          break;
        }

      case 14: { //No
          beep(100);
          page_check = false;
          break;
        }
    }//switch

    lcd.home();
    lcd.write(1); //Arrow
    lcd.print(name_setup_SSID);
    lcd.setCursor(0, 1);

    switch (menu) {
      case 0: {
          lcd.clear();
          lcd.print("New SSID:");
          lcd.print(name_setup_SSID);
          lcd.setCursor(0, 1);
          lcd.print("Yes/No");
          while (page_check) {

            delay(200);
            Input = 0;
            Input = ReadInput();
            if (Input == 13) { //Yes
              beep(300);
              name_setup_SSID += "_<END>_";
              for (int i = 0 ; i < 16 ; i++) {
                EEPROM.write(i, 0);
              }
              for (int i = 0; i < name_setup_SSID.length(); i++)
              {
                EEPROM.write(i, name_setup_SSID[i]);
              }
              EEPROM.commit();

              SetupWifi();


            } else if (Input == 14) { //No
              beep(100);
              lcd.clear();
              SetupWifi();
            }
          }
          break;
        }

      case 1: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 15);
          for (int i = 0; i < 16 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 2: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 9);
          for (int i = 0; i < 10 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 3: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 16);
          for (int i = 0; i < 16 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }
      case 4: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 9);
          for (int i = 0; i < 10 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 5: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 9);
          for (int i = 0; i < 10 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 6: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 15);
          for (int i = 0; i < 16 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 7: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 9);
          for (int i = 0; i < 10 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }


    }//switch

    lcd.setCursor(menu_Keyboard, 1);
    lcd.write(0); //cursor

    lcd.setCursor(14, 0);
    lcd.print(menu_Keyboard);


  }

  page_check = true;

}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^   Setup Wifi SSID

String name_setup_Password;

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv    Setup Wifi Password
void SetupWifi_Password() {
  beep(100);
  delay(300);


  menu = 1;
  menu_Keyboard = 0;
  name_setup_Password = "";

  while (page_check) {
    delay(200);
    lcd.clear();

    Input = 0;
    Input = ReadInput();

    switch (Input) {
      case 0: {
          menu = 0 ;
          break;
        }

      case 1: {
          menu = 1;
          break;
        }

      case 2: {
          menu = 2;
          break;
        }

      case 3: {
          menu = 3;
          break;
        }

      case 4: {
          menu = 4;
          break;
        }

      case 5: {
          menu = 5;
          break;
        }

      case 6: {
          menu = 6;
          break;
        }

      case 7: {
          menu = 7;
          break;
        }

      case 10: { //-
          menu_Keyboard -= 1;
          delay(50);
          break;
        }

      case 12: { //+
          menu_Keyboard += 1;
          delay(50);
          break;
        }

      case 13: { //Yes
          beep(100);
          name_setup_Password += textKey[menu - 1][menu_Keyboard];
          delay(100);
          break;
        }

      case 14: { //No
          beep(100);
          page_check = false;
          break;
        }
    }//switch

    lcd.home();
    lcd.write(1); //Arrow
    lcd.print(name_setup_Password);
    lcd.setCursor(0, 1);

    switch (menu) {
      case 0: {
          lcd.clear();
          lcd.print("New Pass:");
          lcd.print(name_setup_Password);
          lcd.setCursor(0, 1);
          lcd.print("Yes/No");
          while (page_check) {

            delay(200);
            Input = 0;
            Input = ReadInput();
            if (Input == 13) { //Yes
              beep(300);
              name_setup_SSID += "_<ENDPW>_";
              for (int i = 25 ; i < 50 ; i++) {
                EEPROM.write(i, 0);
              }
              for (int i = 25; i < name_setup_Password.length()+25; i++)
              {
                EEPROM.write(i, name_setup_Password[i-25]);
              }
              EEPROM.commit();

              SetupWifi();


            } else if (Input == 14) { //No
              beep(100);
              lcd.clear();
              SetupWifi();
            }
          }
          break;
        }

      case 1: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 15);
          for (int i = 0; i < 16 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 2: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 9);
          for (int i = 0; i < 10 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 3: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 16);
          for (int i = 0; i < 16 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }
      case 4: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 9);
          for (int i = 0; i < 10 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 5: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 9);
          for (int i = 0; i < 10 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 6: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 15);
          for (int i = 0; i < 16 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }

      case 7: {
          menu_Keyboard = constrain(menu_Keyboard, 0, 9);
          for (int i = 0; i < 10 ; i++) {
            lcd.print(textKey[menu - 1][i]);
          }
          break;
        }


    }//switch

    lcd.setCursor(menu_Keyboard, 1);
    lcd.write(0); //cursor

    lcd.setCursor(14, 0);
    lcd.print(menu_Keyboard);


  }

  page_check = true;

}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^   Setup Wifi Password



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

  int count_number = Firebase.getInt(Header + "History/Count");   // เก็บจำหนวนครั้ง
  count_number++;
  Firebase.setInt(Header + "History/Count" , count_number);

  String History_Log = "";
  History_Log += String(count_number);
  History_Log += ")";
  History_Log += "  ";
  History_Log += String(p_tm -> tm_hour);
  History_Log += ":";
  History_Log += String(p_tm -> tm_min);
  History_Log += " ID=";
  History_Log += String(id_now);
  History_Log += " Value=";

  if (P_or_M) {                                           // เช็คว่าลบหรือบวก
    int count_purchase = Firebase.getInt(Header + "History/Purchase");
    Firebase.setInt(Header + "History/Purchase" , count_purchase + money_cal);

    History_Log += "+";
  } else {
    int count_sell = Firebase.getInt(Header + "History/Sell");
    Firebase.setInt(Header + "History/Sell" , count_sell + money_cal);

    History_Log += "-";
  }
  History_Log += String(money_cal);
  Firebase.pushString(Header + "History/Log", History_Log);
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
  Header += "Day:";
  Header += String(p_tm -> tm_mday);                   //คำนวนวัน
  Header += "/";
  Header += "Number:";
  Header += Number_Tracking;
  Header += "/";
}

void Exit() {
  //โชว์ยอดเงินคงเหลือ
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ID:");
  lcd.print(String(id_now));
  lcd.setCursor(0, 1);

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

  lcd.clear();
  lcd.print("Find New ID");
}


int ReadInput() {
  int newVal = analogRead(A0);

  if (newVal <= 20) {
    return 11;
  } else if (newVal >= 21 && newVal <= 50) {
    return 15;
  } else if (newVal >= 51 && newVal <= 70) { // No
    return 14;
  } else if (newVal >= 71 && newVal <= 90) { // Yes
    return 13;
  } else if (newVal >= 95 && newVal <= 106) { // +
    return 12;
  } else if (newVal >= 107 && newVal <= 150) {
    return 9;
  } else if (newVal >= 151 && newVal <= 190) { // ปุ่มที่11
    return 0;
  } else if (newVal >= 191 && newVal <= 230) {
    return 8;
  } else if (newVal >= 231 && newVal <= 270) { // -
    return 10;
  } else if (newVal >= 271 && newVal <= 350) {
    return 7;
  } else if (newVal >= 351 && newVal <= 470) {
    return 6;
  } else if (newVal >= 471 && newVal <= 600) {
    return 4;
  } else if (newVal >= 601 && newVal <= 700) {
    return 5;
  } else if (newVal >= 701 && newVal <= 800) {
    return 3;
  } else if (newVal >= 801 && newVal <= 1000) {
    return 2;
  } else if (newVal >= 1001) {
    return 1;
  }
}

unsigned long TimeNow() {
  return (millis() / 1000);
}

unsigned int TimeCheck() {
  return ( abs(TimeNow() - timer_this ));
}
