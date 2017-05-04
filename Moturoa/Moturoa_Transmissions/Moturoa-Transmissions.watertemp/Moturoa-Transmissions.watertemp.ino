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
//   Wemos D1 with Temperature Sensor DS18B20 measuring the water temperature of Moturoa Stream, Te Upoko a te Ika a Maui/Wellington
//
//   credits to
//   James Lewis : https://www.baldengineer.com/mqtt-tutorial.html with adaptions as in the comments by Dag Rende & William Brinkman
//   ItKindaWorks : http://github.com/ItKindaWorks - https://github.com/ItKindaWorks/ESP8266/blob/master/Home%20Automation/Part%201/ESP8266_SimpleMQTT/ESP8266_SimpleMQTT.ino
//   Paulus Schoutsen : https://home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/
//   One Wire DS18S20, DS18B20, DS1822 temperature example: // http://www.pjrc.com/teensy/td_libs_OneWire.html

//
//   Requires PubSubClient found here: https://github.com/knolleary/pubsubclient
//




// MQTT
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>


#define node_name "Moturoa_watertemp"
#define wTopic "moturoa/watertemp"

unsigned long boardTime = 0 ;

// Broker Setup
const char* ssid = "Moturoa_Transmissions";
const char* password = "Tangaroa";
const char* mqtt_server = "192.168.42.1";

static int wait = 70000;

WiFiClient espClient;
PubSubClient client(espClient);

OneWire  ds(2);  // on pin 2 (a 4.7K resistor is necessary)


void callback(char* topic, byte * payload, unsigned int length) {
  //convert topic to string to make it easier to work with

  //Print out some debugging info
  Serial.println("Callback update.");

  Serial.print("Topic: ");
  Serial.println(topic);


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
  if ((boardTime % 25000) == 0) {
    byte i;
    byte present = 0;
    byte type_s;
    byte data[12];
    byte addr[8];
    float celsius, fahrenheit;

    if ( !ds.search(addr)) {
      // Serial.println("No more addresses.");
      // Serial.println();
      ds.reset_search();
      delay(250);
      return;
    }
    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);        // start conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE);         // Read Scratchpad

    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();

    }

    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;

    char tresult[20] = "";
    dtostrf(celsius, 3, 1, tresult);
    delay(10);
    // Serial.println(tresult);
    client.publish(wTopic, tresult);
    //client.publish(tpubTopic, tresult);
  }
}


