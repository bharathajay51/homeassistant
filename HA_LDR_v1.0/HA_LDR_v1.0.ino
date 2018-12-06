/*
   MQTT Binary Sensor - Motion (PIR) for Home-Assistant - NodeMCU (ESP8266)
   https://home-assistant.io/components/binary_sensor.mqtt/

   Libraries :
    - ESP8266 core for Arduino : https://github.com/esp8266/Arduino
    - PubSubClient : https://github.com/knolleary/pubsubclient

   Sources :
    - File > Examples > ES8266WiFi > WiFiClient
    - File > Examples > PubSubClient > mqtt_auth
    - File > Examples > PubSubClient > mqtt_esp8266
    - https://learn.adafruit.com/pir-passive-infrared-proximity-motion-sensor/using-a-pir

   Schematic :
    - https://github.com/mertenats/open-home-automation/blob/master/ha_mqtt_binary_sensor_pir/Schematic.png
    - PIR leg 1 - VCC
    - PIR leg 2 - D1/GPIO5
    - PIR leg 3 - GND
    
   Configuration (HA) : 
     binary_sensor:
      platform: mqtt
      state_topic: 'office/motion/status'
      name: 'Motion'
      sensor_class: motion
      
   TODO :
    - Use the interrupts instead of constinously polling the sensor

   Samuel M. - v1.1 - 08.2016
   If you like this example, please add a star! Thank you!
   https://github.com/mertenats/open-home-automation
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_VERSION MQTT_VERSION_3_1_1

// Wifi: SSID and password
const PROGMEM char* WIFI_SSID = "ssid";
const PROGMEM char* WIFI_PASSWORD = "vasu&bharath";

// MQTT: ID, server IP, port, username and password
const PROGMEM char* MQTT_CLIENT_ID = "Light";
const PROGMEM char* MQTT_SERVER_IP = "m12.cloudmqtt.com";
const PROGMEM uint16_t MQTT_SERVER_PORT = 17978;
const PROGMEM char* MQTT_USER = "SDO";
const PROGMEM char* MQTT_PASSWORD = "123456789";

// MQTT: topic
const PROGMEM char* MQTT_MOTION_STATUS_TOPIC = "sensor/light";

// default payload
const PROGMEM char* MOTION_ON = "ON";
const PROGMEM char* MOTION_OFF = "OFF";

// PIR : D1/GPIO5
uint8_t m_pir_state = LOW; // no motion detected
uint8_t m_pir_value = 0;
float threshold = 1.0;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// function called to publish the state of the pir sensor
void publishPirSensorState() {
  if (m_pir_state) {
    client.publish(MQTT_MOTION_STATUS_TOPIC, MOTION_ON, true);
      Serial.println("LIGHT DETECTED");
  } else {
    client.publish(MQTT_MOTION_STATUS_TOPIC, MOTION_OFF, true);
      Serial.println("LIGHT NOT DETECTED");
  }
}

// function called when a MQTT message arrived
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

  // read the PIR sensor
  
  float intensity = analogRead(A0)*(5.0/1023.0);
 
  if (intensity>threshold){
  m_pir_state=HIGH;
  }
   publishPirSensorState();
    m_pir_state=LOW;
}
