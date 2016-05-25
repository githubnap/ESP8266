void setupSerial(){
  Serial.begin(9600);
  while (!Serial) {;}// wait for serial port to connect. Needed for native USB port only
  serial = true;
}

void Serial_print(int text){
  if(serial){
    Serial.print(text);
  }
}

void Serial_println(int text){
  if(serial){
   Serial.println(text);
  }
}

void Serial_print(char text){
  if(serial){
    Serial.print(text);
  }
}

void Serial_println(char text){
  if(serial){
   Serial.println(text);
  }
}

void Serial_print(String text){
  if(serial){
    Serial.print(text);
  }
}

void Serial_println(String text){
  if(serial){
   Serial.println(text);
  }
}

