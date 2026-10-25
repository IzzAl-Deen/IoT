#include <WiFi.h>
#include "ThingSpeak.h"
#include <DHT.h>

const char* WIFI_Name = "Wokwi-GUEST";
const char* WIFI_Pass = "";

unsigned long channelNumber = 3549707;
const char* apiKey = "2XKP0ZU3GJ7ZWZ2F"; 
String UNIVERSITY_ID = "12324586";      

#define DHTPIN 15
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
WiFiClient clientObject;

unsigned long lastTime = 0;
const unsigned long timerDelay = 20000; 

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_Name, WIFI_Pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Status: Not Connected");
    delay(1000);
  }
  Serial.println("Status: Connected");

  ThingSpeak.begin(clientObject);
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {

      float t = dht.readTemperature();
      float h = dht.readHumidity();

      if (isnan(t) || isnan(h)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
      }

      Serial.printf("ID: %s | Temp: %.2f °C | Humidity: %.2f %%\n", 
                    UNIVERSITY_ID.c_str(), t, h);

      ThingSpeak.setField(1, t);
      ThingSpeak.setField(2, h);

      int responseCode = ThingSpeak.writeFields(channelNumber, apiKey);

      if (responseCode == 200) {
        Serial.println("ThingSpeak Update Successful");
      } else {
        Serial.println("Error updating ThingSpeak. HTTP Code: " + String(responseCode));
      }

    } else {
      Serial.println("WiFi Disconnected");
    }

    lastTime = millis();
  }
}