#include <Arduino.h>
#include <FastLED.h>
#include "StripMethods.cpp"

#define MESH_PREFIX "TanguayLight"
#define MESH_PASSWORD "0YpRu)48jqIb"
#define MESH_PORT 5555

#include <painlessMesh.h>

Scheduler userScheduler;
painlessMesh mesh;

bool rainbow = true;

int patternIndex = 0;
bool playPattern = false;

bool onboardLEDOn = true;
#define ONBOARD_LED 2

#define REDPIN   4
#define GREENPIN 19
#define BLUEPIN  23

#define LED_ACTIVE true

#define Brightness 0.5

bool connected = false;

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

void receivedCallback(uint32_t from, String &msg)
{

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




	mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
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
	// Rainbow();
	// delay(20);

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


	if (millis() % 1000 == 0) {

		
	}

	if (Serial.available())
	{
		// Serial.println(Serial.available());

		char c = Serial.read();

		if (c == 'j')
			Serial.println(mesh.subConnectionJson());
		else
			mesh.sendBroadcast(String(c), true);
		// if (c == 'r')
		// 	mesh.sendBroadcast("r", true);
		// else if (c == 'g')
		// 	mesh.sendBroadcast("g", true);
		// else if (c == 'b')
		// 	mesh.sendBroadcast("b", true);
		// else if (c == 'w')
		// 	mesh.sendBroadcast("w", true);

	}

}
