int Speaker_pin = 4;

void Beepshort(){
  pinMode(Speaker_pin, OUTPUT);
  digitalWrite(Speaker_pin, HIGH);
  delay(150);
  pinMode(Speaker_pin, INPUT);
}

void BeepTime(int Time){
  pinMode(Speaker_pin, OUTPUT);
  digitalWrite(Speaker_pin, HIGH);
  delay(Time);
  pinMode(Speaker_pin, INPUT);
}





