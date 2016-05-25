
/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <EEPROM.h>
//#include <Adafruit_GFX.h>
//#include <ESP_Adafruit_SSD1306.h>
static const int PIN_LED = 16; // ESPresso Lite
int ledState = LOW;
#define buffer_size 100
//#define OLED_RESET 4

#define period_udp 10
#define period_led 1000
#define period_control_motor 80
//
//#define motorF 12
//#define motorB 13
//#define motorL 14
//#define motorR 15

#define napL1 12
#define napL2 14
#define napL 16

#define napR1 15
#define napR2 2
#define napR 5

#define disconnectState 1000
#define UDPCONNECT 100


//Adafruit_SSD1306 display(OLED_RESET);

/* Set these to your desired credentials. */
const char *ssid = "NopNap_Pippip";
const char *password = "12345678";

unsigned int localPort = 12345;
unsigned long time_now, time_prev_udp, time_prev_led, time_prev_control_motor, timeCheckDisconnect,timeLED13;
int cb = 1;
int8_t sumCheck,sumCheckS;
int tempCh1, tempCh2;
int outputControlForward, outputControlBackward, outputControlLeft, outputControlRight;

int countPacket;

int eeYaw,eePitch,eeRoll;

boolean dataTrue = true;
boolean setUpPID = false;

char packetBuffer[buffer_size] = {0}; //buffer to hold incoming and outgoing packets
WiFiUDP udp;
int count = 0;

IPAddress local_ip(192, 168, 5, 1);
IPAddress gateway(192, 168, 5, 1);
IPAddress subnet(255, 255, 255, 0);


void Read_UDP();
void setup_oled();
void controlMotor();
void ledStatus(int timeBlink);
void readDataYPR();
void writeDataYPR(int yaw,int pitch,int roll);
void checkSetupData();

void setup()
{
  WiFi.disconnect();
  Serial.begin(115200);
  EEPROM.begin(512);
  

  
//  pinMode(motorF, OUTPUT);
//  pinMode(motorB, OUTPUT);
//  pinMode(motorL, OUTPUT);
//  pinMode(motorR, OUTPUT);
  
  pinMode(napL1, OUTPUT);
  pinMode(napL2, OUTPUT);
  pinMode(napR1, OUTPUT);
  pinMode(napR2, OUTPUT);
  pinMode(napL, OUTPUT);
  pinMode(napR, OUTPUT);
  //pinMode(16, OUTPUT);
  //digitalWrite(16, 1);


  //delay(3000);


  



  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  delay(500);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(500);
  IPAddress myIP = WiFi.softAPIP();


  if ( udp.begin(localPort) > 0) {
    //Serial.println(myIP);
  } else {

  }

  delay(1000);

  time_now = millis();
  time_prev_udp = time_now;
  time_prev_led = time_now;
  time_prev_control_motor = time_now;
  timeCheckDisconnect = time_now;

  

}

void loop()
{
  time_now = millis();
  Read_UDP();// stand by read UDP
  //controlMotor();
  if (time_now - time_prev_control_motor >= 1000)
  {
    time_prev_control_motor = time_now;

    //digitalWrite(12, 1);    // debug
    //Read_UDP();
    // digitalWrite(12, 0);    // debug
    //do
//    if (dataTrue) {
//      controlMotor();
//    }

    Serial.println(countPacket);
    countPacket = 0;

  }//end control motor loop

  //  if (time_now - time_prev_led >= period_led)    // 10Hz
  //  {
  //    time_prev_led = time_now;
  //
  //    //   display.clearDisplay();
  //    //   display.setCursor(0, 0);
  //    //   display.println("CMMC.Espresso_lite");
  //    // display.println("");
  //    //   display.print((int8_t)packetBuffer[0]);
  //    //   display.print("  ");
  //    //   display.print((int8_t)packetBuffer[1]);
  //    //   display.print("  ");
  //    //   display.print((int8_t)packetBuffer[2]);
  //    //   display.print("  ");
  //    //   display.print((int8_t)packetBuffer[3]);
  //    //   display.print("                            ");
  //    //   display.display();
  //
  //
  //  }

  //delay(1);
}//end void loop

