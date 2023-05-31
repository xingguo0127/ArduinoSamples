#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "xingguo";
const char* password = "12345678";


// MQTT Broker
const char *mqtt_broker = "47.101.132.35";
const char *mqtt_username = "devicename";
const char *mqtt_password = "whatever";
const int mqtt_port = 1883;

const char *pubTopic = "pillBox/test/pub";
const char *subTopic = "pillBox/test/sub";

WiFiClient espClient;
PubSubClient client(espClient);



void setup() {
  Serial.begin(115200);
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);

  while (!client.connected()) {
    String client_id = "esp8266-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    bool connectResult = client.connect(client_id.c_str(), mqtt_username, mqtt_password);
    if (connectResult) {
      Serial.println(" emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.subscribe(subTopic);
}

void loop() {
  client.loop();
}


void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char) payload[i];  // convert *byte to string
  }
  Serial.print(message);
  Serial.println();
  Serial.println("-----------------------");
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }
}

void publishMessage() {
  char dataChar[100];
  StaticJsonDocument<100> dataSet;
  dataSet["rfid"] = "123321";
  serializeJson(dataSet, dataChar);
  client.publish(pubTopic, dataChar);
}
