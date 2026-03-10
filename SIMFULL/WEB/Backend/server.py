from flask import Flask, jsonify, request, render_template
import paho.mqtt.client as mqtt

# endereço do broker (seu ESP32)
BROKER = "192.168.15.42"
PORT = 1883

app = Flask(__name__)

# variáveis do sistema
data = {
    "humidity": 0,
    "n": 0,
    "p": 0,
    "k": 0,
    "pump": 0
}

# conexão MQTT
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")

    client.subscribe("soil/umidity")
    client.subscribe("soil/n")
    client.subscribe("soil/p")
    client.subscribe("soil/k")

def on_message(client, userdata, msg):

    value = float(msg.payload.decode())

    if msg.topic == "soil/umidity":
        data["humidity"] = value

    elif msg.topic == "soil/n":
        data["n"] = value

    elif msg.topic == "soil/p":
        data["p"] = value

    elif msg.topic == "soil/k":
        data["k"] = value

    print(msg.topic, value)

mqtt_client = mqtt.Client()

mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
