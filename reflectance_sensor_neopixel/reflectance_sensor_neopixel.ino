#include <Adafruit_NeoPixel.h>


// Number of LEDs in total
const int nLEDs = 36;
// The offset, used to ensure we go to full and zero after adjusting for ranges
const int offset = 30;
// The pixel strip, connected to digital pin 6
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(nLEDs, 6, NEO_GRB + NEO_KHZ800);

// Input from reflectance sensor on A6
void setup() {
  Serial.begin(9600);
  pixels.begin();
}

// Values used to auto-calibrate the sensor
int maxValue = 512;
int minValue = 511;

void loop() {
  int reading = analogRead(A6);
  if (reading > maxValue) {
    maxValue = reading;
  }
  if (reading < minValue) {
    minValue = reading;
  }
  Serial.print(reading);
  Serial.print(" ");
  int val = map(reading, minValue+offset, maxValue-offset, 0, nLEDs);
  Serial.println(val);
  for (int i = 0; i < nLEDs; i++) {
    if (i < val) {
      pixels.setPixelColor(i, pixels.Color(0,30,30));
    }
    else {
      pixels.setPixelColor(i, pixels.Color(255,200,20));
    }
  }
  pixels.show();
}

