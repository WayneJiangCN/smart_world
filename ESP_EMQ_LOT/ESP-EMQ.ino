#include "DHTesp.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

DHTesp dht;
unsigned char rx[8];
char msg[100]; //存放json数据
float humidity;
float temperature;
int lightux;
int gas_value;
unsigned int curtain;
unsigned int entranceGuard;

const char* ssid = "esp8266_123456";    //你的wifi名
const char* password = "66666678";   //你的wifi密码
const char* mqtt_server = "47.100.247.43";   //服务器地址

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;


void setup()
{
  pinMode(D2, INPUT); //将光感DO口接到D2
  Serial.begin(9600);
  Serial.println();
  //  Serial.println("Status\tHumidity (%)\tTemperature (C)\t(F)\tHeatIndex (C)\t(F)");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  dht.setup(5, DHTesp::DHT11); // Connect DHT sensor to GPIO D1
}
int shine()
{
  int i = analogRead(A0); //光感模拟口接esp8266AO口
  i = 1023 - i;
  i = i / 10.23 - 1; //将光照强度设置为0-99
  return i;
}
void loop()
{
  if (Serial.available() > 0)
  {
    Serial.readBytes(rx, 8);
    if (rx[0] == 'a')
    {
      if (rx[1] == '1') //窗帘打开
        curtain = 1;
      else
        curtain = 0;
      if (rx[2] == '1')   //门禁打开
        entranceGuard = 1;
      else
        entranceGuard = 0;
      lightux = (rx[3] - 48) * 10 + (rx[4] - 48);
      gas_value = (rx[5] - 48) * 10 + (rx[6] - 48);
    }
  }

  delay(dht.getMinimumSamplingPeriod());

  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  encodeJson();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    //    snprintf (msg, 75, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("DHT11", msg);
  }

}

//JSON编码函数
void encodeJson() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root1 = jsonBuffer.createObject();
  root1["Humidity"] = humidity;
  root1["Temperature"] = temperature;
  root1["Lightux"] = lightux;
   root1["Gas_Value"] = gas_value;
   root1["Curtain"] = curtain;
      root1["EntranceGuard"] = entranceGuard;
  //  root1.prettyPrintTo(Serial);
  root1.printTo(msg);
}

//JSON解码函数
void decodeJson(char msg[100]) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(msg);
  float temp = root["Temperature"];
  float hum = root["Humidity"];
  float light = root["Lightux"];
  int gas =root["Gas_Value"];
  unsigned char curtain_status  = root["Curtain"];
    unsigned char entranceGuard_status  = root["EntranceGuard"];


  Serial.println(temp);
  Serial.println(hum);
  Serial.println(light);
   Serial.println(gas);
    Serial.println(curtain_status);
    Serial.println(entranceGuard_status);

}

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
    //    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      //      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
