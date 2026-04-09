# python 3.11

import random
import time

from paho.mqtt import client as mqtt_client


broker = 'localhost'
port = 1883


topic = "painlessMesh/to/258100605"
# msg = '{"state":"playing","looping":true,"pattern":[{"red":255,"green":255,"blue":255,"ms":1000},{"red":255,"green":0,"blue":0,"ms":1000},{"red":0,"green":255,"blue":0,"ms":1000},{"red":0,"green":0,"blue":255,"ms":1000}, {"red":100,"green":255,"blue":60,"ms":200},{"red":20,"green":111,"blue":255,"ms":200}]}'

# blink red loop
msg = '{"state":"playing","looping":true,"pattern":[{"red":255,"green":0,"blue":0,"ms":500},{"red":0,"green":0,"blue":0,"ms":500}]}'


# blink red thrice
# msg = '{"state":"playing","looping":false,"pattern":[{"red":255,"green":0,"blue":0,"ms":100},{"red":0,"green":0,"blue":0,"ms":100},{"red":255,"green":0,"blue":0,"ms":100},{"red":0,"green":0,"blue":0,"ms":100},{"red":255,"green":0,"blue":0,"ms":100},{"red":0,"green":0,"blue":0,"ms":100}]}'

# topic = "painlessMesh/to/gateway"
# msg = "getNodes"


# Generate a Client ID with the publish prefix.
client_id = f'publish-{random.randint(0, 1000)}'
# username = 'emqx'
# password = 'public'

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
    # For paho-mqtt 2.0.0, you need to add the properties parameter.
    # def on_connect(client, userdata, flags, rc, properties):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)
    
    # For paho-mqtt 2.0.0, you need to set callback_api_version.
    client = mqtt_client.Client(client_id=client_id, callback_api_version=mqtt_client.CallbackAPIVersion.VERSION2)
    
    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def publish(client):

    time.sleep(1)
    result = client.publish(topic, msg)
    # result: [0, 1]
    status = result[0]
    if status == 0:
        print(f"Send `{msg}` to topic `{topic}`")
    else:
        print(f"Failed to send message to topic {topic}")



def run():
    client = connect_mqtt()
    client.loop_start()
    publish(client)
    client.loop_stop()


if __name__ == '__main__':
    run()