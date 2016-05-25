int timezone = 7;
int dst = 0;


  
void setupTime(){
  configTime(timezone * 3600, dst, "pool.ntp.org", "time.nist.gov");
  Serial_println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial_print(".");
    delay(1000);
  }
  Serial_println("");
}

void getTime(){
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  
  T_hour = p_tm -> tm_hour;
  T_min = p_tm -> tm_min;
  T_sec = p_tm -> tm_sec;
  T_day = p_tm -> tm_mday;
  T_mon = p_tm -> tm_mon+1;
}

