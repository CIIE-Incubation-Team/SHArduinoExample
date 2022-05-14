/**
   PostHTTPClient.ino

    Created on: 21.11.2016

*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

/* this can be run with an emulated server on host:
        cd esp8266-core-root-dir
        cd tests/host
        make ../../libraries/ESP8266WebServer/examples/PostServer/PostServer
        bin/PostServer/PostServer
   then put your PC's IP address in SERVER_IP below, port 9080 (instead of default 80):
*/
//#define SERVER_IP "10.0.1.7:9080" // PC address with emulation on host
#define SERVER_IP "smart-h-backend.herokuapp.com"

#ifndef STASSID
#define STASSID "Adi"
#define STAPSK  "lpacafcs"
#endif

void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D7,OUTPUT);
  pinMode(D8,OUTPUT);
}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    WiFiClient client;
    HTTPClient http;

    Serial.print("[HTTP] begin login...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/login"); //HTTP
    http.addHeader("Content-Type", "application/json");

    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST("{\"user\":\"bhardwaj.aditya99@gmail.com\",\"password\":\"lpacafcs\"}");

    // httpCode will be negative on error
    if (httpCode > 0) {
      StaticJsonDocument<10000> doc;
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }
      }

      String token = doc["data"]["jwt"];
      Serial.println(token);
    Serial.println("[HTTP] POST \\login end");
    Serial.print("[HTTP] begin fetchDevices...\n");
    // configure traged server and url
    http.begin(client, "http://" SERVER_IP "/fetchDevices"); //HTTP
    http.addHeader("Content-Type", "application/json");
    http.addHeader("token", token);
    Serial.print("[HTTP] POST...\n");
    // start connection and send HTTP header and body
    int httpCode = http.POST("");

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
      }

      
      for(int i=0;i<4;i++){
        bool state = doc["data"][i]["state"].as<bool>();
        int varState = doc["data"][i]["varState"].as<int>();
        String app = doc["data"][i]["app"];
        Serial.print(app+" ");
        Serial.print(varState);
        Serial.println(" "+state);
        if(doc["data"][i]["state"]){
          if(doc["data"][i]["app"]=="fan1"){
            int val1 = (varState);
            Serial.println(val1);
            analogWrite(D5,val1);
          } else if(doc["data"][i]["app"]=="fan2"){
            int val2 = (varState);
            Serial.println(val2);
            analogWrite(D6,val2);
          } else if(doc["data"][i]["app"]=="light1"){
            int val3 = (varState);
            Serial.println(val3);
            analogWrite(D7,val3);
          } else if(doc["data"][i]["app"]=="light2"){
            int val4 = (varState);
            Serial.println(val4);
            analogWrite(D8,val4);
          }
        } else {
          if(doc["data"][i]["app"]=="fan1"){
            analogWrite(D5,0);
          } else if(doc["data"][i]["app"]=="fan2"){
            analogWrite(D6,0);
          } else if(doc["data"][i]["app"]=="light1"){
            analogWrite(D7,0);
          } else if(doc["data"][i]["app"]=="light2"){
            analogWrite(D8,0);
          }
          }
      }
      
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}
}
