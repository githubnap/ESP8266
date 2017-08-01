
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);



#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 16;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 0;     // Configurable, see typical pin layout above
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];

void printDec(byte *buffer, byte bufferSize);

String keep_ID = "";


void setup() { 
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  lcd.begin();
  lcd.backlight();
  lcd.print("Start");
  delay(500);
  lcd.clear();
  
}
 
void loop() {
lcd.setCursor(0, 0);
  lcd.print("Find New ID...");

  
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  if ( ! rfid.PICC_ReadCardSerial())
    return;







    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   


    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
  


  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


void printDec(byte *buffer, byte bufferSize) {
  
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
    if(buffer[i] < 0x10){
      keep_ID += "0";
    }
      keep_ID += buffer[i];
    
  }
   // id_now = keep_ID.toInt();
   Serial.println();
  Serial.print(keep_ID);
  lcd.setCursor(0, 1);
   lcd.print(keep_ID);
   //delay(5000);
   //lcd.clear();
  keep_ID = "";
}
