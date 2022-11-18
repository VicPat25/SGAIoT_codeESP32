#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// Global Vars
WiFiClient wClient;
DynamicJsonDocument doc(2048);
PubSubClient mqttClient(wClient);

// Functions definitions
bool get_mqtt_credentials();

// Network connection
const char *wifi_ssid = "LIDA";         // Change parameters
const char *wifi_password = "25081157"; // Change parameters

// Device data
String dId = "4321";                // Change parameters
String webhook_pass = "8Khg0629TQ"; // Change parameters
String webhook_endpoint = "http://192.168.1.87:3001/api/getdevicecredentials";
const char *mqtt_server = "192.168.1.87"; // Change parameters

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (get_mqtt_credentials())
  {
    mqttClient.setServer(mqtt_server, 1883);
    String analogSend = "{\"value\":0}";
    String str_clientid = "device" + dId + "_" + random(1, 9999);
    const char *username = doc["username"];
    const char *password = doc["password"];
    String str_topic = doc["topic"];
    while (1)
    {
      if (mqttClient.connect(str_clientid.c_str(), username, password))
      {
        String variable = doc["variables"][0]["variable"];
        String topic = str_topic + variable + "/sdata";
        String value = String(random(14, 32));
        analogSend = "{\"value\":" + value + ", \"save\":1}";
        mqttClient.publish(topic.c_str(), analogSend.c_str());
        delay(3000);
      }
    }
  }
}

// Functions

bool get_mqtt_credentials()
{
  String toSend = "dId=" + dId + "&password=" + webhook_pass;

  HTTPClient http;
  http.begin(webhook_endpoint);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int response_code = http.POST(toSend);

  if (response_code < 0)
  {
    http.end();
    return false;
  }

  if (response_code != 200)
  {
    http.end();
    return false;
  }

  if (response_code == 200)
  {
    String responseBody = http.getString();
    deserializeJson(doc, responseBody);
    http.end();
    delay(1000);
  }

  return true;
}