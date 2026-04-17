#include <Arduino.h>
#include <FastLED.h>
#include "StripMethods.cpp"
#include <ArduinoJson.h>
#include <exception>

#define MESH_PREFIX "TanguayLight"
#define MESH_PASSWORD "0YpRu)48jqIb"
#define MESH_PORT 5555
#define   STATION_CHANNEL 1

#include <painlessMesh.h>

Scheduler userScheduler;
painlessMesh mesh;

bool rainbow = true;

bool onboardLEDOn = true;
#define ONBOARD_LED 2

#define REDPIN   4
#define GREENPIN 19
#define BLUEPIN  23

#define LED_ACTIVE true

#define Brightness 0.5

bool connected = false;

String state = "off";
bool looping = true;

JsonDocument doc;

int patternIndex = 0;
unsigned long patternTimer = 0;

void newConnectionCallback(uint32_t nodeId)
{
	Serial.printf("New connection, nodeId = %u", nodeId);
}


void BeginStrips()
{
    pinMode(REDPIN,   OUTPUT);
    pinMode(GREENPIN, OUTPUT);
    pinMode(BLUEPIN,  OUTPUT);
}

// This function takes the incoming RGB values and outputs the values
// on three analog PWM output pins to the r, g, and b values respectively.
void showAnalogRGB( const CRGB& rgb)
{
  analogWrite(REDPIN,   rgb.r * Brightness);
  analogWrite(GREENPIN, rgb.g * Brightness);
  analogWrite(BLUEPIN,  rgb.b * Brightness);
}

void deserializeJsonAndApply(String& json) {
	DeserializationError error = deserializeJson(doc, json);
	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	} else {
		Serial.println("JSON received successfully");
		try {				
			state = doc["state"].as<String>();
			looping = doc["looping"].as<bool>(); 

			patternIndex = 0;
		} catch (const std::exception &e) {
			Serial.print(F("Exception during JSON processing: "));
			Serial.println(e.what());
		} catch (...) {
			Serial.println(F("Unknown exception caught during JSON processing"));
		}
	}
}

void receivedCallback(uint32_t from, String &msg)
{

	if (Serial) {
		Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
	}
	
	if (msg == "r") {
		showAnalogRGB(CRGB::Red);
	}
	else if (msg == "g") {
		showAnalogRGB(CRGB::Green);
	}
	else if (msg == "b") {
		showAnalogRGB(CRGB::Blue);
	}
	else if (msg == "w") {
		showAnalogRGB(CRGB::White);
	}
	else {
		deserializeJsonAndApply(msg);
	}
}

CRGB Wheel(byte WheelPosition) {
  static byte c[3];
 
  if(WheelPosition < 85) {
   c[0] = WheelPosition * 3;
   c[1] = 255 - WheelPosition * 3;
   c[2] = 0;
  }
  else if(WheelPosition < 170) {
   WheelPosition -= 85;
   c[0] = 255 - WheelPosition * 3;
   c[1] = 0;
   c[2] = WheelPosition * 3;
  }
  else {
   WheelPosition -= 170;
   c[0] = 0;
   c[1] = WheelPosition * 3;
   c[2] = 255 - WheelPosition * 3;
  }

  
  return CRGB(c[0], c[1], c[2]);
}

void Rainbow() {

	CRGB hue = Wheel(millis() / 10);
    // Use FastLED automatic HSV->RGB conversion
    showAnalogRGB(hue);
}



void PulseWhite() {

	int x = (millis() / 2) % 510;
	if (x <= 255) {
		showAnalogRGB(CRGB(x, x, x));
	}
	else {
		showAnalogRGB(CRGB(255 - x, 255 - x, 255 - x));
	}

	delay(20);
}

void setup()
{
	if (LED_ACTIVE) {
		BeginStrips();
	}

	// put your setup code here, to run once:
	Serial.begin(115200);
	delay(50);
	Serial.println("Hello world");

	pinMode(ONBOARD_LED, OUTPUT);




	mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT, WIFI_AP_STA, STATION_CHANNEL);
	mesh.onReceive(&receivedCallback);
	mesh.onNewConnection(&newConnectionCallback);

	if (Serial) {
		mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION | MESH_STATUS);

	}



	showAnalogRGB(CRGB::White);
	delay(500);
	showAnalogRGB(CRGB::Black);

	// Serial.println("White");
	//Serial.println("Setup complete!");
}

void loop()
{
	// reboot esp32 after 5 days, to prevent memory leaks and other issues that may arise from long uptimes
	if (millis() > 432000000) {
		ESP.restart();
	}

	mesh.update();
	userScheduler.execute();

	// Fade white when not inside mesh AKA
	// TODO check to change with a  root note check
	if (mesh.getNodeList().size() == 0) {
		PulseWhite();
		connected = false;
	} else if (!connected) {
		showAnalogRGB(CRGB::Green);
		delay(200);
		showAnalogRGB(CRGB::Black);

		connected = true;
	}


	if (millis() > patternTimer && state == "playing") {
		// Timer elapsed, load next color from pattern
		JsonArray pattern = doc["pattern"].as<JsonArray>();

		
		if (patternIndex >= pattern.size()) {
			if (looping) {
				patternIndex = 0;
			} else {
				state = "off";
				showAnalogRGB(CRGB::Black);
			}
		}

		if (state == "playing") {
			JsonObject color = pattern[patternIndex];
			CRGB rgb(color["red"].as<int>(), color["green"].as<int>(), color["blue"].as<int>());
			showAnalogRGB(rgb);
			int ms = color["ms"].as<int>();
			patternTimer = millis() + ms;
		}

		patternIndex = patternIndex + 1;
	}

	if (Serial.available())
	{
		// Serial.println(Serial.available());

		char c = Serial.peek();
		// example
		// t{"state":"playing","looping":true,"pattern":[{"red":255,"green":255,"blue":255,"ms":1000},{"red":255,"green":0,"blue":0,"ms":1000},{"red":0,"green":255,"blue":0,"ms":1000},{"red":0,"green":0,"blue":255,"ms":1000}, {"red":100,"green":255,"blue":60,"ms":200},{"red":20,"green":111,"blue":255,"ms":200}]}

		if (c == 'j')
			Serial.println(mesh.subConnectionJson());
		else if (c == 't') {
			Serial.read();
			delay(50);
			String s = Serial.readString();
			deserializeJsonAndApply(s);

		}
		else
			mesh.sendBroadcast(String(c), true);


	}

}
