#include <ESP8266WiFi.h>
#include <time.h>
#include "LedControl.h"

const char* ssid = "ON_Home";
const char* password = "044349220";

int timezone = 7;
int dst = 0;
/*
  Arduino -> Module MAX7219
  ขา13 -> DIN
  ขา14 -> CS
  ขา12 -> CLK
  5V -> Vcc
  Gnd -> Gnd
*/
LedControl lc = LedControl(13, 12, 14, 1); // เลข 1 คือจำนวนโมดูลแสดงผลที่ต่อ ในที่นี้ต่อ 1 โมดูล

void setup() {
    // กำหนดค่าเริ่มต้น MAX7219 device
  lc.shutdown(0, false); // Enable display
  lc.setIntensity(0, 15); // ปรับระดับความสว่างได้ 16 ระดับ 0 คือค่าต่ำสุด 15 คือค่าสูงสุด
  lc.clearDisplay(0); // Clear display register

  
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  configTime(timezone * 3600, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

void loop() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);

  Serial.print(p_tm -> tm_hour);Serial.print(":");
  Serial.print(p_tm -> tm_min);Serial.print(":");
  Serial.print(p_tm -> tm_sec);Serial.print("\t");
  Serial.print(p_tm -> tm_mday);Serial.print("/");
  Serial.println(p_tm -> tm_mon+1);

  lc.setChar(0, 7, (p_tm -> tm_hour)/10, false);
  lc.setChar(0, 6, (p_tm -> tm_hour)%10, false);
  lc.setChar(0, 4, (p_tm -> tm_min)/10, false);
  lc.setChar(0, 3, (p_tm -> tm_min)%10, false);
  lc.setChar(0, 1, (p_tm -> tm_sec)/10, false);
  lc.setChar(0, 0, (p_tm -> tm_sec)%10, false);
//  if(p_tm -> tm_hour >=  10 && p_tm -> tm_hour <= 19){
//    lc.setChar(0, 7, (p_tm -> tm_hour-10, false);
//    lc.setChar(0, 7, p_tm -> tm_hour-10, false);
//  }else if(p_tm -> tm_hour >=20  ){
//    
//  }
  
  
  delay(1000);
}
