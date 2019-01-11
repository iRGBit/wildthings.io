//             _ _     _ _   _     _                   _
//            (_) |   | | | | |   (_)                 (_)
//   __      ___| | __| | |_| |__  _ _ __   __ _ ___   _  ___
//   \ \ /\ / / | |/ _` | __| '_ \| | '_ \ / _` / __| | |/ _ \ 
//    \ V  V /| | | (_| | |_| | | | | | | | (_| \__ \_| | (_) |
//     \_/\_/ |_|_|\__,_|\__|_| |_|_|_| |_|\__, |___(_)_|\___/
//                                         __ / |
//                                       | ___ /

//   Papawai Transmissions
//   MQTT Client for Wemos D1 Pro
//   wildthings.io - Birgit Bachler, Aotearoa/New Zealand, 2019
//
//   Wemos D1 Pro

//   credits to
//   James Lewis : https://www.baldengineer.com/mqtt-tutorial.html with adaptions as in the comments by Dag Rende & William Brinkman
//   ItKindaWorks : http://github.com/ItKindaWorks - https://github.com/ItKindaWorks/ESP8266/blob/master/Home%20Automation/Part%201/ESP8266_SimpleMQTT/ESP8266_SimpleMQTT.ino
//   Paulus Schoutsen : https://home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/
//
//    Requires PubSubClient found here: https://github.com/knolleary/pubsubclient


// MQTT
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define node_name "Papawai-Kaituhituhi-Tahi"
#define ecTopic "papawai/katuhi"


// EC sensor
#define ledPin D2

int ledState = LOW;

unsigned long previousMillis = 0;

const long interval = 1000;

//Debug Mode
boolean debug = false; // set to true to read data via Serial


// Broker Setup
const char* ssid = "Moturoa_Transmissions";
const char* password = "Tangaroa";
const char* mqtt_server = "192.168.42.1";


WiFiClient espClient;
PubSubClient client(espClient);

unsigned long boardTime = 0 ;

void callback(char* topic, byte * payload, unsigned int length) {
  //convert topic to string to make it easier to work with

  //Print out some debugging info
  if (debug) {
    Serial.println("Callback update.");

    Serial.print("Topic: ");
    Serial.println(topic);
    //hl;
    //client.publish(confirmTopic, "ERROR");

  }
}
void reconnect() {
  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    long waitTime = millis();
    //debug printing
    if (debug) {
      Serial.print("Connecting to ");
      Serial.println(ssid);
    }
    //

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if (debug) {
        Serial.print(".");
      }
    }

    //print out some more debug once connected


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
        delay(100);
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
  pinMode(ledPin, OUTPUT);

  if (debug) {
    //start the serial line for debugging
    Serial.begin(9600);
    delay(100);
  }

  //start wifi subsystem
  WiFi.begin(ssid, password);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();

  //wait a bit before starting the main loop
  delay(2000);


}


void loop() {

  if (!client.connected() && WiFi.status() == 3) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
    }
    else {
      ledState = LOW;
    }
    digitalWrite(ledPin, ledState);
  }
}

