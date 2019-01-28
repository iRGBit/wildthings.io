//             _ _     _ _   _     _                   _       
//            (_) |   | | | | |   (_)                 (_)      
//   __      ___| | __| | |_| |__  _ _ __   __ _ ___   _  ___  
//   \ \ /\ / / | |/ _` | __| '_ \| | '_ \ / _` / __| | |/ _ \ 
//    \ V  V /| | | (_| | |_| | | | | | | | (_| \__ \_| | (_) |
//     \_/\_/ |_|_|\__,_|\__|_| |_|_|_| |_|\__, |___(_)_|\___/ 
                                        __/ |              
                                       |___/               

//   Papawai Transmissions 
//   1st Prototype for Wemos D1 mini for measuring turbidity
//
//   wildthings.io - Birgit Bachler, Aotearoa/New Zealand, 2019
//



int sensorPin = A0;    // select the input pin for the LDR
int sensorValue = 0;  // variable to store the value coming from the sensor
int ledPin = D1; // select output pin for LED
int ledValue = 64; // low light LED 

boolean debug = true;
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // read the value from the sensor:
  analogWrite(ledPin, ledValue);
  delay(10);
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);

  delay(500);

}
