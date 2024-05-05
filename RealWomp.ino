#include <Wire.h>
#include "DHT.h"
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <Wire.h>
#define WIFI_SSID "Jones"
#define WIFI_PASSWORD "Pokemon@1234"

#define INFLUXDB_URL "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "ZjD6C8Ub-6NWbFPBlY6thrpb5tRqjtenc2HX4QlNvEpi7S2s2HpjnpCPpSmPgA-9l0ekOXTCuSXTHfGcVY5WDQ=="
#define INFLUXDB_ORG "jmays@usc.edu"
#define INFLUXDB_BUCKET "WeatherData"
#define TZ_INFO "CET"

//The other sensors//
#define RELAY_PIN 26
#define BIG_SOUND_PIN 25
#define LIGHT_SENSOR 32
#define TILT_SWITCH 27

WiFiClient espClient;

DHT dht(16,DHT11);

int temp = 0;
int humid = 0;
int val = 0;
int soundValue = 0;
int light = 0;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point sensor("weather");

void setup() {

  Serial.begin(9600);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BIG_SOUND_PIN, INPUT);
  pinMode(LIGHT_SENSOR, INPUT);
  pinMode(TILT_SWITCH, INPUT);

  Serial.println("Initializing...");

  dht.begin();

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("WiFi connected!");

  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WIFI_SSID);

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

}

void loop() {

  temp = dht.readTemperature();
  humid = dht.readHumidity();
  soundValue = digitalRead(BIG_SOUND_PIN);
  val = digitalRead(TILT_SWITCH);
  light = analogRead(LIGHT_SENSOR);

  sensor.clearFields();
  sensor.addField("temperature", temp);
  sensor.addField("humidity", humid);
  sensor.addField("sound", soundValue);
  sensor.addField("light", light);

  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi connection lost, attempting to reconnect...");
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  }

  if (!client.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
  }
  
  Serial.print("Temp: ");
  Serial.println(temp);
  Serial.print("Humidity: ");
  Serial.println(humid);
  Serial.print("Sound Level: ");
  Serial.println(soundValue);
  Serial.print("Light Level: ");
  Serial.println(light);

  Serial.println("Waiting 5 seconds");

  if (Serial.available() > 0) {

    String line = Serial.readStringUntil('\n');

    Serial.println("Line replace: " + line);

    if (line == "wakeUp"){
      if (val == 1){
          digitalWrite(RELAY_PIN, LOW);
        }
        else{
          digitalWrite(RELAY_PIN, HIGH);  
        }
    }

    if (line == "shutDown"){
      if (val == 1){
          digitalWrite(RELAY_PIN, HIGH);
        }
        else{
          digitalWrite(RELAY_PIN, LOW);  
        }
    }
  }

  delay(500);

}
