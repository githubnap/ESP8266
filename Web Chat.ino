
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
extern "C" {
#include "user_interface.h"
}

const char *ssid = "NapChat(192.168.4.1)";
const char *password = "startchat";

ESP8266WebServer server(80);

void setup() {
      struct softap_config config;
      wifi_softap_get_config(&config); // Get config first.
    
    ESP.eraseConfig();
    delay(1000);
    wifi_set_phy_mode(PHY_MODE_11B);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    IPAddress apip = WiFi.softAPIP();

      config.ssid_len = 0;// or its actual length
      config.beacon_interval = 100;
      config.max_connection = 8;
      wifi_softap_set_config(&config);// Set ESP8266 softap config
    
    server.on("/", handleNap);
    server.begin();

}

void loop() {
    server.handleClient();
}

String htmlBody_Nap_Text ="";
String htmlBody_Nap_from = 
    "<form action='http://192.168.4.1/' method='POST' id='down'>"
      "<input type='text' name='valueForm'>"
      "<input type='submit' value='Submit'><br>"
    "</form>" ;

void handleNap(){
  if ( htmlBody_Nap_Text.length() > 1024*3) {
    htmlBody_Nap_Text = htmlBody_Nap_Text.substring(htmlBody_Nap_Text.length() - 1024);
   }
  
   String htmlBody_Nap_Head = 
    "<head>"
      "<meta http-equiv='Content-Language' content='th'> "
      "<meta http-equiv='content-Type' content='text/html; charset=window-874'> "
      "<meta http-equiv='content-Type' content='text/html; charset=tis-620'> "
    "</head>"
  
    "<h1>Nap Chat</h1>  <a href='#down'>GoToDown</a>  <br/>\n"
      "<style>"
        "body{background-color: #000000; Color: #ffffff; }"
      "</style>"
  ;
  String a = "abcd";
  
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
