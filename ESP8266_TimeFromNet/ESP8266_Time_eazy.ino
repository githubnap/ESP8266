#include <ESP8266WiFi.h>
#include <time.h>

#define WIFI_SSID       "ON_Home"
#define WIFI_PASSWORD   "044349220"

#define timezone = 7 * 3600;
#define dst = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(WiFi.localIP());
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());



  configTime(timezone , dst, "pool.ntp.org", "time.nist.gov");
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

  //Serial.print(ctime(&now));  //Full time & day
  //Serial.print("  >>>  ");
  Serial.print(p_tm -> tm_hour); Serial.print(":");
  Serial.print(p_tm -> tm_min); Serial.print(":");
  Serial.print(p_tm -> tm_sec); Serial.print("\t");
  Serial.print(p_tm -> tm_mday); Serial.print("/");
  Serial.print(p_tm -> tm_mon + 1); Serial.print("/");
  int year_time = (p_tm -> tm_year) - 100 + 2000;
  Serial.println(year_time);

  delay(1000);
}
