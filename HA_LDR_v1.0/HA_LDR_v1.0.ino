/*
This code uses a LDR sensor wired up to an esp8266 module to sense ambient light itensity in a room. It then relays this information 
back to the home assistant server using mqtt connection. The LDR sensor is connected to one of the analog inputs (A0) of the 
microcontroller.

Credits @mertenats (https://github.com/mertenats/Open-Home-Automation).
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const PROGMEM char* WIFI_SSID = "";
const PROGMEM char* WIFI_PASSWORD = "";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "";
const PROGMEM char* MQTT_SERVER_IP = "";
const PROGMEM uint16_t MQTT_SERVER_PORT = ;
const PROGMEM char* MQTT_USER = "";
const PROGMEM char* MQTT_PASSWORD = "";

// MQTT: topic
const PROGMEM char* MQTT_LIGHT_STATUS_TOPIC = "sensor/light";

// default payload
const PROGMEM char* LIGHT_ON = "ON";
const PROGMEM char* LIGHT_OFF = "OFF";

uint8_t ldr_state = LOW; // no light detected
uint8_t ldr_value = 0;
float threshold = 1.0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void publishldrSensorState() {
  if (ldr_state) {
    client.publish(MQTT_LIGHT_STATUS_TOPIC, LIGHT_ON, true);
      Serial.println("LIGHT DETECTED");
  } else {
    client.publish(MQTT_LIGHT_STATUS_TOPIC, LIGHT_OFF, true);
      Serial.println("LIGHT NOT DETECTED");
  }
}

void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("INFO: connected");
    } else {
      Serial.print("ERROR: failed, rc=");
      Serial.print(client.state());
      Serial.println("DEBUG: try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // init the serial
  Serial.begin(115200);

  // init the WiFi connection
  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.println("INFO: IP address: ");
  Serial.println(WiFi.localIP());

  // init the MQTT connection
  client.setServer(MQTT_SERVER_IP, MQTT_SERVER_PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // read the LDR sensor
  
  float intensity = analogRead(A0)*(5.0/1023.0);
 
  if (intensity>threshold){
  ldr_state=HIGH;
  }
   publishldrSensorState();
   ldr_state=LOW;
}
