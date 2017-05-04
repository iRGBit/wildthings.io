//             _ _     _ _   _     _                   _       
//            (_) |   | | | | |   (_)                 (_)      
//   __      ___| | __| | |_| |__  _ _ __   __ _ ___   _  ___  
//   \ \ /\ / / | |/ _` | __| '_ \| | '_ \ / _` / __| | |/ _ \ 
//    \ V  V /| | | (_| | |_| | | | | | | | (_| \__ \_| | (_) |
//     \_/\_/ |_|_|\__,_|\__|_| |_|_|_| |_|\__, |___(_)_|\___/ 
                                        __/ |              
                                       |___/               

//   Moturoa Transmissions 
//   MQTT Client for Wemos D1
//   wildthings.io - Birgit Bachler, Aotearoa/New Zealand, 2017
//
//   Wemos D1 with RGB shield recoeding network traffic of Moturoa Stream WiFi, Te Upoko a te Ika a Maui/Wellington

//   credits to 
//   ESP8266learning http://www.esp8266learning.com/logging-dht11-data-sd-card.php
//   https://www.baldengineer.com/mqtt-tutorial.html
//   with adaptions as in the comments by Dag Rende & William Brinkman



#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <SPI.h>
#include <SD.h>

const int chipSelect = D8;
File dataFile;

#define FILENAME "Moturoa_Transmissions.csv"

#define node_name "ESP8266/networkSD"
#define all "#"

#define hTopic "moturoa/ahumid" // DHT11 humidity
#define tTopic "moturoa/atemp" // DHT11 temp
#define humidTopic "moturoa/humid" // soil
#define rainTopic "moturoa/rain" // rain
#define ecTopic "moturoa/ec" // conduct
#define spikeTopic "moturoa/spike" // spike in water
#define wTopic "moturoa/watertemp"


// Connect to the WiFi
const char* ssid = "Moturoa_Transmissions";
const char* password = "Tangaroa";
const char* mqtt_server = "192.168.42.1";

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  dataFile = SD.open(FILENAME, FILE_WRITE);
  if (dataFile) {
    Serial.print("opened ");
    Serial.print(FILENAME);
    Serial.println("...");
    Serial.print(topic);
    Serial.print(" ");
    String myMessage = "";
    for (int i = 0; i < length; i++) {
      char receivedChar = (char)payload[i];
      myMessage+=receivedChar;
    }
    Serial.println(myMessage);
    dataFile.print(topic);
    dataFile.print(",");
    dataFile.print(myMessage);
    dataFile.print(",");
    dataFile.println(millis());
    // close the file:
    dataFile.close();
    // print to the serial port too:
    Serial.print("closed ");
    Serial.print(FILENAME);
    Serial.println("...");
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening MoturoaTransmissions.csv");
  }

  // if (strcmp(topic, spikeTopic) == 0)
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    if (client.connect(node_name)) {
      Serial.println("connected");
      // ... and subscribe to topic
      client.subscribe(all);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

    }
  }
}

void setup()
{
  Serial.begin(9600);

  setup_wifi();             // added this line of code !!!!!!!

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  delay(2000);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();

  }
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}

void setup_wifi() {
  // added this function - without it the mqtt part will not connect
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
