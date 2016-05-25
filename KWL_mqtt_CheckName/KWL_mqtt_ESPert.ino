#include <Arduino.h>
#include <MqttConnector.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>


#define RST_PIN         2           // Configurable, see typical pin layout above
#define SS_PIN          15          // Configurable, see typical pin layout above
#define LED_PIN         16

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;



MqttConnector *mqtt;

#define MQTT_HOST         "mqtt.espert.io"
#define MQTT_PORT         1883
#define MQTT_USERNAME     "nap1993@windowslive.com"
#define MQTT_PASSWORD     "EsperTnap15493"
#define MQTT_CLIENT_ID    "Check"
#define MQTT_PREFIX       "Nap"
#define PUBLISH_EVERY     (5*1000)// every 5 seconds

/* DEVICE DATA & FREQUENCY */
#define DEVICE_NAME       "NAP_Send"

/* WIFI INFO */
#ifndef WIFI_SSID
//  #define WIFI_SSID        "ON_Home"
//  #define WIFI_PASSPHARSE  "044349220"
  #define WIFI_SSID        "Sangtawan1"
  #define WIFI_PASSPHARSE  "0897217868"
#endif

#include "_publish.h"
#include "_receive.h"
#include "init_mqtt.h"

void LED(bool ch);
int ID_income ;

void init_hardware()
{
  Serial.begin(9600);
  delay(10);
  Serial.println();
  Serial.println("BEGIN");
}

void setup()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSPHARSE);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("CONNECTING...");
    delay(300);
  }
  init_hardware();
  init_mqtt();
  
   EEPROM.begin(25);
   
    SPI.begin();        
    mfrc522.PCD_Init(); 
    pinMode(LED_PIN,OUTPUT);
    digitalWrite(LED_PIN,LOW);
    
    for(int i=0;i<=21;i++){
      Staff[i] = EEPROM.read(i);
    }
    
}

void loop()
{
  mqtt->loop();
  
// Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
  
    Serial.print(F("Card UID:"));

    String ID_Read;
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        byte *buffer = mfrc522.uid.uidByte;
        ID_Read+=buffer[i];
    }
  

    if(ID_Read == "15418781219"){//พี่ปุ๊ก
        ID_income = 0;
    }else if(ID_Read == "3122400"){//พี่เนย
        ID_income = 1;
    }else if(ID_Read == "21114581"){//May counter
        ID_income = 2;
    }else if(ID_Read == "2623878219"){//พี่ทราย
        ID_income = 3;
    }else if(ID_Read == "243238611"){//May service
        ID_income = 4;
    }else if(ID_Read == "67251681"){//นุช service
        ID_income = 5;
    }else if(ID_Read == "3143671"){//
        ID_income = 6;
    }else if(ID_Read == "19168661"){//
        ID_income = 7;
    }else if(ID_Read == "14768571"){//
        ID_income = 8;
    }else if(ID_Read == "14722581"){//
        ID_income = 9;
    }else if(ID_Read == "83206681"){//
        ID_income = 10;
    }else if(ID_Read == "211127661"){//
        ID_income = 11;
    }else if(ID_Read == "21194591"){//
        ID_income = 12;
    }else if(ID_Read == "67132621"){//
        ID_income = 13;
    }else if(ID_Read == "227136621"){//
        ID_income = 14;
    }else if(ID_Read == "163166611"){//
        ID_income = 15;
    }else if(ID_Read == "22742621"){//
        ID_income = 16;
    }else if(ID_Read == "131220641"){//
        ID_income = 17;
    }else if(ID_Read == "163222651"){//
        ID_income = 18;
    }else if(ID_Read == "9925681"){//
        ID_income = 19;
    }else if(ID_Read == "24388671"){//
        ID_income = 20;
    }else {
      Serial.print("Not have this card");
    }
        Staff[ID_income] = !Staff[ID_income];
        
        EEPROM.write(ID_income, Staff[ID_income]);
        EEPROM.commit();
        
        Serial.print("Staff[");
        Serial.print(ID_income);
        Serial.print("]= ");
        Serial.println(Staff[ID_income]);



        LED(Staff[ID_income]);
    
    
    Serial.print("ID_Read = ");
    Serial.println(ID_Read);
    
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(1000);
}

void LED(bool ch){
        int delay_Val;
        if(ch){
          delay_Val=300;
        }else{
          delay_Val=1000;
        }
        digitalWrite(LED_PIN,HIGH);
        delay(delay_Val);
        digitalWrite(LED_PIN,LOW);
}

