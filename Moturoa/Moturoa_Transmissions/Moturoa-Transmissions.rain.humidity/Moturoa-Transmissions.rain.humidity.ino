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
//   Wemos D1 with a rain sensor FR-04 & DFRobot soil hygrometer EDT-368835 monitoring the environment of Moturoa Stream, Te Upoko a te Ika a Maui/Wellington

//   Moturoa Transmissions - MQTT Client for Wemos D1 - wildthings.io - Birgit Bachler 2017
//   credits to
//   James Lewis : https://www.baldengineer.com/mqtt-tutorial.html with adaptions as in the comments by Dag Rende & William Brinkman
//   ItKindaWorks : http://github.com/ItKindaWorks - https://github.com/ItKindaWorks/ESP8266/blob/master/Home%20Automation/Part%201/ESP8266_SimpleMQTT/ESP8266_SimpleMQTT.ino
//   Paulus Schoutsen : https://home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/
//
//    Requires PubSubClient found here: https://github.com/knolleary/pubsubclient
//




// MQTT
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define node_name "Moutroa-humidrain"
#define rainTopic "moturoa/rain"
#define rpubTopic "moturoa/glowworm1/rain"
#define humidTopic "moturoa/humid"
#define hpubTopic "moturoa/glowworm1/humid"

// rain sensor
#define rainSensor D3 // FR-04
#define humidSensor A0 // EDT-368835


// Broker Setup
const char* ssid = "Moturoa_Transmissions";
const char* password = "Tangaroa";
const char* mqtt_server = "192.168.42.1";

String rain_str; //see last code block below use these to convert the float that you get back from DHT to a string =str
String humid_str;
char rain[50];
char humid[50];

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long boardTime = 0 ;
boolean which = false;


void callback(char* topic, byte * payload, unsigned int length) {
  //convert topic to string to make it easier to work with

  //Print out some debugging info
  Serial.println("Callback update.");

  Serial.print("Topic: ");
  Serial.println(topic);

  //client.publish(confirmTopic, "ERROR");

}

void reconnect() {
  //attempt to connect to the wifi if connection is lost
  if (WiFi.status() != WL_CONNECTED) {
    long waitTime = millis();
    //debug printing
    //    Serial.print("Connecting to ");
    //    Serial.println(ssid);
    //

    //loop while we wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      //      Serial.print(".");
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
        Serial.println("connected");
        // ... and subscribe to topic
        //       client.subscribe(tempTopic);


      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
}


void setup() {


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
  if ((boardTime % 30000) == 0) {
    if (which == false) { // do rain
      int val = digitalRead(rainSensor);     // read the input pin
      rain_str = String(val);
      rain_str.toCharArray(rain, rain_str.length() + 1); //packaging up the data to publish to mqtt whoa...
      client.publish(rainTopic, rain);
      // client.publish(rpubTopic, rain);
      which = true;
    }
    else { //do humid
      int val2 = analogRead(humidSensor);     // read the input pin
      humid_str = String(val2);
      humid_str.toCharArray(humid, humid_str.length() + 1); //packaging up the data to publish to mqtt whoa...
      client.publish(humidTopic, humid);
      //  client.publish(hpubTopic, humid);
      which = false;
    }

  }


}

