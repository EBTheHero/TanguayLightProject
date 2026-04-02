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

void receivedCallback(uint32_t from, String &msg)
{

	if (msg == "o")
		digitalWrite(ONBOARD_LED, HIGH);
	if (msg == "f")
		digitalWrite(ONBOARD_LED, LOW);

	Serial.println("Received from " + String(from) + ": " + msg.c_str());
}

void newConnectionCallback(uint32_t nodeId)
{
	Serial.printf("New connection, nodeId = %u", nodeId);
}

void setup()
{
	BeginStrips();
	SetBrightness(40);
	SetAllLED(CRGB::Yellow);

	// put your setup code here, to run once:
	Serial.begin(115200);
	delay(50);
	Serial.println("Hello world");




	pinMode(ONBOARD_LED, OUTPUT);


	if (Serial)
		mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);
	// mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
	// mesh.onReceive(&receivedCallback);
	// mesh.onNewConnection(&newConnectionCallback);


	SetAllLED(CRGB::White);
	//Serial.println("Setup complete!");
}

void loop()
{
	Rainbow();

	// mesh.update();
	// userScheduler.execute();

	// if (millis() % 1000 == 0) {

	// 	list<uint32_t> nodeList = mesh.getNodeList();

	// 	int size = nodeList.size();

	// 	if (Serial)
	// 		Serial.println("Mesh Size: " + size);
	// }

	// if (Serial.available())
	// {
	// 	Serial.println(Serial.available());

	// 	char c = Serial.read();

	// 	if (c == 'o')
	// 	{
	// 		mesh.sendBroadcast("o", true);
	// 	}
	// 	if (c == 'f')
	// 	{
	// 		mesh.sendBroadcast("f", true);
	// 	}
	// }

	// int thousands = millis() % 1000;

	// if (thousands <= 500)
	// 	digitalWrite(ONBOARD_LED, HIGH);
	// if (thousands >= 500)
	// 	digitalWrite(ONBOARD_LED, LOW);
}
