#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>


#define MQTT_VERSION MQTT_VERSION_3_1_1


// Wifi: SSID and password

const char* WIFI_SSID = "ssid";
const char* WIFI_PASSWORD = "vasu&bharath";


// MQTT: ID, server IP, port, username and password

const PROGMEM char* MQTT_CLIENT_ID = "office_switches_sensors";
const PROGMEM char* MQTT_SERVER_IP = "192.168.0.6";//m12.cloudmqtt.com
const PROGMEM uint16_t MQTT_SERVER_PORT = 1883;//17978
const PROGMEM char* MQTT_USER = "";//waterlevelS
const PROGMEM char* MQTT_PASSWORD = "";//123456789

// MQTT: topics

// light sensor topic
const PROGMEM char* MQTT_LIGHT_SENSOR_STATUS_TOPIC = "sensor/light";

// fan state

const PROGMEM char* MQTT_FAN_STATE_TOPIC = "office/fan";
const PROGMEM char* MQTT_FAN_COMMAND_TOPIC = "office/fan/switch";

// fan speed

const PROGMEM char* MQTT_FAN_SPEED_TOPIC = "office/fan/speed";
const PROGMEM char* MQTT_FAN_SPEED_COMMAND_TOPIC = "office/fan/speed/set";

//lights
const char* MQTT_LIGHT1_STATE_TOPIC = "ha/light1/status";
const char* MQTT_LIGHT1_COMMAND_TOPIC = "ha/light1/switch";

const char* MQTT_LIGHT2_STATE_TOPIC = "ha/light2/status";
const char* MQTT_LIGHT2_COMMAND_TOPIC = "ha/light2/switch";

// RGB lights
// state
const PROGMEM char* MQTT_LIGHT_STATE_TOPIC = "office/rgb1/light/status";
const PROGMEM char* MQTT_LIGHT_COMMAND_TOPIC = "office/rgb1/light/switch";

// brightness
const PROGMEM char* MQTT_LIGHT_BRIGHTNESS_STATE_TOPIC = "office/rgb1/brightness/status";
const PROGMEM char* MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC = "office/rgb1/brightness/set";

// colors (rgb)
const PROGMEM char* MQTT_LIGHT_RGB_STATE_TOPIC = "office/rgb1/rgb/status";
const PROGMEM char* MQTT_LIGHT_RGB_COMMAND_TOPIC = "office/rgb1/rgb/set";

// fan payloads

const PROGMEM char* FAN_ON = "true";
const PROGMEM char* FAN_OFF = "false";
const PROGMEM char* FAN_LOW = "low";
const PROGMEM char* FAN_MED = "med";
const PROGMEM char* FAN_HIGH = "high";

// light payloads
const char* LIGHT_ON = "ON";
const char* LIGHT_OFF = "OFF";

// light variables

boolean m_light1_state = false;
boolean m_light2_state = false;

// rgb variables
boolean m_rgb_state = false;
uint8_t m_rgb_brightness = 100;
uint8_t m_rgb_red = 255;
uint8_t m_rgb_green = 255;
uint8_t m_rgb_blue = 255;


// fan varibles
boolean m_fan_state = false;

String payload;

uint8_t m_light_brightness = 255;

// light sensor varibles
uint8_t m_light_sensor_state = LOW; // no motion detected
float threshold = 1.0;

// output pins
// fan pins
//  const PROGMEM uint8_t FAN_PIN = 5;
const PROGMEM uint8_t low_pin = 4;
const PROGMEM uint8_t medium_pin = 0;
const PROGMEM uint8_t high_pin = 2;
Servo myservo;
// light pins
const PROGMEM uint8_t LED1_PIN = 16;
const PROGMEM uint8_t LED2_PIN = 15;
//rgb pins
const PROGMEM uint8_t RGB_LIGHT_RED_PIN = 14;//5
const PROGMEM uint8_t RGB_LIGHT_GREEN_PIN = 12;//0
const PROGMEM uint8_t RGB_LIGHT_BLUE_PIN = 13;//4


// buffer used to send/receive data with MQTT
const uint8_t MSG_BUFFER_SIZE = 20;
char m_msg_buffer[MSG_BUFFER_SIZE];


WiFiClient wifiClient;

PubSubClient client(wifiClient);

// function called to publish the state of the pir sensor
void publishLightSensorState() {
  if (m_light_sensor_state) {
    client.publish(MQTT_LIGHT_SENSOR_STATUS_TOPIC, LIGHT_ON, true);
  } else {
    client.publish(MQTT_LIGHT_SENSOR_STATUS_TOPIC, LIGHT_OFF, true);
  }
}

