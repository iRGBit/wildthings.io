// Kaituhituhi-rua prototype - temperature

// brown wire - D7 - <5°C – 5°C - fair (yel)
// green wire - D4 - 5°C – 9.9°C - good (grn)
// orange wire - D2 - 10°C – 14.9°C - excellent (grn)
// blue/white wire - D4 -


// LEDs for temperature probe
// int ledPins[] = {D1, D2, D3, D4, D5, D6, D7, D8};
// int ledState[8];


// LEDs for turbidity output
int ledPins[] = {D0, D6, D8};
int ledState[3];


unsigned long previousMillis = 0;

const long interval = 1000;



void setup() {
  for (int p = 0; p < 3; p++) {
    pinMode(ledPins[p], OUTPUT);
    ledState[p] = LOW;
  }
}

void loop() {
  for (int c = 0; c < 4; c++) {
    for (int p = 0; p < 3; p++) {

      digitalWrite(ledPins[p], HIGH);
      delay(500);
      digitalWrite(ledPins[p], LOW);
      delay(700);


    }




    delay(500);
  }

}
