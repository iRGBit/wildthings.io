// Kaituhituhi-rua prototype - temperature

// blue wire - G

// brown wire          D7 - <5°C – 5°C - fair:5 (yel)
// green wire          D4 - 5°C – 9.9°C - good:8 (grn)
// orange wire         D2 - 10°C – 14.9°C - excellent:10 (grn)
// blue/white wire     D4 - 15°C – 19.9°C - good:5 (grn)
// green/white wire    D8 - 20°C – 24.9°C - fair:5 (yel)
// brown/white wire    D5 - 25°C – 29.9°C - poor:0 (red)
// orange/white wire   D6 - 30°C< - poor:-5 (red)


// LEDs
int ledPins[] = {D1, D2, D3, D4, D5, D6, D7, D8};

int ledState[8];

unsigned long previousMillis = 0;

const long interval = 1000;



void setup() {
  for (int p = 0; p < 8; p++) {
    pinMode(ledPins[p], OUTPUT);
    ledState[p] = LOW;
  }
}

void loop() {
  for (int c = 0; c < 4; c++) {
    for (int p = 0; p < 8; p++) {

      digitalWrite(ledPins[p], HIGH);
    }
    delay(500);


    for (int p = 0; p < 8; p++) {

      digitalWrite(ledPins[p], LOW);

    }


    delay(500);
  }

}


