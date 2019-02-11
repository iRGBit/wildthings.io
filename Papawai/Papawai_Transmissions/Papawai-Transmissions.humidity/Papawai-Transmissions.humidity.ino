//             _ _     _ _   _     _                   _
//            (_) |   | | | | |   (_)                 (_)
//   __      ___| | __| | |_| |__  _ _ __   __ _ ___   _  ___
//   \ \ /\ / / | |/ _` | __| '_ \| | '_ \ / _` / __| | |/ _ \ 
//    \ V  V /| | | (_| | |_| | | | | | | | (_| \__ \_| | (_) |
//     \_/\_/ |_|_|\__,_|\__|_| |_|_|_| |_|\__, |___(_)_|\___/
//                                        __/ |
//                                       |___/

//   Papawai Transmissions
//   MQTT Client for Wemos D1
//   wildthings.io - Birgit Bachler, Aotearoa/New Zealand, 2018
//
//   Wemos D1 with a humidity and temperature sensor DHT22 monitoring the environment of Papawai Stream, Te Upoko a te Ika a Maui/Wellington
//
//    credits to
//   James Lewis : https://www.baldengineer.com/mqtt-tutorial.html with adaptions as in the comments by Dag Rende & William Brinkman
//   ItKindaWorks : http://github.com/ItKindaWorks - https://github.com/ItKindaWorks/ESP8266/blob/master/Home%20Automation/Part%201/ESP8266_SimpleMQTT/ESP8266_SimpleMQTT.ino
//   Paulus Schoutsen : https://home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/
//
//    Requires PubSubClient found here: https://github.com/knolleary/pubsubclient
//



//DHT
#include "DHT.h"
#define DHTPIN D4     // what pin we're connected to
#define DHTTYPE DHT11  // DHT 11 / DHT 22  (AM2302), AM2321 / DHT 21 (AM2301)


// MQTT
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define node_name "Papawai_takawai"
#define hTopic "papawai/takawaiea" // air humidity | takawai-humidity ea-air
#define tTopic "papawai/paemahanaea" // air temperature | paemahanea-temperature ea-air
#define hpubTopic "papawai/glowworm1/takawaiea" // air humidity | takawai-humidity ea-air
#define tpubTopic "papawai/glowworm1/paemahanaea" // air temperature | paemahanea-temperature ea-air

// DEBUG
boolean debug = true; // set to true to read data via Serial

unsigned long boardTime = 0 ;
boolean which = false;

const char* ssid = "Moturoa_Transmissions";
const char* password = "Tangaroa";
const char* mqtt_server = "192.168.42.1";

static int wait = 60000;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);


void callback(char* topic, byte * payload, unsigned int length) {
  //convert topic to string to make it easier to work with

  //Print out some debugging info
  if (debug) {
    Serial.println("Callback update.");

    Serial.print("Topic: ");
    Serial.println(topic);
  }

}

void reconnect() {
  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    long waitTime = millis();
    if (debug) {
      //debug printing
      Serial.print("Connecting to ");
      Serial.println(ssid);
    }

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if (debug) {
        Serial.print(".");
      }
    }


  }

  //make sure we are connected to WIFI before attemping to reconnect to MQTT
  if (WiFi.status() == WL_CONNECTED) {
    // Loop until we're reconnected to the MQTT server
    while (!client.connected()) {
      if (debug) {
        Serial.print("Attempting MQTT connection...");
      }
      // Attempt to connect
      if (client.connect(node_name)) {
        client.disconnect();
        client.connect(node_name);
        if (debug) {
          Serial.println("connected");
        }
        // ... and subscribe to topic
        //       client.subscribe(tempTopic);


      } else {
        if (debug) {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 5 seconds");
        }
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
}


void setup() {


  //start the serial line for debugging
  if (debug) {
    Serial.begin(9600);
  }
  delay(100);


  //start wifi subsystem
  WiFi.begin(ssid, password);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();

  //wait a bit before starting the main loop
  delay(2000);

  //dht
  dht.begin();

}


void loop() {
  if (!client.connected() && WiFi.status() == 3) {
    reconnect();
  }

  client.loop();
  boardTime = millis();
  if (debug){
    wait=1000;
    }
  if ((boardTime % wait) == 0) {
    if (which == false) { // do humidity
      float h = dht.readHumidity();
      char result[20] = "";
      dtostrf(h, 3, 1, result);
      delay(10);
      if (debug) {
        Serial.println(result);
      }
      client.publish(hTopic, result);
      //  client.publish(hpubTopic, result);
      which = true;
    }
    else { //do humid
      float t = dht.readTemperature();
      char tresult[20] = "";
      dtostrf(t, 3, 1, tresult);
      delay(10);
      if (debug) {
        Serial.println(tresult);
      }
      client.publish(tTopic, tresult);
      //client.publish(tpubTopic, tresult);
      which = false;
    }

  }


}

