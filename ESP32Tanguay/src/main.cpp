#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include "StripMethods.cpp"

#include <ESPAsyncWebServer.h>

const char *ssid = "Hornblende";
const char *password = "minuscule";

const char* PARAM_MESSAGE = "message";

AsyncWebServer server(80);


void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
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

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "Hello, world");
  });

  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
      String message;
      if (request->hasParam(PARAM_MESSAGE)) {
          message = request->getParam(PARAM_MESSAGE)->value();
      } else {
          message = "No message sent";
      }
      request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
      String message;
      if (request->hasParam(PARAM_MESSAGE, true)) {
          message = request->getParam(PARAM_MESSAGE, true)->value();
          if (message == "Red")
            SetOnboardLED(CRGB::Red);
          if (message == "Blue")
            SetOnboardLED(CRGB::Blue);
          if (message == "Green")
            SetOnboardLED(CRGB::Green);
      } else {
          message = "No message sent";
      }
      request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.onNotFound(notFound);

  server.begin();
}



void loop() {
}