// function called to adapt the brightness and the color of the led
void setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue) {
  // convert the brightness in % (0-100%) into a digital value (0-255)

  uint8_t brightness = map(m_rgb_brightness, 0, 100, 0, 255);

  analogWrite(RGB_LIGHT_RED_PIN, map(p_red, 0, 255, 0, brightness));
  analogWrite(RGB_LIGHT_GREEN_PIN, map(p_green, 0, 255, 0, brightness));
  analogWrite(RGB_LIGHT_BLUE_PIN, map(p_blue, 0, 255, 0, brightness));
}

// Function to updte fan state
void setFanState() {
  if (m_fan_state) {
    if (payload.equals(String(FAN_LOW))) {
      myservo.write(40);
      Serial.println("FAN SPEED: LOW ");
    }
    if (payload.equals(String(FAN_MED))) {
      myservo.write(100);
      Serial.println("FAN SPEED: MEDIUM");
    }
    if (payload.equals(String(FAN_HIGH))) {
      myservo.write(180);
      Serial.println("FAN SPEED: HIGH");
    }
    if (payload.equals(String(FAN_ON))) {
      digitalWrite(low_pin,0);
      Serial.println("FAN SPEED: HIGH");
    }
    
  } else {
    digitalWrite(low_pin,1);
    Serial.println("INFO: Turn fan off");
  }
}

// function called to publish the state of the led (on/off)
void publishRGBState() {
  if (m_rgb_state) {
    client.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_ON, true);
  } else {
    client.publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_OFF, true);
  }
}

// function called to publish the brightness of the led (0-100)
void publishRGBBrightness() {
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d", m_rgb_brightness);
  client.publish(MQTT_LIGHT_BRIGHTNESS_STATE_TOPIC, m_msg_buffer, true);
}

// function called to publish the colors of the led (xx(x),xx(x),xx(x))
void publishRGBColor() {
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d,%d,%d", m_rgb_red, m_rgb_green, m_rgb_blue);
  client.publish(MQTT_LIGHT_RGB_STATE_TOPIC, m_msg_buffer, true);
}

// function called to publish the state of the light (on/off)
void publishLight1State() {
  if (m_light1_state) {
    client.publish(MQTT_LIGHT1_STATE_TOPIC, LIGHT_ON, true);
  } else {
    client.publish(MQTT_LIGHT1_STATE_TOPIC, LIGHT_OFF, true);
  }
}
void publishLight2State() {
  if (m_light2_state) {
    client.publish(MQTT_LIGHT2_STATE_TOPIC, LIGHT_ON, true);
  } else {
    client.publish(MQTT_LIGHT2_STATE_TOPIC, LIGHT_OFF, true);
  }
}

// function called to turn on/off the light
void setLight2State() {
  if (m_light2_state) {
    digitalWrite(LED2_PIN, LOW);
    Serial.println("INFO: Turn light2 on...");
  } else {
    digitalWrite(LED2_PIN, HIGH);
    Serial.println("INFO: Turn light2 off...");
  }
}
void setLight1State() {
  if (m_light1_state) {
    digitalWrite(LED1_PIN, LOW);
    Serial.println("INFO: Turn light1 on...");
  } else {
    digitalWrite(LED1_PIN, HIGH);
    Serial.println("INFO: Turn light1 off...");
  }
}

// function called to publish fan state
void publishFanState() {
  if (m_fan_state) {
    client.publish(MQTT_FAN_STATE_TOPIC, FAN_ON, true);
  } else {
    client.publish(MQTT_FAN_STATE_TOPIC, FAN_OFF, true);
  }
}

// function called to publish the speed of the fan
void publishFanSpeed() {
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d", m_light_brightness);
  if (payload.equals(String(FAN_LOW)))
    client.publish(MQTT_FAN_SPEED_TOPIC, FAN_LOW, true);
  if (payload.equals(String(FAN_MED)))
    client.publish(MQTT_FAN_SPEED_TOPIC, FAN_MED, true);
  if (payload.equals(String(FAN_HIGH)))
    client.publish(MQTT_FAN_SPEED_TOPIC, FAN_HIGH, true);
}

