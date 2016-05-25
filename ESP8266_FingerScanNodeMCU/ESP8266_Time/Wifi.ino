
int count = 0;
void setupWifi(){
  Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial_println("\nConnecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {

    for(int i=0;i<=7;i++){
      showLED(i,count);
    }
    
    Serial_print(".");
    count++;
    delay(1000);
  }
  
}

