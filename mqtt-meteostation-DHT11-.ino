#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient client(espClient);
String clientId = String(ESP.getChipId());

#define DHTPIN D2     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT11
DHT dht(DHTPIN, DHTTYPE);


int  delayMS = 10000;
long lastMsg = 0;
int  value = 0;        // Переменная для формирования публикуемого сообщения

// Функция обработки входящих сообщений
void callback(char* topic, byte* payload, unsigned int length) {
  // Печать информации о полученном сообщенийй
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);

  }
  Serial.println();
}


void setup() {
  
  Serial.begin(115200);
  Serial.setTimeout(2000);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);



  
  // DHT11
 // sensor_t sensor;
 // dht.begin(); 
 // dht.temperature().getSensor(&sensor);
 // dht.humidity().getSensor(&sensor);
}


void report(double humidity, double tempC) {
  StaticJsonBuffer<500> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["humidity"] = humidity;
  root["tempC"] = tempC;
  char payload[256];
  root.printTo(payload, sizeof(payload));
  char* s = payload;
  client.publish("esp8266/32/led", s);
  Serial.println("Отправлено!");
}

int timeSinceLastRead = 0;







void loop() {
  if (!client.connected()) {
    reconnect();
  }


 if(timeSinceLastRead > 5000) {

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      Serial.println("Данных нет! Останавливаем цикл и запускаем по новой");
      timeSinceLastRead = 0;
      return;
    }

    report(h, t); // Функция отправки данных
  
    timeSinceLastRead = 0;
  }
  
  delay(100);
  timeSinceLastRead += 100;



  
  client.loop();


  
}