// function called when a MQTT message arrived
void callback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string

  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  // handle message topic
  Serial.println(payload);
  // FANS
  if (String(MQTT_FAN_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(FAN_ON))) {
      if (m_fan_state != true) {
        m_fan_state = true;
        setFanState();
        publishFanState();
        payload = "";  // Refresh payload
      }
    }
    else {
      if (m_fan_state != false) {
        m_fan_state = false;
        setFanState();
        publishFanState();
        payload = "";
      }
    }
  } else if (String(MQTT_FAN_SPEED_COMMAND_TOPIC).equals(p_topic)) {
    setFanState();
    publishFanSpeed();
    payload = "";
  }

  // LIGHTS
  if (String(MQTT_LIGHT2_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(LIGHT_ON))) {
      if (m_light2_state != true) {
        m_light2_state = true;
        setLight2State();
        publishLight2State();
        payload = "";
      }
    } else if (payload.equals(String(LIGHT_OFF))) {
      if (m_light2_state != false) {
        m_light2_state = false;
        setLight2State();
        publishLight2State();
        payload = "";
      }
    }
  }
  if (String(MQTT_LIGHT1_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(LIGHT_ON))) {
      if (m_light1_state != true) {
        m_light1_state = true;
        setLight1State();
        publishLight1State();
        payload = "";
      }
    } else if (payload.equals(String(LIGHT_OFF))) {
      if (m_light1_state != false) {
        m_light1_state = false;
        setLight1State();
        publishLight1State();
        payload = "";
      }
    }
  }

  // RGB LIGHTS
  if (String(MQTT_LIGHT_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    Serial.println(payload);
    if (payload.equals(String(LIGHT_ON))) {
      if (m_rgb_state != true) {
        m_rgb_state = true;
        setColor(m_rgb_red, m_rgb_green, m_rgb_blue);
        publishRGBState();
        payload = "";
      }
    } else if (payload.equals(String(LIGHT_OFF))) {
      Serial.println(payload);
      if (m_rgb_state != false) {
        m_rgb_state = false;
        setColor(0, 0, 0);
        publishRGBState();
        payload = "";
      }
    }
    payload = "";
  } else if (String(MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC).equals(p_topic)) {
    Serial.println(payload);
    uint8_t brightness = payload.toInt();
    if (brightness < 0 || brightness > 100) {
      // do nothing...
      return;
      payload = "";
    } else {
      m_rgb_brightness = brightness;
      setColor(m_rgb_red, m_rgb_green, m_rgb_blue);
      publishRGBBrightness();
      payload = "";
    }
    payload = "";
  } else if (String(MQTT_LIGHT_RGB_COMMAND_TOPIC).equals(p_topic)) {
    // get the position of the first and second commas
    Serial.println(payload);
    uint8_t firstIndex = payload.indexOf(',');
    uint8_t lastIndex = payload.lastIndexOf(',');

    uint8_t rgb_red = payload.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) {
      return;
    } else {
      m_rgb_red = rgb_red;
    }

    uint8_t rgb_green = payload.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) {
      return;
    } else {
      m_rgb_green = rgb_green;
    }

    uint8_t rgb_blue = payload.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) {
      return;
    } else {
      m_rgb_blue = rgb_blue;
    }

    setColor(m_rgb_red, m_rgb_green, m_rgb_blue);
    publishRGBColor();
    payload = "";
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("INFO: Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("\nINFO: connected");
      // Once connected, publish an announcement...

      // publish the initial values

      publishFanState();
      publishFanSpeed();
      publishLight1State();
      publishLight2State();
      publishRGBState();
      publishRGBBrightness();
      publishRGBColor();

      // ... and resubscribe
      client.subscribe(MQTT_FAN_COMMAND_TOPIC);
      client.subscribe(MQTT_FAN_SPEED_COMMAND_TOPIC);
      client.subscribe(MQTT_LIGHT2_COMMAND_TOPIC);
      client.subscribe(MQTT_LIGHT1_COMMAND_TOPIC);
      client.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
      client.subscribe(MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC);
      client.subscribe(MQTT_LIGHT_RGB_COMMAND_TOPIC);

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
  // init the led
  //fan pins
  myservo.attach(5);
  // pinMode(FAN_PIN,OUTPUT);
  pinMode(low_pin, OUTPUT);
  pinMode(medium_pin, OUTPUT);
  pinMode(high_pin, OUTPUT);
  // light pins
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  // RGB light pins
  pinMode(RGB_LIGHT_BLUE_PIN, OUTPUT);
  pinMode(RGB_LIGHT_RED_PIN, OUTPUT);
  pinMode(RGB_LIGHT_GREEN_PIN, OUTPUT);

  analogWriteRange(255);
  setColor(0, 0, 0);

  setFanState();
  setLight1State();
  setLight2State();

  // init the WiFi connection

  Serial.println();
  Serial.println();
  Serial.print("INFO: Connecting to ");

  WiFi.mode(WIFI_STA);

  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);


  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");

  }

  Serial.println("");
  Serial.println("INFO: WiFi connected");
  Serial.print("INFO: IP address: ");
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

  float intensity = analogRead(A0) * (5.0 / 1023.0);

  if (intensity > threshold) {
    m_light_sensor_state = HIGH;
  }
  publishLightSensorState();
  m_light_sensor_state = LOW;

  payload = "";
}

