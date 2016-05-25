/*
  * SD card attached to SPI bus as follows:
   ** MOSI - pin D7
   ** MISO - pin D6
   ** CLK - pin D5
   ** CS - pin D8=15
 */
File myFile;

int CS_pin = 15; 

void setupSD_card(){
  Serial_print("Initializing SD card...");
  if (!SD.begin(CS_pin)) {
    Serial_println("initialization failed!");
    return;
  }
  Serial_println("initialization done.");
}

void SD_write_to(String Flie_name , String Text_to_write){ 
  Flie_name = Flie_name + ".txt";
  myFile = SD.open(Flie_name, FILE_WRITE);
  if (myFile) {
    Serial_print("Writing to test.txt...");
    myFile.println(Text_to_write);
    myFile.close(); // close the file:
    Serial_println("done.");
  } else {
    Serial_println("error opening test.txt"); // if the file didn't open, print an error:
  }
}

void SD_read(String Flie_name){
  Flie_name = Flie_name + ".txt";
  myFile = SD.open(Flie_name);
  if (myFile) {
    Serial_println(Flie_name);
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();  // close the file:
  } else {
    Serial_println("error opening test.txt"); // if the file didn't open, print an error:
  }
}

void SD_remove(String Flie_name){
  // delete the file:
  Flie_name = Flie_name + ".txt";
  Serial_println("Removing .txt...");
  SD.remove(Flie_name);

  if (SD.exists(Flie_name)) {
    Serial_println("Remove Flie_name done.");
  } else {
    Serial_println("Remove Flie_name Error.");
  }
}


