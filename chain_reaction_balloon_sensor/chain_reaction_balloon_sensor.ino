#include <RunningMedian.h>
#include <Adafruit_NeoPixel.h>

/**
 * Sketch to use an IR rangefinder to detect a rising balloon, switching a relay when 
 * the balloon gets closer than a certain distance. Once tripped the sensor can only 
 * be reset by powering it down and rebooting it. For the Chain Reaction 2014 event.
 *
 * Licensed under the GPL v3, created by Tom Oinn, 21st November 2014
 */

// The pin connected to the data line of the neopixel rings
const int PIXEL_PIN = 10;
// Number of LEDs in the neopixel array
const int LED_COUNT = 36;
// The pin connected to the relay 1 input
const int RELAY_1_PIN = 4;
// The pin connected to the relay 2 input
const int RELAY_2_PIN = 3;
// The pin connected to the IR rangefinder sensor output
const int RANGE_PIN = A5;
// The pin connected to the wiper of the calibration pot
const int POT_PIN = A4;

// Rolling median buffer size
const int MEDIAN_BUFFER_SIZE = 19;

// Hysterisis value for the sensor.
const int HYST = 20;

// Neopixel rings
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(LED_COUNT, PIXEL_PIN, NEO_RGB + NEO_KHZ800);

// Running median sample buffer, used to compensate for sensor data spikes
RunningMedian samples = RunningMedian(MEDIAN_BUFFER_SIZE, true);

// Relay states
int relay_1_state = LOW;
int relay_2_state = LOW;

void setup() {
  // Serial at 9600 baud used for debugging
  Serial.begin(9600);
  // Initialize the neopixel rings. We have a 24 pixel ring and a 12 pixel ring inside it connected in serial
  pixels.begin();
  // Set the output pins for the relays
  pinMode(RELAY_1_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, relay_1_state);
  pinMode(RELAY_2_PIN, OUTPUT);
  digitalWrite(RELAY_2_PIN, relay_2_state);
  for (int i = 0; i < MEDIAN_BUFFER_SIZE; i++) {
    samples.add(0);
  }
}

// Animation ticker, incremented every time the loop runs and used by
// the display routines, wraps at 256 so ranges from 0 to 255.
int animation = 0;

void loop() {
  animation = (animation+1) % 256;
  samples.add((long)analogRead(RANGE_PIN));
  int irSensor = samples.getMedian();
  int calibration = analogRead(POT_PIN);
  Serial.print(irSensor);
  Serial.print(", ");
  Serial.println(calibration);
  // When the sensor points at nothing we get values around 250 - as this is a very small detection
  // range otherwise we can ignore any readings here. This will lead to a dead zone about an inch and
  // a half from the sensor, but improve performance at longer ranges.
  if (irSensor < 230 || irSensor > 270) {
    if(irSensor > calibration + HYST) {
      relay_1_state = HIGH;
      relay_2_state = HIGH;
    } 
    else if (irSensor < calibration - HYST) {
      relay_1_state = LOW;
      // We don't reset relay 2 to high, this means we have a single shot latch on relay 2 and a
      // momentary one on relay 1, this gives us plenty of options to handle duck related motor driving.
    }
  }
  digitalWrite(RELAY_2_PIN, relay_2_state);
  digitalWrite(RELAY_1_PIN, relay_1_state);
  if (relay_1_state == HIGH) {
    showState(-50,30,255,255,100,255);
  }
  else {
    showState(150,200,255,255,100,255);
  }
}

void showState(int hLow, int hHigh, int sLow, int sHigh, int vLow, int vHigh) {
  clearRings();
  int innerTick = map(animation % 32, 0, 31, 0, 6);
  int outerTick = map(animation % 32, 0, 31, 0, 12);
  for (int i = 0; i < 13; i++) {
    setOuterRing((outerTick + i) % 13, map(i, 0, 13, hLow, hHigh),  map(i, 0, 13, sLow, sHigh), map(i, 0, 13, vLow, vHigh));
  }
  for (int i = 0; i < 7; i++) {
    setInnerRing((innerTick + i) % 7, map(i, 0, 7, hLow, hHigh),  map(i, 0, 7, sLow, sHigh), map(i, 0, 7, vLow, vHigh));
  }
  pixels.show();
}

// Set HSV for the outer ring, where i is from 0 to 12
void setOuterRing(int i, int h, int s, int v) {
  uint32_t col = hsvToColour(h, s, v);
  int offset = 18;
  pixels.setPixelColor((i + offset) % 24, col);
  if (i>0 && i <12) {
    pixels.setPixelColor((24-i + offset) % 24, col);
  }
}

// Set HSV for the inner ring, where i is from 0 to 6 and sets in pairs
void setInnerRing(int i, int h, int s, int v) {
  uint32_t col = hsvToColour(h, s, v);
  pixels.setPixelColor(24+i, col);
  if (i>0 && i <6) {
    pixels.setPixelColor(36-i, col);
  }
}

// Set all LEDs to black
void clearRings() {
  for (int i = 0; i < LED_COUNT; i++) {
    pixels.setPixelColor(i, 0);
  }
}

/**
 * Fast approximation for HSV to RGB, modified to return the format used by the 
 * neopixel library when setting pixel values. Hue, saturation and value range 
 * 0 to 255, with hue allowing larger or smaller values and wrapping as appropriate
 */
uint32_t hsvToColour(unsigned int h, unsigned int s, unsigned int v) {
  unsigned char region, remainder, p, q, t;
  h = (h+256) % 256;
  if (s > 255) s = 255;
  if (v > 255) v = 255;
  else v = (v * v) >> 8;
  if (s == 0) return pixels.Color(v, v, v);
  region = h / 43;
  remainder = (h - (region * 43)) * 6; 
  p = (v * (255 - s)) >> 8;
  q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
  switch (region) {
  case 0:
    return pixels.Color(v, p, t);
  case 1:
    return pixels.Color(q, p, v);
  case 2:
    return pixels.Color(p, t, v);
  case 3:
    return pixels.Color(p, v, q);
  case 4:
    return pixels.Color(t, v, p);
  }
  return pixels.Color(v, q, p);
}