void Read_UDP()
{
  cb = udp.parsePacket();
  if (!cb) {
    if (time_now - timeCheckDisconnect > 1000) {// When no data from UDP > 500ms car wil stop
      timeCheckDisconnect = time_now;
      //Serial.println("Disconnect!");
      packetBuffer[1] = 0;
      packetBuffer[2] = 0;
      dataTrue = true;
      controlMotor();

      ledStatus(disconnectState);


    }
  }
  else {
    //Serial.print("length=");
    //Serial.println(cb);
    timeCheckDisconnect = time_now;
    ledStatus(UDPCONNECT);
    memset(packetBuffer, 0, buffer_size); // clear mem
    countPacket++;
    udp.read(packetBuffer, cb); // read the packet into the buffer
    // String temp = packetBuffer;
    // Serial.println(packetBuffer);

    // if (count > 8)
    // {
    //   count = 0;
    //   display.clearDisplay();
    //   display.setCursor(0, 0);
    // }
    // count++;
     //Serial.println((int16_t)packetBuffer[0],HEX);
    
    checkSetupData();
 
    
    // display.print((int)packetBuffer[0]);
    // display.print("  ");
    // display.print((int)packetBuffer[1]);
    // display.print("  ");
    // display.print((int)packetBuffer[2]);
    // display.print("  ");
    // display.print((int)packetBuffer[3]);
    // display.println("  ");
    // display.display();
    sumCheck = (int8_t)packetBuffer[1] + (int8_t)packetBuffer[2] + (int8_t)packetBuffer[3] + (int8_t)packetBuffer[4];

    if (sumCheck == (int8_t)packetBuffer[5] ) {
      dataTrue = true;
      //      Serial.print(" check bit ");
      //      Serial.print((int8_t)packetBuffer[0]);
      //      Serial.print("  ch1:");
      //      Serial.print((int8_t)packetBuffer[1]);
      //      Serial.print("  ch2:");
      //      Serial.print((int8_t)packetBuffer[2]);
      //      Serial.print("  ch3:");
      //      Serial.print((int8_t)packetBuffer[3]);
      //      Serial.print("  ch4:");
      //      Serial.print((int8_t)packetBuffer[4]);
      //      Serial.print("  sumAll:");
      //      Serial.print((int8_t)packetBuffer[5]);
      //      Serial.print("  sum check:");
      //      Serial.println(sumCheck);
      controlMotor();
    } else {
      dataTrue = false;
    }

    //    packetBuffer[0] = (packetBuffer[0] + 85);
    //    packetBuffer[1] = (packetBuffer[1] + 85);
    //    packetBuffer[3] = (packetBuffer[3] + 85);

    //    int ch3Temp = packetBuffer[2];
    //    ch3Temp = map(ch3Temp, 0, 100, 0, 255);
    //
    //    int ch1 = packetBuffer[0] * 1.4825;
    //    int ch2 = packetBuffer[1] * 1.4825;
    //    int ch3 = ch3Temp ;
    //    int ch4 = packetBuffer[3] * 1.4825;
    //
    //    Serial.print("a");
    //    Serial.print(ch2);
    //    Serial.print("b");
    //    Serial.print(ch1);
    //    Serial.print("c");
    //    Serial.print(ch3);
    //    Serial.print("d");
    //    Serial.print(ch4);
    //    Serial.println("!");

    //    Serial.write((int8_t)packetBuffer[0]);  // roll
    //      Serial.write((int8_t)packetBuffer[1]);  // pitch
    //      Serial.write((int8_t)packetBuffer[2]);  // throttle
    //      Serial.write((int8_t)packetBuffer[3]);  // yaw
    //      Serial.write(0xfe);
    //digitalWrite(16, !digitalRead(16));



  }
}

/*
   Function controlMotor()
   4 output  F B  L  R
  F B output range 0 - 1023
  L R output on or off (0 , 1023)
*/

