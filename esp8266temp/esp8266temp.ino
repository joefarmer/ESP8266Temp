#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <OneWire.h>

OneWire  ds(2);
DallasTemperature sensors(&ds);
int sensorCount = 0;
int status = WL_IDLE_STATUS;

// ThingSpeak Settings
char thingSpeakAddress[] = "api.thingspeak.com";
String writeAPIKey = "YOURAPIKEY";
const int updateThingSpeakInterval = 30 * 1000;      // Time interval in milliseconds to update ThingSpeak (number of seconds * 1000 = interval)

// Variable Setup
long lastConnectionTime = 0; 
boolean lastConnected = false;
int failedCounter = 0;

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  
  sensors.begin();
  sensorCount = sensors.getDeviceCount();  
}

void loop() {
 // Update ThingSpeak
  //if(millis() - lastConnectionTime > updateThingSpeakInterval)
  //{
    if ( status != WL_CONNECTED) { 
      Serial.println("Couldn't get a wifi connection");
      WiFi.printDiag(Serial);
      status = WiFi.begin("YOURSSID", "YOURPASSWORD");
    } 
    else {
      WiFiClient client;
      while(!client.connect(thingSpeakAddress, 80)) {
        Serial.print(".");
      }
      String temps = readTemps();
      
      // Make a HTTP request:
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(temps.length());
      client.print("\n\n");
      client.print(temps);

      client.stop();
      client.flush();
      Serial.println(temps);
      lastConnectionTime = millis();
    }
//}

  delay(updateThingSpeakInterval);              // wait for a second
}

String readTemps() 
{ 
  String temp;
  sensors.requestTemperatures(); // Send the command to get temperatures

  for(int i = 0;i < sensorCount; i++ )
  {
    temp += "field"; 
    temp += (i+1);
    temp += "=";
    temp += sensors.getTempFByIndex(i);
    temp += "&"; 
  }

  return temp;
}

















