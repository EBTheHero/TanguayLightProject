#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include "StripMethods.cpp"
#include <ArduinoJSON.h>

#include <ESPAsyncWebServer.h>

const char *ssid = "Hornblende";
const char *password = "minuscule";

const char *PARAM_MESSAGE = "json";

AsyncWebServer server(80);

bool rainbow = true;

int patternIndex = 0;
bool playPattern = false;

JsonArray pattern = {};

void notFound(AsyncWebServerRequest *request)
{
	request->send(404, "text/plain", "Not found");
}

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(9600);
	delay(50);
	Serial.println("Initializing LED...");

	
	BeginStrips();


	WiFi.begin(ssid, password);
	Serial.println("Connecting to WiFi...");
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}



	SetBrightness(50);
	SetAllLED(CRGB::Yellow);


	Serial.println("\nConnected to WiFi");

	SetAllLED(CRGB::Green);

	Serial.print("IP Address: ");
	Serial.println(WiFi.localIP());

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/plain", "Hello, world"); });

	// Send a GET request to <IP>/get?message=<message>
	server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
      String message;
      if (request->hasParam(PARAM_MESSAGE)) {
          message = request->getParam(PARAM_MESSAGE)->value();
      } else {
          message = "No message sent";
      }
      request->send(200, "text/plain", "Hello, GET: " + message); });

	// Send a POST request to <IP>/post with a form field message set to <message>
	server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
			  {
      String message;
      if (request->hasParam(PARAM_MESSAGE, true)) {
		SetAllLED(CRGB::Black);
		delay(200);
		message = request->getParam(PARAM_MESSAGE, true)->value();
		JsonDocument myObject;
		deserializeJson(myObject, message);

		if (myObject.containsKey("pattern")) {
			pattern = myObject["pattern"].as<JsonArray>();
			playPattern = true;
		} else {
			SetAllLED(CRGB::Red);
		}

		rainbow = false;


      } else {
          message = "No message sent";
      }
      request->send(200, "text/plain", "Hello, POST: " + message); });

	server.onNotFound(notFound);

	server.begin();
}

void loop()
{
	if (rainbow)
		Rainbow();

	if (pattern)
	{

		String color = pattern[patternIndex].as<String>();

		if (color == "Green")
			SetAllLED(CRGB::Green);
		if (color == "Red")
			SetAllLED(CRGB::Red);
		if (color == "Blue")
			SetAllLED(CRGB::Blue);
		if (color == "Orange")
			SetAllLED(CRGB::Orange);

		patternIndex++;
		if (patternIndex > pattern.size())
			patternIndex = 0;

		delay(500);
	}
}
