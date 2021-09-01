#include <DHT.h>
#include <WiFi.h>
 
const char* ssid     = "SKYX2YPS 2.4GHz"; // ESP32 and ESP8266 uses 2.4GHZ wifi only
const char* password = "chp6pcs5BWeF"; 
 
//Initialise MQTT 
#include <PubSubClient.h>
#define mqtt_server "192.168.0.29"
WiFiClient espClient;
PubSubClient client(espClient);
#define mqttTempExt "growShed/tempExt"
#define mqttHumExt "growShed/humExt"
#define mqttrealExt "growShed/realExt"
//Pin variables
#define DHTPIN 33
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
 
float tempExt, humExt,  realExt;
 
void setup() {
  Serial.begin(9600);
  Serial.println();
  
//Initialisation of Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 
  client.setServer(mqtt_server, 1883);
  
  dht.begin();
  delay(5000);
}
 
void getValues() {

  tempExt = dht.readTemperature();
  humExt = dht.readHumidity();
  realExt = dht.computeHeatIndex(tempExt, humExt, false);

 
  Serial.print("Ext Temp = ");
  Serial.print(tempExt);
  Serial.println(" *C");
 
  Serial.print("Ext Humidity = ");
  Serial.print(humExt);
  Serial.println(" %");

  Serial.print("Ext Real Temp = ");
  Serial.print(realExt);
  Serial.println(" *C");
 
  Serial.println();
 
}
 //Reconnection variables

void reconnect() {

  int counter = 0;
  while (!client.connected()) {
    if (counter==5){
      ESP.restart();
    }
    counter+=1;
    Serial.print("Attempting MQTT connection...");

   
    if (client.connect("growTentController")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
//If disconnected, try in five seconds
      delay(5000);
    }
  }
}
 
 
void loop() {
  if (!client.connected()){
    reconnect();
  }
  getValues();
 
  client.publish(mqttTempExt, String(tempExt).c_str(),true);
  client.publish(mqttHumExt, String(humExt).c_str(),true);
  client.publish(mqttrealExt, String(realExt).c_str(),true);
 //Data will be published every five seconds
  delay(5000);
}
