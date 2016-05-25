/*
  Arduino -> Module MAX7219
  ขา16 = D0 -> DIN
  ขา5 = D2 -> CS
  ขา3 = D1 -> CLK
  5V -> Vcc
  Gnd -> Gnd
*/

//LedControl lc=LedControl(16,5,4,1); // เลข 1 คือจำนวนโมดูลแสดงผลที่ต่อ ในที่นี้ต่อ 1 โมดูล
LedControl lc=LedControl(16,3,1,1);
void setupLED(){  // กำหนดค่าเริ่มต้น MAX7219 device
  lc.shutdown(0, false); // Enable display
  lc.setIntensity(0, 15); // ปรับระดับความสว่างได้ 16 ระดับ 0 คือค่าต่ำสุด 15 คือค่าสูงสุด
  lc.clearDisplay(0); // Clear display register
}

void showTimeLED(){
  lc.clearDisplay(0);
  Serial_print(T_hour);Serial_print(":");
  Serial_print(T_min);Serial_print(":");
  Serial_print(T_sec);Serial_print("\t");
  Serial_print(T_day);Serial_print("/");
  Serial_println(T_mon);

  lc.setChar(0, 7, (T_hour)/10, false);
  lc.setChar(0, 6, (T_hour)%10, false);
  lc.setChar(0, 4, (T_min)/10, false);
  lc.setChar(0, 3, (T_min)%10, false);
  lc.setChar(0, 1, (T_sec)/10, false);
  lc.setChar(0, 0, (T_sec)%10, false);

}

void showLED(int address,int number){
  lc.setChar(0, address, number, false);
}

void clearLED(){
  lc.clearDisplay(0);
}

