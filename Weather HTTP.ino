
#include "WiFi.h"
#include "HTTPClient.h"
#include <ArduinoJson.h>
const char* ssid = "Tgeo";
const char* password = "90206087";
String sensorData = "";
bool ser = false;

int cnt;
void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.print(" Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
cnt=62;
}

void loop() {
  /*
  while (Serial.available())
  {
    //Serial.println(cnt);
    sensorData.concat(char(Serial.read()));
    ser = true;
    cnt=cnt+1;
  }
  */
  if (cnt==62)
  {
    StaticJsonDocument<200> doc;
    // Add values in the document
    //
    doc["temp"] = 123.11;
    doc["hum"] = 1351824120;
    //Serial.println("Data to be sent : ");
    //Serial.println(sensorData);
    String requestBody;
    serializeJson(doc, requestBody);
    Serial.println("Data");
    if (WiFi.status() == WL_CONNECTED) {

      HTTPClient http;

      http.begin("http://192.168.1.5:8000/api/");
      delay(1000);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(requestBody);

      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode);
        Serial.println(response);
      }
      else {
        Serial.print("Error on sending PUT Request: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    } else {
      Serial.println("Error in WiFi connection");
    }
    sensorData = "";
    //cnt=0;
  }
}
