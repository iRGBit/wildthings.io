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
//   Wemos D1 with LEDs to visualise turbidity data
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
#define node_name "Papawai-Kaituhituhi-Toru"
#define turbidityTopic "papawai/renga"



// LEDs
int ledPins[] = {D3, D0, D5, D7, D8, D6};

static int ledNum = 6;
int ledState[6];

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
int myTurbdity = 0;


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

  }

  if (strcmp(topic, turbidityTopic) == 0) {
    String reading = "";
    for (int i = 0; i < length; i++) {
      char receivedChar = (char)payload[i];
      reading += receivedChar;
    }
    float turbidity = reading.toFloat(); // fetch current temperature as float
    // then switch the right one on:
    if (debug) {
      Serial.println(turbidity);
    }
    for (int i = 0; i < ledNum; i++) {
      digitalWrite(ledPins[i], HIGH);
    }
    int num = (100 / turbidity) * 6;
    // for some reason, a loop for the following task just wouldn't work...
    for (int i = 0; i < num; i++) {
      digitalWrite(ledPins[i], HIGH);
    }
    delay(1000);
    for (int i = 0; i < num; i++) {
      digitalWrite(ledPins[i], LOW);
    }
    delay(1000);
    /*    if (temp < 5) {
          digitalWrite(D7, HIGH);
        }
        else if (temp >= 5 && temp < 10 ) {
          digitalWrite(D4, HIGH);
        }
        else if (temp >= 10 && temp < 15 ) {
          digitalWrite(D2, HIGH);
        }
        else if (temp >= 15 && temp < 20 ) {
          digitalWrite(D3, HIGH);
        }
        else if (temp >= 20 && temp < 25 ) {
          digitalWrite(D8, HIGH);
        }
        else if (temp >= 25 && temp < 30 ) {
          digitalWrite(D5, HIGH);
        }
        else if (temp >= 30) {
          digitalWrite(D6, HIGH);
        }

      }*/



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
        client.subscribe(turbidityTopic);



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