void controlMotor() {
  //outputControlForward,outputControlBackward,outputControlLeft,outputControlRight;
  /*tempCh1,tempCh2
     use CH1 and Ch2 control car
  */
  //tempCh1 = (int8_t)packetBuffer[1];
  // tempCh2 = (int8_t)packetBuffer[2];

  //Stick center and offset
  if ((int8_t)packetBuffer[2] > -5  &&  (int8_t)packetBuffer[2] < 5) {
    outputControlForward = 0;
    outputControlBackward =  0;
  }

  if ((int8_t)packetBuffer[1]  > -40  && (int8_t)packetBuffer[1] < 40) {
    outputControlRight = 0;
    outputControlLeft = 0;
  }
  //

  if ((int8_t)packetBuffer[2] > 0) { //if ch1 > 0  car will forward
    outputControlForward = map((int8_t)packetBuffer[2], 0, 60, 0, 1023);
    outputControlBackward = 0;
  }
  if ((int8_t)packetBuffer[2] < 0) { //if ch1 < 0  car will backward
    outputControlForward = 0;

    outputControlBackward =  map(abs((int8_t)packetBuffer[2]), 0, 60, 0, 1023);
  }
//control left right  use on or off
  if ((int8_t)packetBuffer[1] > 40) { //if ch2 > 0  car will right
    outputControlLeft = 0;
    //outputControlRight = map((int8_t)packetBuffer[1], 0, 35, 0, 1023);
    outputControlRight = 1023;
  }
  if ((int8_t)packetBuffer[1] < -40) { //if ch2 < 0  car will left
    outputControlRight = 0;
    //outputControlLeft = map(abs((int8_t)packetBuffer[1]), 0, 35, 0, 1023);
    outputControlLeft = 1023;
  }


  /*
     debug
  */
    Serial.print(" F:");
    Serial.print(outputControlForward);
    Serial.print(" B:");
    Serial.print(outputControlBackward);
    Serial.print(" L:");
    Serial.print(outputControlLeft);
    Serial.print(" R:");
    Serial.println(outputControlRight);
  /*

  */

  /*
     sending output
  */
//
//  outputControlForward = constrain(outputControlForward, 0, 1023);  
//  outputControlBackward = constrain(outputControlBackward, 0, 1023);
//  outputControlLeft = constrain(outputControlLeft, 0, 1023);
//  outputControlRight = constrain(outputControlRight, 0, 1023);
  
  // limits range of sensor values to between 0 and 1023
//  analogWrite(motorF, outputControlForward);
//  analogWrite(motorB, outputControlBackward);
//  analogWrite(motorL, outputControlLeft);
//  analogWrite(motorR, outputControlRight);

  
  if(outputControlLeft > 500 && outputControlForward >500)
  {
    digitalWrite(5,HIGH);
    digitalWrite(16,HIGH);
    digitalWrite(napL1,LOW);
    digitalWrite(napL2,LOW);
    digitalWrite(napR1,HIGH);
    digitalWrite(napR2,LOW);
  }else if(outputControlRight > 500 && outputControlForward >500)
  {
    digitalWrite(5,HIGH);
    digitalWrite(16,HIGH);
    digitalWrite(napL1,HIGH);
    digitalWrite(napL2,LOW);
    digitalWrite(napR1,LOW);
    digitalWrite(napR2,LOW);
  }else  if(outputControlLeft > 500 && outputControlBackward >500)
  {
    digitalWrite(5,HIGH);
    digitalWrite(16,HIGH);
    digitalWrite(napL1,LOW);
    digitalWrite(napL2,LOW);
    digitalWrite(napR1,LOW);
    digitalWrite(napR2,HIGH);
  }else if(outputControlRight > 500 && outputControlBackward >500)
  {
    digitalWrite(5,HIGH);
    digitalWrite(16,HIGH);
    digitalWrite(napL1,LOW);
    digitalWrite(napL2,HIGH);
    digitalWrite(napR1,LOW);
    digitalWrite(napR2,LOW);
  }else if(outputControlLeft > 500)
  {
    digitalWrite(5,HIGH);
    digitalWrite(16,HIGH);
    digitalWrite(napL1,LOW);
    digitalWrite(napL2,HIGH);
    digitalWrite(napR1,HIGH);
    digitalWrite(napR2,LOW);
  }else if(outputControlRight > 500)
  {
    digitalWrite(5,HIGH);
    digitalWrite(16,HIGH);
    digitalWrite(napL1,HIGH);
    digitalWrite(napL2,LOW);
    digitalWrite(napR1,LOW);
    digitalWrite(napR2,HIGH);
  }else if(outputControlForward > 500)
  {
    digitalWrite(5,HIGH);
    digitalWrite(16,HIGH);
    digitalWrite(napL1,HIGH);
    digitalWrite(napL2,LOW);
    digitalWrite(napR1,HIGH);
    digitalWrite(napR2,LOW);
  }else if(outputControlBackward > 500)
  {
    digitalWrite(5,HIGH);
    digitalWrite(16,HIGH);
    digitalWrite(napL1,LOW);
    digitalWrite(napL2,HIGH);
    digitalWrite(napR1,LOW);
    digitalWrite(napR2,HIGH);
  }else 
  {
    stop_car();
  }
//////////////


  
  /*

  */


}//end void controlMotor()


