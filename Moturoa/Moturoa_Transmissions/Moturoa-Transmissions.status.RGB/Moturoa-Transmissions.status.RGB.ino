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
//   Wemos D1 with RGB shield monitoring and visualizing network traffic of Moturoa Stream WiFi, Te Upoko a te Ika a Maui/Wellington

//   credits to 
//   https://www.baldengineer.com/mqtt-tutorial.html
//   with adaptions as in the comments by Dag Rende & William Brinkman
//   Adafruit Neopixel Uberguide https://cdn-learn.adafruit.com/downloads/pdf/adafruit-neopixel-uberguide.pdf


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Adafruit_NeoPixel.h>
#define PIN            D2

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, PIN, NEO_GRB + NEO_KHZ800);


#define node_name "ESP8266_PIR/network"
#define all "#"

#define hTopic "moturoa/ahumid" // DHT11 humidity
#define tTopic "moturoa/atemp" // DHT11 temp - pink
#define humidTopic "moturoa/humid" // soil - 'brown/orange'
#define rainTopic "moturoa/rain" // rain - no rain: 3x green, rain: 3x orange
#define ecTopic "moturoa/ec" // conduct
#define spikeTopic "moturoa/spike" // spike in water - red
#define wTopic "moturoa/watertemp" // violet x 2


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
  pixels.setPixelColor(0, pixels.Color(0, 0, 255)); //blue
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(700);
  pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
  pixels.show(); // This sends the updated pixel color to the hardware.


  if (strcmp(topic, spikeTopic) == 0)
  { pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // red
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(3000);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
  else if (strcmp(topic, tTopic) == 0) // air temperature
  { pixels.setPixelColor(0, pixels.Color(255, 51, 153)); // pink
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(3000);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
  else if (strcmp(topic, rainTopic) == 0) // rain
  {
    for (int i = 0; i < length; i++) {
      char receivedChar = (char)payload[i];
      //Serial.print(receivedChar);
      if (receivedChar == '1') { // dry

        for (int i = 0; i < 3; i++) {
          pixels.setPixelColor(0, pixels.Color(51, 255, 51)); // green
          pixels.show(); // This sends the updated pixel color to the hardware.
          delay(500);
          pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
          pixels.show(); // This sends the updated pixel color to the hardware.
          delay(500);

        }
      }
      else if (receivedChar == '0') { // rain
        for (int i = 0; i < 3; i++) {

          pixels.setPixelColor(0, pixels.Color(255, 128, 0)); // orange
          pixels.show(); // This sends the updated pixel color to the hardware.
          delay(500);
          pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
          pixels.show(); // This sends the updated pixel color to the hardware.
          delay(500);

        }
      }

    }
  }
  else if (strcmp(topic, wTopic) == 0)
  {
    for (int i = 0; i < 2; i++) {

      pixels.setPixelColor(0, pixels.Color(153, 1, 255)); // violet x2
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(1000);
      pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(1000);
    }
  }
  else if (strcmp(topic, humidTopic) == 0)
  {

      pixels.setPixelColor(0, pixels.Color(153, 76, 0)); // 'brown/orange'
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(1000);
      pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
      pixels.show(); // This sends the updated pixel color to the hardware.
      delay(1000);
    
  }
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
      pixels.setPixelColor(0, pixels.Color(255, 255, 0)); //yellow
      pixels.show(); // This sends the updated pixel color to the hardware.
      // Wait 5 seconds before retrying
      delay(5000);
      pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
      pixels.show(); // This sends the updated pixel color to the hardware.


    }
  }
}

void setup()
{
  Serial.begin(9600);
  pixels.begin(); // This initializes the NeoPixel library.

  setup_wifi();             // added this line of code !!!!!!!

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


long lastMsg = 0;
float light = 0.0;
float diff = 20.0;


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
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(1000);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(2000);


  }

  Serial.println("");
  Serial.println("WiFi connected");
  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(5000);
  pixels.setPixelColor(0, pixels.Color(0, 0, 0, 0)); //off
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(3000);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

bool checkBound(float newValue, float prevValue, float maxDiff) {
  return !isnan(newValue) &&
         (newValue < prevValue - maxDiff || newValue > prevValue + maxDiff);
}
