#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include "StripMethods.cpp"
#include <WebServer.h>

const char *ssid = "Hornblende";
const char *password = "minuscule";

WebServer server(80);

void handleRoot() {
  String json = "{\"response:\" : \"Hello world\"}";
  
  server.send(200, "application/json", json);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(50);
  Serial.println("");
  WiFi.begin(ssid, password);

  BeginStrips();

  SetOnboardLED(CRGB::Yellow);

  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
      delay(500);
      Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());
  SetOnboardLED(CRGB::Green);

  server.on("/", handleRoot);
  

  server.begin();
  Serial.println("HTTP server started");
}



void loop() {
    server.handleClient();
}

