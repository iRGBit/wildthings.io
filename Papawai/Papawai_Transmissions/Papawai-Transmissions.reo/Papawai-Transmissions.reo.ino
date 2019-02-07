//             _ _     _ _   _     _                   _
//            (_) |   | | | | |   (_)                 (_)
//   __      ___| | __| | |_| |__  _ _ __   __ _ ___   _  ___
//   \ \ /\ / / | |/ _` | __| '_ \| | '_ \ / _` / __| | |/ _ \ 
//    \ V  V /| | | (_| | |_| | | | | | | | (_| \__ \_| | (_) |
//     \_/\_/ |_|_|\__,_|\__|_| |_|_|_| |_|\__, |___(_)_|\___/
//                                         __ / |
//                                       | ___ /

//   Papawai Transmissions
//   Audio Jar
//   Adafruit Feather Huzzah + Audioshield
//   MIDI jumper soldered closed on bottom

// MQTT

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define node_name "Audiojar"
#define waterTempTopic "moturoa/watertemp"
#define ecTopic "moturoa/ec"


// MIDI / Feather
// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
#define VS1053_GM1_OCARINA 80



#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0


#if defined(__AVR_ATmega32U4__) || defined(ARDUINO_SAMD_FEATHER_M0) || defined(TEENSYDUINO) || defined(ARDUINO_STM32_FEATHER)
#define VS1053_MIDI Serial1
#elif defined(ESP32)
HardwareSerial Serial1(2);
#define VS1053_MIDI Serial1
#elif defined(ESP8266)
#define VS1053_MIDI Serial
#endif


//Debug Mode
boolean debug = true; // set to true to read data via Serial


// Broker Setup
const char* ssid = "Moturoa_Transmissions";
const char* password = "Tangaroa";
const char* mqtt_server = "192.168.42.1";



WiFiClient espClient;
PubSubClient client(espClient);


void callback(char* topic, byte * payload, unsigned int length) {
  //convert topic to string to make it easier to work with

  //Print out some debugging info
  if (debug) {
    Serial.println("Callback update.");

    Serial.print("Topic: ");
    Serial.println(topic);

  }
  // for now just playing a test ocarina scale when ec or watertemp send some data
  for (uint8_t i = 60; i < 69; i++) {
    midiNoteOn(0, i, 127);
    delay(100);
    midiNoteOff(0, i, 127);
  }
  // play end

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
  delay(1000);

  if (debug) {
    //start the serial line for debugging
    Serial.begin(9600);
    delay(100);
    Serial.println("VS1053 MIDI test");
  }
  //start wifi subsystem
  WiFi.begin(ssid, password);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  //attempt to connect to the WIFI network and then connect to the MQTT server
  reconnect();


  // start midi
  VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'

  midiSetChannelBank(0, VS1053_BANK_MELODY);

  midiSetChannelVolume(0, 127);

  midiSetInstrument(0, VS1053_GM1_OCARINA);
  
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

// all midi functions below
void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);
  delay(10);
  VS1053_MIDI.write(inst);
  delay(10);
}


void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t)MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_ON | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}
