//             _ _     _ _   _     _                   _
//            (_) |   | | | | |   (_)                 (_)
//   __      ___| | __| | |_| |__  _ _ __   __ _ ___   _  ___
//   \ \ /\ / / | |/ _` | __| '_ \| | '_ \ / _` / __| | |/ _ \ 
//    \ V  V /| | | (_| | |_| | | | | | | | (_| \__ \_| | (_) |
//     \_/\_/ |_|_|\__,_|\__|_| |_|_|_| |_|\__, |___(_)_|\___/
//                                         __ / |
//                                       | ___ /

//   Papawai Transmissions
//   LED visualiser for temperature probe
//   MQTT Client for Wemos D1 Pro
//   wildthings.io - Birgit Bachler, Aotearoa/New Zealand, 2019
//
//   Wemos D1 Pro
//
//   To record datastream on your laptop: mosquitto_sub -v -h 192.168.42.1 -p 1883 -t '#' | xargs -d$'\n' -L1 sh -c 'date "+%D %T $0"' > 1612-nr2-datalog-2.log
//   credits to
//   James Lewis : https://www.baldengineer.com/mqtt-tutorial.html with adaptions as in the comments by Dag Rende & William Brinkman
//   ItKindaWorks : http://github.com/ItKindaWorks - https://github.com/ItKindaWorks/ESP8266/blob/master/Home%20Automation/Part%201/ESP8266_SimpleMQTT/ESP8266_SimpleMQTT.ino
//   Paulus Schoutsen : https://home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/
//
//   Requires PubSubClient found here: https://github.com/knolleary/pubsubclient

// Wire Scheme:
// blue wire - G

// brown wire          D7 - <5°C – 5°C - fair:5 (yel)
// green wire          D4 - 5°C – 9.9°C - good:8 (grn)
// orange wire         D2 - 10°C – 14.9°C - excellent:10 (grn)
// blue/white wire     D4 - 15°C – 19.9°C - good:5 (grn)
// green/white wire    D8 - 20°C – 24.9°C - fair:5 (yel)
// brown/white wire    D5 - 25°C – 29.9°C - poor:0 (red)
// orange/white wire   D6 - 30°C< - poor:-5 (red)


// MQTT

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define node_name "Papawai-Kaituhituhi-Rua"
#define waterTempTopic "moturoa/watertemp"



// LEDs
int ledPins[] = {D1, D2, D3, D4, D5, D6, D7, D8};

int ledState[8];

unsigned long previousMillis = 0;

const long interval = 1000;

//Debug Mode
boolean debug = true; // set to true to read data via Serial

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



    if (strcmp(topic, waterTempTopic) == 0) {
      String reading = "";
      for (int i = 0; i < length; i++) {
        char receivedChar = (char)payload[i];
        reading += receivedChar;
      }
      float temp = reading.toFloat(); // fetch current temperature as float
      // then switch the right one on:
      Serial.println(temp);

      // for some reason, a loop for the following task just wouldn't work...
      digitalWrite(D1, LOW);
      digitalWrite(D2, LOW);
      digitalWrite(D3, LOW);
      digitalWrite(D4, LOW);
      digitalWrite(D5, LOW);
      digitalWrite(D6, LOW);
      digitalWrite(D7, LOW);
      digitalWrite(D8, LOW);

      delay(100);
      if (temp < 5) {
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

    }

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
        client.subscribe(waterTempTopic);



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
  for (int p = 0; p < 8; p++) {
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

  //idle

}

