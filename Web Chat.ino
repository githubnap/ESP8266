/*
 * Open browser, visit 192.168.4.1
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
extern "C" {
#include "user_interface.h"
}

const char *ssid = "NapChat(192.168.4.1)";
const char *password = "nap123456789";

ESP8266WebServer server(80);

void setup() {
    ESP.eraseConfig();
    delay(1000);
    Serial.begin(115200);
    Serial.println();
    Serial.print("Configuring access point...");
    /* You can remove the password parameter if you want the AP to be open. */
    wifi_set_phy_mode(PHY_MODE_11B);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    Serial.print("Wifi name : ");
    Serial.println(ssid);
    Serial.print("Wifi name : ");
    Serial.println(ssid);

    IPAddress apip = WiFi.softAPIP();
    Serial.print("visit: \n");
    Serial.println(apip);
    server.on("/", handleNap);
    //server.on("/fah", handleFah);
    server.begin();
    Serial.println("HTTP server beginned");
}

void loop() {
    server.handleClient();
}

String htmlBody_Nap_Text ="";
String htmlBody_Nap_from = 
    "<form action='http://192.168.4.1/' method='POST'>"
      "<input type='text' name='valueForm'>"
      "<input type='submit' value='Submit'><br>"
    "</form>" ;

void handleNap(){
   String htmlBody_Nap_Head = 
    "<head>"
      "<meta http-equiv='Content-Language' content='th'> "
      "<meta http-equiv='content-Type' content='text/html; charset=window-874'> "
      "<meta http-equiv='content-Type' content='text/html; charset=tis-620'> "
    "</head>"
  
    "<h1>Nap Chat</h1><br/>\n"
      "<style>"
        "body{background-color: #000000; Color: #ffffff; }"
      "</style>"
  ;
  String getValueForm = server.arg("valueForm");
  htmlBody_Nap_Text += getValueForm;
  if(getValueForm.length() > 0){
    htmlBody_Nap_Text += "<br>"; 
  }
  htmlBody_Nap_Head += htmlBody_Nap_Text;
  htmlBody_Nap_Head += htmlBody_Nap_from;
  
  server.send(200, "text/html", htmlBody_Nap_Head);
  htmlBody_Nap_Head ="";
}

String htmlBody_Fah_Text ="";
String htmlBody_Fah_from = 
    "<form action='http://192.168.4.1/fah' method='POST'>"
      "<input type='text' name='valueFormFah'>"
      "<input type='submit' value='Submit'><br>"
    "</form>" ;

void handleFah(){
   String htmlBody_Fah_Head = 
    "<head>"
      "<meta http-equiv='Content-Language' content='th'> "
      "<meta http-equiv='content-Type' content='text/html; charset=window-874'> "
      "<meta http-equiv='content-Type' content='text/html; charset=tis-620'> "
    "</head>"
  
    "<h1>Fah Chat</h1><br/>\n"
      "<style>"
        "body{background-color: #000000; Color: #ffffff; }"
      "</style>"
  ;
  String getValueFormFah = server.arg("valueFormFah");
  htmlBody_Fah_Text += getValueFormFah;
  if(getValueFormFah.length() > 0){
    htmlBody_Fah_Text += "<br>"; 
  }
  htmlBody_Fah_Head += htmlBody_Fah_Text;
  htmlBody_Fah_Head += htmlBody_Fah_from;
  
  server.send(200, "text/html", htmlBody_Fah_Head);
  htmlBody_Fah_Head ="";
}
 
