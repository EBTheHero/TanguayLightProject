# python 3.11

import random
import time

from paho.mqtt import client as mqtt_client

import paho.mqtt.subscribe as subscribe

broker = 'localhost'
port = 1883


topic = "painlessMesh/to/258100605"
# msg = '{"state":"playing","looping":true,"pattern":[{"red":255,"green":255,"blue":255,"ms":1000},{"red":255,"green":0,"blue":0,"ms":1000},{"red":0,"green":255,"blue":0,"ms":1000},{"red":0,"green":0,"blue":255,"ms":1000}, {"red":100,"green":255,"blue":60,"ms":200},{"red":20,"green":111,"blue":255,"ms":200}]}'

# blink red loop
msg = '{"state":"playing","looping":true,"pattern":[{"red":255,"green":0,"blue":0,"ms":500},{"red":0,"green":0,"blue":0,"ms":500}]}'
import paho.mqtt.client as mqtt 

def on_subscribe(client, userdata, mid, reason_code_list, properties):
    # Since we subscribed only for a single channel, reason_code_list contains
    # a single entry
    if reason_code_list[0].is_failure:
        print(f"Broker rejected you subscription: {reason_code_list[0]}")
    else:
        print(f"Broker granted the following QoS: {reason_code_list[0].value}")

def on_unsubscribe(client, userdata, mid, reason_code_list, properties):
    # Be careful, the reason_code_list is only present in MQTTv5.
    # In MQTTv3 it will always be empty
    if len(reason_code_list) == 0 or not reason_code_list[0].is_failure:
        print("unsubscribe succeeded (if SUBACK is received in MQTTv3 it success)")
    else:
        print(f"Broker replied with failure: {reason_code_list[0]}")
    client.disconnect()

def on_message(client, userdata, message: mqtt.MQTTMessage):
    # userdata is the structure we choose to provide, here it's a list()
    print(f"Message on {message.topic} : {message.payload}")



def on_connect(client, userdata, flags, reason_code, properties):
    if reason_code.is_failure:
        print(f"Failed to connect: {reason_code}. loop_forever() will retry connection")
    else:
        # we should always subscribe from on_connect callback to be sure
        # our subscribed is persisted across reconnections.
        client.subscribe("painlessMesh/from/#")

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message
mqttc.on_subscribe = on_subscribe
mqttc.on_unsubscribe = on_unsubscribe

mqttc.user_data_set({})
mqttc.connect(broker)
mqttc.loop_forever()
print(f"Received the following message: {mqttc.user_data_get()}")