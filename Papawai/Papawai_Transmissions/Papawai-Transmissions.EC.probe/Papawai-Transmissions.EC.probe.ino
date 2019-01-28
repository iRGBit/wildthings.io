//             _ _     _ _   _     _                   _
//            (_) |   | | | | |   (_)                 (_)
//   __      ___| | __| | |_| |__  _ _ __   __ _ ___   _  ___
//   \ \ /\ / / | |/ _` | __| '_ \| | '_ \ / _` / __| | |/ _ \ 
//    \ V  V /| | | (_| | |_| | | | | | | | (_| \__ \_| | (_) |
//     \_/\_/ |_|_|\__,_|\__|_| |_|_|_| |_|\__, |___(_)_|\___/
//                                         __ / |
//                                       | ___ /

//   Papawai Transmissions
//   MQTT Client for Wemos D1
//   wildthings.io - Birgit Bachler, Aotearoa/New Zealand, 2018



//
//   Wemos D1 with a DIY EC probe monitoring Papawai Stream, Te Upoko a te Ika a Maui/Wellington

//   credits to
//   Michael Ratcliffe: https://hackaday.io/project/7008-fly-wars-a-hackers-solution-to-world-hunger/log/24646-three-dollar-ec-ppm-meter-arduino
//   James Lewis : https://www.baldengineer.com/mqtt-tutorial.html with adaptions as in the comments by Dag Rende & William Brinkman
//   ItKindaWorks : http://github.com/ItKindaWorks - https://github.com/ItKindaWorks/ESP8266/blob/master/Home%20Automation/Part%201/ESP8266_SimpleMQTT/ESP8266_SimpleMQTT.ino
//   Paulus Schoutsen : https://home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/
//
//    Requires PubSubClient found here: https://github.com/knolleary/pubsubclient


// MQTT
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define node_name "Papawai-EC"
//#define node_name "Papawai-ECrua"
#define ecTopic "moturoa/ecrua"
#define spikeTopic "moturoa/spike"


// EC sensor
#define ecSensor A0


//Debug Mode
boolean debug = false; // set to true to read data via Serial


// Broker Setup
const char* ssid = "Moturoa_Transmissions";
const char* password = "Tangaroa";
const char* mqtt_server = "192.168.42.1";


WiFiClient espClient;
PubSubClient client(espClient);

unsigned long boardTime = 0 ;

const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int ecIndex = 0;                  // the ecIndex of the current reading
int total = 0;                  // the running total
int average = 0;                // the average noise value
const int ecThreshold = 33;

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
      Serial.print("Connecting to ");
      Serial.println(ssid);
      //

      //loop while we wait for connection
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      //print out some more debug once connected


    }

    //make sure we are connected to WIFI before attemping to reconnect to MQTT
    if (WiFi.status() == WL_CONNECTED) {
      // Loop until we're reconnected to the MQTT server
      while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
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

    for (int thisReading = 0; thisReading < numReadings; thisReading++) {
      readings[thisReading] = 0;
      total += readings[thisReading];

    }
    //start the serial line for debugging
    Serial.begin(9600);
    delay(100);


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
    boardTime = millis();

    if ((boardTime % 10000) == 0) {
      total = total - readings[ecIndex];
      float ec = analogRead(ecSensor);     // read the input pin
      readings[ecIndex] = ec;
      total = total + readings[ecIndex];
      ecIndex = ecIndex + 1;

      if (ecIndex >= numReadings) {
        // ...wrap around to the beginning:
        ecIndex = 0;
      }

      average = total / numReadings;
      if (ec > average + ecThreshold || ec < average - ecThreshold) {
        Serial.println("Spike");
        client.publish(spikeTopic, "1");   // sets the LED on  }
      }


      char ecresult[20] = "";
      dtostrf(ec, 3, 1, ecresult);
      delay(10);
      // Serial.println(ec);

      client.publish(ecTopic, ecresult);

    }
  }

