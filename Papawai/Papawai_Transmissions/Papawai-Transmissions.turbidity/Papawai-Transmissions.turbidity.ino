//             _ _     _ _   _     _                   _
//            (_) |   | | | | |   (_)                 (_)
//   __      ___| | __| | |_| |__  _ _ __   __ _ ___   _  ___
//   \ \ /\ / / | |/ _` | __| '_ \| | '_ \ / _` / __| | |/ _ \ 
//    \ V  V /| | | (_| | |_| | | | | | | | (_| \__ \_| | (_) |
//     \_/\_/ |_|_|\__,_|\__|_| |_|_|_| |_|\__, |___(_)_|\___/
//                                         __ / |
//                                       | ___ /

//   Papawai Transmissions
//   1st Prototype for Wemos D1 mini for measuring turbidity
//
//   wildthings.io - Birgit Bachler, Aotearoa/New Zealand, 2019
//

boolean debug = false;

// MQTT
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define node_name "papawai/haranu" // https://maoridictionary.co.nz/search?idiom=&phrase=&proverb=&loan=&histLoanWords=&keywords=haranu
#define turbidityTopic "papawai/renga"

// Broker Setup
const char* ssid = "Moturoa_Transmissions";
const char* password = "Tangaroa";
const char* mqtt_server = "192.168.42.1";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long boardTime = 0 ;

#define sensorPin A0    // select the input pin for the LDR
#define ledPin D1 // select output pin for LED

int murkValue = 0;  // variable to store the value coming from the sensor
int ledValue = 512; // low light LED

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
        // client.subscribe(notopic);



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
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

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

  if ((boardTime % 5000) == 0) {

    analogWrite(ledPin, ledValue);
    delay(100);
    murkValue = analogRead(sensorPin);  // read the value from the sensor:
    if (debug) {
      Serial.println(murkValue);
    }
    char result[20] = "";
    dtostrf(murkValue, 3, 1, result);
    delay(10);
    client.publish(turbidityTopic, result);
    analogWrite(ledPin, 0);

  }

}
