#include "lightStructs.h"
#include <Adafruit_NeoPixel.h>

const int nLEDs = 36;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(nLEDs, 6, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin();
  Serial.begin(57600);
}

int offset = 0;

void loop() {
  int n;
  for (n = 0 ; n < 24; n++) {
    int h = (int)((float)256.0 * (float(n)/24.0));
    RGB rgb = hsvToColour(h+offset, 255, 255);
    pixels.setPixelColor(n, pixels.Color(rgb.r, rgb.g, rgb.b)); 
  }
  for (n = 0 ; n < 12; n++) {
    int h = (int)((float)256.0 * (float(n)/12.0));
    RGB rgb = hsvToColour(h+offset, 255, 255);
    pixels.setPixelColor(n+24, pixels.Color(rgb.r, rgb.g, rgb.b)); 
  }
  offset+=2;
  offset=(offset+256) % 256;
  pixels.show();
}


RGB hsvToColour(unsigned int h, unsigned int s, unsigned int v) {
  unsigned char region, remainder, p, q, t;
  h = (h+256) % 256;
  if (s > 255) s = 255;
  if (v > 255) v = 255;
  else v = (v * v) >> 8;
  if (s == 0) return (RGB){
    v, v, v  };
  region = h / 43;
  remainder = (h - (region * 43)) * 6; 
  p = (v * (255 - s)) >> 8;
  q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
  switch (region) {
  case 0:
    return (RGB){
      v, p, t    };
  case 1:
    return (RGB){
      q, p, v    };
  case 2:
    return (RGB){
      p, t, v    };
  case 3:
    return (RGB){
      p, v, q    };
  case 4:
    return (RGB){
      t, v, p    };
  }
  return (RGB){
    v, q, p  };
}

