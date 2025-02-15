#include <UbiConstants.h>
#include <UbidotsEsp32Mqtt.h>
#include <UbiTypes.h>
#include <stdio.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "DHT.h"

#define DHTPIN 27
#define DHTTYPE DHT11
#define PIN_SW3 33  // Pin controlado por sw3

#define MI_ABS(x) ((x) < 0 ? -(x) : (x))

TFT_eSPI tft = TFT_eSPI();
DHT dht(DHTPIN, DHTTYPE);

const char *UBIDOTS_TOKEN = "BBUS-ucZRBkQA3URwItPv0fsaWc7HEWZJVn";
const char *WIFI_SSID = "Gomez";
const char *WIFI_PASS = "3108329470";
const char *DEVICE_LABEL = "esp32";
const char *VARIABLE_LABEL1 = "sw1";
const char *VARIABLE_LABEL2 = "sw2";
const char *VARIABLE_LABEL3 = "sw3";
const char *VARIABLE_LABEL4 = "Temp";
const char *VARIABLE_LABEL5 = "Hum";

const int PUBLISH_FREQUENCY = 5000;
unsigned long timer;

Ubidots ubidots(UBIDOTS_TOKEN);

bool sw1State = false;
bool sw2State = false;
bool sw3State = false;

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  char boton = topic[strlen(topic) - 4];
  bool estado = (payload[0] == '1');

  if (boton == '1') {
    sw1State = estado;
    tft.fillCircle(35, 100, 10, estado ? TFT_GREEN : TFT_DARKGREY);
  }
  if (boton == '2') {
    sw2State = estado;
    tft.fillCircle(145, 100, 10, estado ? TFT_RED : TFT_DARKGREY);
  }
  if (boton == '3') {
    sw3State = estado;
    digitalWrite(PIN_SW3, estado ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_SW3, OUTPUT);
  digitalWrite(PIN_SW3, LOW);

  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();

  tft.init();
  tft.fillScreen(TFT_LIGHTGREY);
  tft.setRotation(1);
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.drawString("Realizado por:", 10, 5, 2);
  tft.setTextColor(TFT_BLUE, TFT_LIGHTGREY);
  tft.drawString("Dario", 50, 35, 4);
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.drawString("Temperatura", 25, 70, 2);
  tft.drawString("Humedad", 150, 70, 2);

  tft.fillCircle(35, 100, 10, TFT_DARKGREY);
  tft.fillCircle(145, 100, 10, TFT_DARKGREY);

  ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL1);
  ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL2);
  ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL3);
  
  dht.begin();
  timer = millis();
}

void loop() {
  if (!ubidots.connected()) {
    ubidots.reconnect();
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL1);
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL2);
    ubidots.subscribeLastValue(DEVICE_LABEL, VARIABLE_LABEL3);
  }

  float Hum = dht.readHumidity();
  float Temp = dht.readTemperature();

  if ((MI_ABS(millis() - timer)) > PUBLISH_FREQUENCY) {
    Serial.print("Temperatura: ");
    Serial.print(Temp);
    Serial.print("Humedad: ");
    Serial.println(Hum);

    tft.drawString(String(Hum), 160, 100);
    tft.drawString(String(Temp), 50, 100);
    ubidots.add(VARIABLE_LABEL4, Temp);
    ubidots.add(VARIABLE_LABEL5, Hum);
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }

  ubidots.loop();
}
