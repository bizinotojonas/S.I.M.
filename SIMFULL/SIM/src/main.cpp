#include <arduino.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <PubSubClient.h>
#include<sMQTTBroker.h>
#include <WiFi.h>
#include <MQTT.h>
//ip esp 192.168.15.42
//MQTT Broker

const char* ssid = "Jonas";
const char* password = "99843512jo";

MyBroker broker;


//MQTT Receptor

class MyBroker : public sMQTTBroker {

public:

  bool onMessage(sMQTTEvent *event, const char* topic, const char* payload, uint32_t length) {

    Serial.print("Topico: ");
    Serial.println(topic);

    Serial.print("Payload: ");

    for(int i=0;i<length;i++){
      Serial.print((char)payload[i]);
    }
    Serial.println();

    // verificar tópico da bomba
    if(strcmp(topic,"pump/control") == 0)
    {
      mqttreceive(payload,length);
    }

    return true;
  }
};

#define PUMP 14
#define UmiD 13
#define UmiA 32
#define col 16
#define lin 2
#define ende 0x27

LiquidCrystal_I2C
lcd(ende,col,lin);

//declaração de funções
void pumpcontroldigital();
void pumpcontrolanalog();
void displaywrite();
void umiread();
void RNPKG();
void mqttsend();
void mqttreceive(const char* payload, uint32_t length);
void pumpcontrol();

float n, p, k, umidityanalog, refumi;
bool umidity, pumporder;
int lastSend = 0;
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  pinMode(UmiD, INPUT);
  pinMode(UmiA, INPUT);
  pinMode(PUMP, OUTPUT);

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  

  Serial.print("Conectando ao WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  broker.init(1883, false);
}

void loop() {
  umiread();
  displaywrite();
  pumpcontrolanalog();
  if (millis() - lastSend > 5000) {
    mqttsend();
    lastSend = millis();
  }
  pumpcontrol();
  broker.update();
  RNPKG();
}


// put function definitions here:

void displaywrite()
{
  // escrever no diplay lcd
  lcd.setCursor(0, 0);
  lcd.print("N:");
  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.setCursor(8, 1);
  lcd.print("K:");
  delay(750);
}
void umiread()
{
  // leitura dos sensores
  umidity = digitalRead(UmiD);
  umidityanalog = analogRead(UmiA);
  refumi = map(umidityanalog, 0, 4095, 0, 1000)/10;

}
void pumpcontrol()
{
  digitalWrite (PUMP, pumporder);
}
void RNPKG()
{
  n = float(random(0, 100));
  p = float(random(0, 100));
  k = float(random(0, 100));
  delay(1000);
}
void pumpcontroldigital()
{
  if (umidity == LOW)
  {
    pumporder = HIGH;
  }
  else
  {
    pumporder = LOW;
  }
}
void pumpcontrolanalog()
{
  if (refumi < 30)
  {
    pumporder = HIGH;
  }
  else
  {
    pumporder = LOW;
  }
}
void mqttsend(){
  //send data to web
  char msg[20];

  // Umidade
  sprintf(msg, "%.2f", refumi);
  broker.publish("soil/umidity", msg);

  // Nitrogenio
  sprintf(msg, "%.2f", n);
  broker.publish("soil/n", msg);

  // Fosforo
  sprintf(msg, "%.2f", p);
  broker.publish("soil/p", msg);

  // Potassio
  sprintf(msg, "%.2f", k);
  broker.publish("soil/k", msg);
}
void mqttreceive(const char* payload, uint32_t length){
  //receive data from web
  if (length == 1)
  {
    if (payload[0] == '1')
    {
      pumporder = HIGH;
    }
    else if (payload[0] == '0')
    {
      pumporder = LOW;
    }
  }
}