void stop_car(){
    analogWrite(5,0);
    analogWrite(16,0);
    digitalWrite(napL1,LOW);
    digitalWrite(napL2,LOW);
    digitalWrite(napR1,LOW);
    digitalWrite(napR2,LOW);
  }
void ledStatus(int timeBlink) {
  //timeLED13
  if (( time_now - timeLED13) >= timeBlink) {
    //blinkState = !blinkState;
    timeLED13 =  time_now;
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(PIN_LED, ledState);
  }
}

void writeDataYPR(int yaw,int pitch,int roll){
  EEPROM.write(0, yaw);
  EEPROM.commit();
  delay(1);
  EEPROM.write(1, pitch);
  EEPROM.commit();
  delay(1);
  EEPROM.write(2, roll);
  EEPROM.commit();
  delay(1);
}

void readDataYPR(){
  //eeYaw,eePitch,eeRoll
  eeYaw = EEPROM.read(0);
  delay(1);
  eePitch = EEPROM.read(1);
  delay(1);
  eeRoll = EEPROM.read(2);
  delay(1);
}

void checkSetupData(){
     if((int16_t)packetBuffer[0] == 0xf0 && (int16_t)packetBuffer[0] == 0xf0){//setUpPID
      
      int8_t sumCheckSetup = (int8_t)((int8_t)packetBuffer[3] + (int8_t)packetBuffer[4] + (int8_t)packetBuffer[5]);
      if(sumCheckSetup == (int8_t)packetBuffer[6]){
        
      Serial.print("setup!!  ");
      Serial.print("  setbit1:");
      Serial.print((int8_t)packetBuffer[0]);
      Serial.print("  setbit2:");
      Serial.print((int8_t)packetBuffer[1]);
      Serial.print("  YPR");
      Serial.print((int8_t)packetBuffer[2]);
      Serial.print("  Yaw gain:");
      Serial.print((int8_t)packetBuffer[3]+ 128);
      Serial.print("  Pitch gain:");
      Serial.print((int8_t)packetBuffer[4]+ 128);
      Serial.print("  Roll gain:");
      Serial.print((int8_t)packetBuffer[5]+ 128);
      Serial.print("  Sum YPR:");
      Serial.print((int8_t)packetBuffer[6]);
      Serial.print("  Sum YPR check:");
      Serial.println(sumCheckSetup);
      
      writeDataYPR((int8_t)packetBuffer[3]+ 128,(int8_t)packetBuffer[4]+ 128,(int8_t)packetBuffer[5]+ 128);
      readDataYPR();
      Serial.print("  yaw in eerom:");
      Serial.print(eeYaw);
      Serial.print("  pitch in eerom:");
      Serial.print(eePitch);
      Serial.print("  roll in eerom:");
      Serial.println(eeRoll);
      }
      
    }
}



