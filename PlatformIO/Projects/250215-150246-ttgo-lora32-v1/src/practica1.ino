#include <TFT_eSPI.h>
#include <SPI.h>
#include "DHT.h"
#include <UbiConstants.h>
#include <UbidotsEsp32Mqtt.h>
#include <UbiTypes.h>

// Definir pines
#define DHTPIN 27
#define DHTTYPE DHT11
#define ANALOG_PIN 33

// Definir constantes de Ubidots
const char *UBIDOTS_TOKEN = "BBUS-ucZRBkQA3URwItPv0fsaWc7HEWZJVn";  // Put here your Ubidots TOKEN
const char *WIFI_SSID = "Gomez";      // Put here your Wi-Fi SSID
const char *WIFI_PASS = "3108329470";      // Put here your Wi-Fi password
const char *DEVICE_LABEL = "esp32";   // Poner aquí la etiqueta de tu dispositivo a la que se publicarán los datos
const char *VARIABLE_LABEL1 = "temp"; // temperatura
const char *VARIABLE_LABEL2 = "hum"; // humedad
const int PUBLISH_FREQUENCY = 5000; // Tasa de actualización en milisegundos

// Inicializar objetos
TFT_eSPI tft = TFT_eSPI();
DHT dht (DHTPIN, DHTTYPE);
Ubidots ubidots(UBIDOTS_TOKEN);

// Variables globales
unsigned long timer = 0;

void setup() {
  // Inicializar Serial para debug
  Serial.begin(115200);

 // Inicializar pantalla TFT
tft.init();
tft.fillScreen(TFT_LIGHTGREY); // Fondo cambiado a gris claro
tft.setRotation(1);
tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
tft.drawString("Realizado por:", 10, 5, 2);
tft.setTextColor(TFT_BLUE, TFT_LIGHTGREY); // Cambiado de verde a azul
tft.drawString("Dario_Gomez", 50, 35, 4);
tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
tft.drawString("Temperatura", 25, 70, 2);
tft.drawString("Humedad", 150, 70, 2);
  // Inicializar DHT
  Serial.println(F("DHTxx test!"));
  dht.begin();

  // Conexión a Wi-Fi y Ubidots
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setup();
  ubidots.reconnect();

  // Inicializar temporizador
  timer = millis();
}
void loop() {
  // Leer datos del sensor DHT
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  
  // Mostrar datos en pantalla TFT
  tft.drawString(String(hum), 160, 100);
  tft.drawString(String(temp), 50, 100);

  // Publicar datos en Ubidots
  if (millis() - timer > PUBLISH_FREQUENCY) {
    Serial.print("Temperatura: ");
    Serial.println(temp);
    Serial.print("Humedad: ");
    Serial.print(hum);
    ubidots.add(VARIABLE_LABEL1, temp);
    ubidots.add(VARIABLE_LABEL2, hum);
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  // Reconexion Ubidots
  if (!ubidots.connected()) {
    ubidots.reconnect();
  }
  // Procesar eventos Ubidots
  ubidots.loop();
}
