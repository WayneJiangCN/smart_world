#include <ESP8266WiFi.h>   //安装esp8266arduino开发环境
#include <PubSubClient.h>  //安装PubSubClient库
#include <ArduinoJson.h>   //json  V5版本
#include "aliyun_mqtt.h"
//需要安装crypto库
#include "DHT.h"
#include <U8g2lib.h>
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 14, /* data=*/ 2, /* reset=*/ U8X8_PIN_NONE);
unsigned char rx[8];
int lightux;
int gas_value;
unsigned char curtain;
unsigned char entranceGuard;
#define LED     D4
#define WIFI_SSID        "whut"//替换自己的WIFI
#define WIFI_PASSWD      "whut2021"//替换自己的WIFI

#define PRODUCT_KEY      "a1BcSa57h8U" //替换自己的PRODUCT_KEY
#define DEVICE_NAME      "ESP8266" //替换自己的DEVICE_NAME
#define DEVICE_SECRET    "c071740b03efedab40fdae49a7829378"//替换自己的DEVICE_SECRET

#define DEV_VERSION       "S-TH-WIFI-v1.0-20190220"        //固件版本信息

#define ALINK_BODY_FORMAT         "{\"id\":\"123\",\"version\":\"1.0\",\"method\":\"%s\",\"params\":%s}"
#define ALINK_TOPIC_PROP_POST     "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post"
#define ALINK_TOPIC_PROP_POSTRSP  "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/event/property/post_reply"
#define ALINK_TOPIC_PROP_SET      "/sys/" PRODUCT_KEY "/" DEVICE_NAME "/thing/service/property/set"
#define ALINK_METHOD_PROP_POST    "thing.event.property.post"
#define ALINK_TOPIC_DEV_INFO      "/ota/device/inform/" PRODUCT_KEY "/" DEVICE_NAME ""
#define ALINK_VERSION_FROMA      "{\"id\": 123,\"params\": {\"version\": \"%s\"}}"
unsigned long lastMs = 0;

WiFiClient   espClient;
PubSubClient mqttClient(espClient);


#define DHTPIN 5
#define DHTTYPE DHT11

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

void init_wifi(const char *ssid, const char *password)      //连接WiFi
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi does not connect, try again ...");
    delay(500);
  }

  Serial.println("Wifi is connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_callback(char *topic, byte *payload, unsigned int length) //mqtt回调函数，物联网平台发送指令给8266
{
 // Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  payload[length] = '\0';
  //Serial.println((char *)payload);
  // https://arduinojson.org/v5/assistant/  json数据解析网站


 // Serial.println("   ");
 // Serial.println((char *)payload);
  //Serial.println("   ");
  if (strstr(topic, ALINK_TOPIC_PROP_SET))
  {
    StaticJsonBuffer<100> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(payload);
    char sensor  = root["data"];
    //Serial.println(sensor);
    //Serial.println("{\"data\":0}");
    if (sensor == 0)
        Serial.println("A");
    if (sensor == 1)
      Serial.println("B");
    if (sensor == 2)
      Serial.println("C");
    if (sensor == 3)
      Serial.println("D");
  }
}

void mqtt_version_post()
{
  char param[512];
  char jsonBuf[1024];

  sprintf(param, "{\"MotionAlarmState\":%d}", digitalRead(13));
  sprintf(param, "{\"id\": 123,\"params\": {\"version\": \"%s\"}}", DEV_VERSION);
  sprintf(jsonBuf, ALINK_BODY_FORMAT, ALINK_METHOD_PROP_POST, param);
  Serial.println(param);
  mqttClient.publish(ALINK_TOPIC_DEV_INFO, param);
}
void mqtt_check_connect()
{
  while (!mqttClient.connected())//
  {
    while (connect_aliyun_mqtt(mqttClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET))
    {
      Serial.println("MQTT connect succeed!");
      //client.subscribe(ALINK_TOPIC_PROP_POSTRSP);
      mqttClient.subscribe(ALINK_TOPIC_PROP_SET);

      Serial.println("subscribe done");
      mqtt_version_post();
    }
  }

}

void mqtt_interval_post()
{
  int  Humidity = dht.readHumidity();
  int Temperature = dht.readTemperature();
  char param[512];
  char jsonBuf[1024];
  int Light;

  //sprintf(param, "{\"CurrentTemperature\":%d}", Humidity);

  sprintf(param, "{\"CurtainWorkState\":%d,\"CurrentTemperature\":%d,\"CurrentHumidity\":%d,\"LightLux\":%d,\"AQI\":%d,\"StatusLightSwitch\":%d}", curtain, Temperature, Humidity, lightux, gas_value, entranceGuard);
  sprintf(jsonBuf, ALINK_BODY_FORMAT, ALINK_METHOD_PROP_POST, param);
  // Serial.println(jsonBuf);
  mqttClient.publish(ALINK_TOPIC_PROP_POST, jsonBuf);
}


void setup()
{

  u8g2.begin();
  dht.begin();


  pinMode(LED, OUTPUT);
  /* initialize serial for debugging */
  Serial.begin(9600);

  Serial.println("Demo Start");

  init_wifi(WIFI_SSID, WIFI_PASSWD);

  mqttClient.setCallback(mqtt_callback);
}

// the loop function runs over and over again forever
void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

 if (Serial.available() > 0)
  {
     
    Serial.readBytes(rx, 8);

    if (rx[0] == 'a')
    {
      if (rx[1] == '1') //窗帘打开
        curtain = 1;
      else
        curtain = 0;
      if (rx[1] == '1')   //门禁打开
        entranceGuard = 1;
      else
        entranceGuard = 0;
      lightux = (rx[3] - 48) * 10 + (rx[4] - 48);
      gas_value = (rx[5] - 48) * 10 + (rx[6] - 48);
    }
  } 

  //delay(100);
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_inb16_mf); // choose a suitable font
  // Display data
  u8g2.setCursor(15, 20); //OLED   128  X  64
  u8g2.print("T:");
  u8g2.print(t);
  u8g2.setCursor(15, 39);
  u8g2.print("H:");
  u8g2.print(h);
  u8g2.setCursor(15, 64);
  u8g2.setFont(u8g2_font_inb16_mf);
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.print("L:");
  u8g2.print(lightux);
  //u8g2.print(WiFi.localIP());
  u8g2.sendBuffer();
 

//      
//        u8g2.setCursor(20, 63);
//        u8g2.setFont(u8g2_font_ncenB08_tr);
//        u8g2.print("suceess");
//        u8g2.sendBuffer();
//   
      
    
  
  if (millis() - lastMs >= 5000)  //5S，检查连接和发送
  {
    lastMs = millis();
    mqtt_check_connect();
    /* Post */
    mqtt_interval_post();
  }

  mqttClient.loop();


}
