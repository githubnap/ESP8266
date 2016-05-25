//int Bt_1_pin = 3;
//int Bt_2_pin = 1;
int Bt_1_pin = 5;
int Bt_2_pin = 4;
int Bt_1;
int Bt_2;

void setupButton() {
  pinMode(Bt_1_pin, OUTPUT);
  digitalWrite(Bt_1_pin, LOW);
  pinMode(Bt_1_pin, INPUT);

  pinMode(Bt_2_pin, OUTPUT);
  digitalWrite(Bt_2_pin, LOW);
  pinMode(Bt_2_pin, INPUT);

  Bt_1 = 0;
  Bt_2 = 0;
}

int check_puse_button() {
  Bt_1 = digitalRead(Bt_1_pin);
  Bt_2 = digitalRead(Bt_2_pin);
//  showLED(1, Bt_1);
//  showLED(2, Bt_2);

  if (Bt_1 == 1 || Bt_2 == 1 ) {
    delay(300);
    if (Bt_1 == 1 && Bt_2 == 1) {
      //Puse_2_button(); //กด2 ปุ่มเลย
      return 3;
    } else if (Bt_1 == 1 && Bt_2 == 0) {
      //Puse_bt1(); //ถ้ากดแค่ปุ่ม 1
     
       return 1;
    } else if (Bt_1 == 0 && Bt_2 == 1) {
      //Puse_bt2(); //ถ้ากดแค่ปุ่ม 2
       return 2;
    }
  }
  setupButton();
}

void Puse_bt1() {
  showLED(5, 1);
  Serial_println("111");
  Beepshort();
}

void Puse_bt2() {
  showLED(5, 2);
  Serial_println("222");
}

void Puse_2_button() {
  showLED(5, 3);
  Serial_println("333");
}

