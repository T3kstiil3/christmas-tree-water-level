#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include "FS.h"
    
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

WiFiServer server(80);
String html_home = "";
String water_message = "";
const int read = A0; // Sensor AO pin to Arduino pin A0
int value; // Variable to store the incomming data

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("Christmas tree Wifi")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    // start a server
    server.begin();
    Serial.println("Server started");
    prepareFiles();
}

void loop() {
  // put your main code here, to run repeatedly:

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  
  // Read data from analog pin
  value = analogRead(read); 

  // Match the request 
  if (request.indexOf("/tree") != -1)  {
    client.println('{"tree":"ok"}');
  }else if (request.indexOf("/level") != -1)  {
    client.println(value);
  }else {
    client.println(html_home);
  }

  delay(100);
}


void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void getWaterLevel() {
  
  if (value<=480){
    Serial.println("Water level: 0mm - Empty!"); 
  }
  else if (value>480 && value<=530){ 
    Serial.println("Water level: 0mm to 5mm"); 
  }
  else if (value>530 && value<=615){ 
    Serial.println("Water level: 5mm to 10mm"); 
  }
  else if (value>615 && value<=660){ 
    Serial.println("Water level: 10mm to 15mm"); 
  } 
  else if (value>660 && value<=680){ 
    Serial.println("Water level: 15mm to 20mm"); 
  }
  else if (value>680 && value<=690){ 
    Serial.println("Water level: 20mm to 25mm"); 
  }
  else if (value>690 && value<=700){ 
    Serial.println("Water level: 25mm to 30mm"); 
  }
  else if (value>700 && value<=705){ 
    Serial.println("Water level: 30mm to 35mm"); 
  }
  else if (value>705){ 
    Serial.println("Water level: 35mm to 40mm"); 
  }
  
}

void prepareFiles(){
  
  Serial.println("Prepare file system");
  SPIFFS.begin();
  
  File file = SPIFFS.open("/index.html", "r");
  if (!file) {
    Serial.println("File open index failed");  
  } else{
    Serial.println("File open index success");
    html_home = "";
    while (file.available()) {
      String line = file.readStringUntil('\n');
      html_home += line + "\n";
    }
    file.close();
  }
}
