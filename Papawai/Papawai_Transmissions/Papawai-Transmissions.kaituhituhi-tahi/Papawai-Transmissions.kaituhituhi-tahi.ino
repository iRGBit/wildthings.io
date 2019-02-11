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
//   Wemos D1 with LED to visualise EC stream
//
//   To record datastream on your laptop: mosquitto_sub -v -h 192.168.42.1 -p 1883 -t '#' | xargs -d$'\n' -L1 sh -c 'date "+%D %T $0"' > 1612-nr2-datalog-2.log
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
#define ecTopic "papawai/ec"
#define spikeTopic "papawai/spike" // spike in water - blink



// LEDs
int ledPins[] = {D0, D5, D6, D7, D8};

int ledState[5];

unsigned long previousMillis = 0;

const long interval = 1000;

//Debug Mode
boolean debug = false; // set to true to read data via Serial

static int count = 0;

// Broker Setup
const char* ssid = "Moturoa_Transmissions";
const char* password = "Tangaroa";
const char* mqtt_server = "192.168.42.1";

//LED fading
int brightness = 0;
int fadeAmount = 5;


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

  if (strcmp(topic, spikeTopic) == 0) {
    Serial.println("SPIKE");

    for (int c = 0; c < 4; c++) {
      for (int p = 0; p < 5; p++) {

        digitalWrite(ledPins[p], HIGH);
        if (debug) {
          Serial.println("ON");
        }
      }

      delay(500);
      for (int p = 0; p < 5; p++) {

        digitalWrite(ledPins[p], LOW);
        if (debug) {
          Serial.println("OFF");
        }
      }
      

      delay(500);
    }
  }

  else {

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
        client.subscribe(spikeTopic);
        client.subscribe(ecTopic);



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
  for (int p = 0; p < 5; p++) {
    pinMode(ledPins[p], OUTPUT);
    ledState[p] = LOW;
  }

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

  analogWrite(ledPins[3], brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(30);


  //  if (debug) { // just counting LEDs 1-5 [0-4]
  //    unsigned long currentMillis = millis();
  //    if (currentMillis - previousMillis >= interval) {
  //      previousMillis = currentMillis;
  //      //    if (ledState[count] == LOW) {
  //      //      ledState[count] = HIGH;
  //      //    }
  //      //    else {
  //      //      ledState[count] = LOW;
  //      //    }
  //      for (int p = 0; p < 5; p++) {
  //        digitalWrite(ledPins[p], LOW);
  //      }
  //      digitalWrite(ledPins[count], HIGH);
  //      if (count < 4) {
  //        count++;
  //      }
  //      else
  //      {
  //        count = 0;
  //      }
  //
  //    }
  //  }



}

