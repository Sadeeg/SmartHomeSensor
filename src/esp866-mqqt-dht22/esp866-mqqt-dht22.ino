// MQTT_KEEPALIVE : keepAlive interval in Seconds

#define MQTT_KEEPALIVE 5
#define MQTT_SOCKET_TIMEOUT 5

#define DEEPSLEEP_TIME_IN_MIN 5
#define FORCE_DEEPSLEEP

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

/**
  Wifi & MQTT Settings
*/
const char* ssid = "YOUR_SSDID";
const char* password = "YOUR_PASSWORD";
const char* mqtt_server = "YOUR_MQTT_SERVER";
WiFiClient espClient;
PubSubClient client(espClient);

#define DHTTYPE   DHT22
#define DHTPIN    5
DHT dht(DHTPIN, DHTTYPE, 22);

char msg[50];
float temperature = 0;
float humidity = 0;


void setup() {
  Serial.begin(115200);
  setup_wifi();
  dht.begin();
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP-Sensor1";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


/**
  Connect to wifi
*/
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  if (!client.connected()) {
    reconnect();
  }

  dtostrf(temperature, 6, 2, msg);
  client.publish("home/sensor1/temperature", msg);

  dtostrf(humidity, 6, 2, msg);
  client.publish("home/sensor1/humidity", msg);

  //Safe Disconnect
  client.disconnect();
  
  //Sleep for X Minutes, then wakeup and send data again
  #ifdef FORCE_DEEPSLEEP  
  ESP.deepSleep(DEEPSLEEP_TIME_IN_MIN * 60 * 1000000);
  delay(100);
  #endif
  delay(100);
}
