//************************************************************
// this is a simple example that uses the painlessMesh library to
// connect to a another network and relay messages from a MQTT broker to the nodes of the mesh network.
// To send a message to a mesh node, you can publish it to "painlessMesh/to/12345678" where 12345678 equals the nodeId.
// To broadcast a message to all nodes in the mesh you can publish it to "painlessMesh/to/broadcast".
// When you publish "getNodes" to "painlessMesh/to/gateway" you receive the mesh topology as JSON
// Every message from the mesh which is send to the gateway node will be published to "painlessMesh/from/12345678" where 12345678 
// is the nodeId from which the packet was send.
//************************************************************
#include <Arduino.h>
#include <painlessMesh.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#include <sstream>
#include <vector>


#define   MESH_PREFIX     "TanguayLight"
#define   MESH_PASSWORD   "NotSoSecret!"
#define   MESH_PORT       5555
#define   STATION_SSID     "SuperSbap"
#define   STATION_PASSWORD "bazinga!"
#define   STATION_CHANNEL 1
#define HOSTNAME "MQTT_Bridge"
// Prototypes
void receivedCallback( const uint32_t &from, const String &msg );
void mqttCallback(char* topic, byte* payload, unsigned int length);
IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);
IPAddress mqttBroker(192, 168, 1, 101); // MQTT server ip
painlessMesh  mesh;
WiFiClient wifiClient;
PubSubClient mqttClient(mqttBroker, 1883, mqttCallback, wifiClient);
void setup() {
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, STATION_CHANNEL );

  mesh.onReceive(&receivedCallback);
  mesh.stationManual(STATION_SSID, STATION_PASSWORD);
  mesh.setHostname(HOSTNAME);
  // Bridge node, should (in most cases) be a root node. See [the wiki](https://gitlab.com/painlessMesh/painlessMesh/wikis/Possible-challenges-in-mesh-formation) for some background
  mesh.setRoot(true);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);

  mqttClient.setBufferSize(2048);

  Serial.begin(115200);
	delay(50);
  Serial.printf("nodeID:%u\n", mesh.getNodeId());

}

std::vector<String> split (const String &s, char delim) {
    std::vector<String> result;
    std::stringstream ss (s.c_str());
    std::string item;

    while (getline (ss, item, delim)) {
        result.push_back (item.c_str());
    }

    return result;
}


void loop() {
  mesh.update();
  mqttClient.loop();
  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
	  Serial.printf("My ID: %d", String(mesh.getNodeId()));
  }

  // We are online, but not connected to MQTT broker
  if (myIP != IPAddress(0,0,0,0) && mqttClient.state() != 0) {
    Serial.println("Attempting to connect to MQTT...");
    if (mqttClient.connect(MESH_PREFIX)) {
      Serial.println("Success!");
      mqttClient.publish("painlessMesh/from/gateway","Ready!");
      mqttClient.subscribe("painlessMesh/to/#");
    } else {
      Serial.printf("failed to connect to mqtt broker. error code: %d \n", mqttClient.state());
    }
  }
}

void receivedCallback( const uint32_t &from, const String &msg ) {
  Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
  String topic = "painlessMesh/from/" + String(from);
  mqttClient.publish(topic.c_str(), msg.c_str());
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  Serial.printf("bridge: Received MQTT length=%d to topic=%s\n", length, topic);
	char* cleanPayload = (char*)malloc(length+1);
  memcpy(cleanPayload, payload, length);
  cleanPayload[length] = '\0';
  String msg = String(cleanPayload);
  free(cleanPayload);

   std::vector<String> v = split (topic, '/');

  //String targetStr = String(topic).substring(16);
  String targetStr = v[2];

  Serial.printf("bridge: Received MQTT msg=%s to topic=%s\n with target=%s", msg.c_str(), topic, targetStr.c_str());

  if(targetStr == "gateway")
  {
    if(msg == "getNodes")
    {
      auto nodes = mesh.getNodeList(true);
      auto tree = mesh.subConnectionJson();
      String str;
      for (auto &&id : nodes)
        str += String(id) + String(" ");
      mqttClient.publish("painlessMesh/from/gateway", tree.c_str());
    }
  }
  else if(targetStr == "broadcast") 
  {
    mesh.sendBroadcast(msg);
  }
  else
  {
    uint32_t target = strtoul(targetStr.c_str(), NULL, 10);
    if(mesh.isConnected(target))
    {
      mesh.sendSingle(target, msg);
    }
    else
    {
      mqttClient.publish("painlessMesh/from/gateway", "Client not connected!");
    }
  }
}
IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}