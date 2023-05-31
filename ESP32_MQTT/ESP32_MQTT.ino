
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
StaticJsonDocument<200> jsonBuffer;

int LED = 2;
const char* ssid = "xingguo";
const char* password = "12345678";
const char* mqttServer = "47.101.132.35";//本机ip地址


WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);


void pubmsg( const String &topicString, const String &messageString) {
  char publishTopic[topicString.length() + 1];
  strcpy(publishTopic, topicString.c_str());
  char publishMsg[messageString.length() + 1];
  strcpy(publishMsg, messageString.c_str());

  // 实现ESP8266向主题发布信息
  if (mqttClient.publish(publishTopic, publishMsg)) {
    Serial.println("Publish Topic:"); Serial.println(publishTopic);
    Serial.println("Publish message:"); Serial.println(publishMsg);
  } else {
    Serial.println("Message Publish Failed.");
  }
}

void setup() {
  pinMode(LED, OUTPUT);     // 设置板上LED引脚为输出模式
  digitalWrite(LED, LOW);  // 启动后关闭板上LED
  Serial.begin(9600);               // 启动串口通讯

  connectWifi();
  // 设置MQTT服务器和端口号
  mqttClient.setServer(mqttServer, 1883);
  mqttClient.setCallback(receiveCallback);

  // 连接MQTT服务器
  connectMQTTserver();
}

void loop() {
  if (mqttClient.connected()) { // 如果开发板成功连接服务器
    mqttClient.loop();          // 处理信息以及心跳
  } else {                      // 如果开发板未能成功连接服务器
    connectMQTTserver();        // 则尝试连接服务器
  }
}

void connectMQTTserver() {
  String clientId = "esp8266-" + WiFi.macAddress();

  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("MQTT Server Connected.");
    Serial.println("Server Address:");
    Serial.println(mqttServer);
    Serial.println("ClientId: ");
    Serial.println(clientId);
    subscribeTopic(); // 订阅指定主题
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(5000);
  }
}
// 收到信息后的回调函数
void receiveCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message Received [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
  Serial.print("Message Length(Bytes) ");
  Serial.println(length);
  int temp = 0;
  while (*topic) {
    temp++;
    topic++;
  }
  Serial.println("主题长度:");
  Serial.println(temp);

  DeserializationError error = deserializeJson(jsonBuffer, payload);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  const char* str = jsonBuffer["msg"];           // 读取字符串
  Serial.println(str);
}

// 订阅指定主题
void subscribeTopic() {
  // 通过串口监视器输出是否成功订阅主题1以及订阅的主题1名称
  if (mqttClient.subscribe("device/dev/#")) {
    Serial.println("Subscrib Topic:");
    Serial.println("device/action");
  } else {
    Serial.print("Subscribe Fail...");
  }
}

void connectWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected!");
  Serial.println("");
}
