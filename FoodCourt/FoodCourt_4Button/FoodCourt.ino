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
#define WIFI_SSID       "ON_Home"
#define WIFI_PASSWORD   "044349220"
#define FIREBASE_HOST "test-178e4.firebaseio.com"
#define FIREBASE_KEY "Cy4ZEdOd02GaVEYXO4ynBHQPCFiwZn7J7qWsTzvs"

#define btL 1 //1
#define btR 15 //0
#define btD 3 //1
#define btU 2 //1

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

void dump_byte_array(byte *buffer, byte bufferSize);
void setup() {

  Serial.begin(9600);

  pinMode(btL, INPUT_PULLUP);
  pinMode(btR, INPUT_PULLUP);
  pinMode(btU, INPUT_PULLUP);
  pinMode(btD, INPUT_PULLUP);



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
  lcd.setCursor(0, 1);
  lcd.print("Start");
  delay(100);
  Firebase.begin(FIREBASE_HOST, FIREBASE_KEY);

  while (!Serial);
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);


}


void loop() {

  Header_Nameid = "KWLcard/";

  switch (page) {
    case 0:
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

      break;

    case 1:
      delay(100);
      money_1 = 1;
      money_2 = money_3 = -1;
      money_cal = money_value = 0;
      Header_Nameid += id_now;
      page_check = true;
      push_Header();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ID:");
      lcd.print(String(id_now));
      lcd.setCursor(0, 1);

      money_now = Firebase.getInt(Header + Header_Nameid);

      if (Firebase.failed()) {
        lcd.print("Cann't Connect");
        delay(100);
        return;
      } else {
        //money_now = Firebase.getInt(Header_Nameid);
        lcd.print(money_now);
        lcd.print("$");
      }
      attachInterrupt(digitalPinToInterrupt(btR), changPage, FALLING);
      attachInterrupt(digitalPinToInterrupt(btL), changPage, FALLING);
      page = 0 ;


      break;

    case 2:
      detachInterrupt(btR);
      detachInterrupt(btL);
      lcd.clear();
      lcd.print("ID:");
      lcd.print(String(id_now));
      lcd.setCursor(0, 1);

      if (digitalRead(btL) != 1) {
        P_or_M = false;
        lcd.print("-");
        page = 3 ;
      } else {
        P_or_M = true;
        lcd.print("+");
        page = 3 ;
      }
      break;

    case 3:
      delay(100);                   //ตัวเลขหลักที่ 1
      lcd.setCursor(15, 1);
      lcd.print("3");

      if (page_check) {
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        page_check = false;
        delay(500);
      }

      if (digitalRead(btU) != 1) {
        money_1 += 1;
        if (money_1 > 9) {
          money_1 = 9;
        }
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        delay(100);
      } else if (digitalRead(btD) == 0) {
        money_1 -= 1;
        if (money_1 <= 0) {
          money_1 = 1;
        }
        lcd.setCursor(1, 1);
        lcd.print(money_1);
        delay(100);
      } else if (digitalRead(btR) == 1) {
        page = 4;
        money_value = 1;
        page_check = true;
      } else if (digitalRead(btL) == 0) {
        lcd.print(" ");
        lcd.setCursor(5, 1);
        lcd.print("Cancler");
        delay(300);
        page = 1;
      }
      break;

    case 4:
      delay(100);                   //ตัวเลขหลักที่ 2
      lcd.setCursor(15, 1);
      lcd.print("4");

      if (page_check) {
        lcd.setCursor(2, 1);
        lcd.print("$");
        page_check = false;
        delay(500);
      }


      if (digitalRead(btU) == 0) {  // ปุ่มขึ้น
        money_2 += 1;
        if (money_2 > 9) {
          money_2 = 9;
        }
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        delay(100);
        
      } else if (digitalRead(btD) == 0) {  // ปุ่มลง
        if (money_2 <= 0) {
          money_2 = 0;
        } else {
          money_2 -= 1;
        }
        lcd.setCursor(2, 1);
        lcd.print(money_2);
        delay(100);
        
      } else if (digitalRead(btR) == 1) {  // ปุ่มขวา
        page = 5;
        money_value = 2;
        page_check = true;

        if (money_2 < 0) {
          page = 6;
          money_value = 1;
          page_check = false;
        }
        
      } else if (digitalRead(btL) == 0) {  // ปุ่มซ้าย
        lcd.print(" ");
        lcd.setCursor(5, 1);
        lcd.print("Cancler");
        delay(300);
        page = 1;
      }
      //}
      break;

    case 5:
      delay(100);                   //ตัวเลขหลักที่ 3
      lcd.setCursor(15, 1);
      lcd.print("5");

      if (page_check) {
        lcd.setCursor(3, 1);
        lcd.print("$");
        page_check = false;
        delay(500);
      }


      if (digitalRead(btU) != 1) {   // ปุ่มขึ้น
        money_3 += 1;
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        if (money_3 >= 9) {
          money_3 = 9;
        }
        delay(100);

      } else if (digitalRead(btD) == 0) {  // ปุ่มลง
        if (money_3 <= 0) {
          money_3 = 0;
        } else {
          money_3 -= 1;
        }
        lcd.setCursor(3, 1);
        lcd.print(money_3);
        delay(100);

      } else if (digitalRead(btR) == 1) {  // ปุ่มขวา
        page = 6;
        money_value = 3;
        page_check = true;
        if (money_3 < 0) {
          page = 6;
          money_value = 2;
          page_check = false;
        }

      } else if (digitalRead(btL) == 0) {  // ปุ่มซ้าย
        lcd.print(" ");
        lcd.setCursor(5, 1);
        lcd.print("Cancler");
        delay(300);
        page = 1;
      }
      break;

    case 6:                                   // หน้ายืนยัน
      delay(300);
      lcd.setCursor(15, 1);
      lcd.print("6");

      lcd.setCursor(6, 1);
      lcd.print("Y/N");
      if (digitalRead(btL) == 0) {
        page = 7;
      }

      if (digitalRead(btR) == 1) {
        lcd.setCursor(5, 1);
        lcd.print("Cancler");
        delay(500);
        Exit();
      }
      break;

    case 7:                                   //หน้าคำนวน และ อัพค่าต่างๆขึ้นไป

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
          beep(500);
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
  Header = String((p_tm -> tm_year) - 100 + 2000);        //คำนวนปี
  Header += "/";
  Header += String(p_tm -> tm_mon + 1);                   //คำนวนเดือน
  Header += "/";
  Header += String(p_tm -> tm_mday);                   //คำนวนวัน
  Header += "/";
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

  money_now = Firebase.getInt(Header + Header_Nameid);

  if (Firebase.failed()) {
    lcd.print("Cann't Connect");
    delay(100);
    return;
  } else {
    lcd.print(money_now);
    lcd.print("$");
  }

  delay(3000);
  page = 0;

  lcd.clear();
  lcd.print("Find New ID");
}