#include "LPD8806.h"
#include "SPI.h"
#include "lightStructs.h"

const int nLEDs = 260;
LPD8806 strip = LPD8806(nLEDs);
const int blobCount = 4;
const int realmHeight = 64;
const int realmBorder = 24;

// Array of blobs, each blob has x,y and RGB values
Blob blobs[blobCount];

void setup() {
  strip.begin();
  strip.show();
  randomSeed(analogRead(0));
  int n;
  for (n = 0; n < blobCount; n++) {
    RGB col = hsvToColour(randomHue(),255,255);
      blobs[n] = (Blob){col.r, col.g, col.b, random(strip.numPixels()), realmHeight - (realmHeight / blobCount) * n};
  }
  Serial.begin(57600);
}

void loop() {
  int n;
  for (n=0; n<strip.numPixels(); n++) {
     setPixel(n, getColour(n, realmHeight/2)); 
  }
  strip.show();
  advanceBlobs();
}

int randomHue() {
  // Hues range from 0 to 255, the line below produces negative ones
  // but that's fine as the hsv->rgb algorithm wraps negative hue values.
  // In this case summing two random 0-20 and then subtracting 20 leads
  // to mostly reds with some purples and yellows.
  return random(20)+random(20)-20;
}

// Decrement the Y coordinate of all blobs, if this leads to a coordinate < 0 create a new
// blob at the top of the realm
void advanceBlobs() {
  int n;
  for (n = 0; n < blobCount; n++) {
    blobs[n].y = blobs[n].y - 1;
     if (blobs[n].y < 0) {
       RGB col = hsvToColour(randomHue(),255,255);
       blobs[n] = (Blob){col.r, col.g, col.b, random(strip.numPixels()), realmHeight-1};
     } 
     
  }
}

// Get the colour given the influence of all blobs at the given position
RGB getColour(int x, int y) {
  float influence[blobCount];
  float r,g,b, sum;
  // Calculate influence as the inverse of distance - if distance is zero
  // assume influence to be large (10f)
  int n;
  sum = 0.0f;
  for (n = 0; n < blobCount; n++) {
   float dx = x - blobs[n].x;
   float dy = y - blobs[n].y;
   if (abs(dx) > (strip.numPixels()/2)) {
     dx = (float)(strip.numPixels()) - (float)abs(dx);
   }
   if (dx ==0 && dy ==0) {
      influence[n] = 100.0f * influenceFactor(blobs[n].y);
   } 
   else {
      float d = dx*dx+dy*dy;
      influence[n] = 100.0f/(d) * influenceFactor(blobs[n].y); 
   }
   sum+=influence[n];
  }
  
  // Normalise such that influences sum to zero, assemble aggregate colour
  // values to convert to an RGB
  r=0.0f;
  g=0.0f;
  b=0.0f;
  for (n = 0; n < blobCount; n++) {
   influence[n] = influence[n] / sum;
   r += ((float)(blobs[n].r)) * influence[n];
   g += ((float)(blobs[n].g)) * influence[n];
   b+=((float)(blobs[n].b)) * influence[n];
  }
  return (RGB){(int)r,(int)g,(int)b};
}


// Return the influence multiplier for a given Y coordinate - use this
// to produce a gradual fade in and fade out based rather than having blobs
// just appear and disappear suddenly when hitting the boundaries of the
// grid.
float influenceFactor(int y) {
  if (y > realmBorder && y < (realmHeight - realmBorder)) {
    return 1.0f;
  }
  else if (y < realmBorder) {
     return ((float)(y) / (float)(realmBorder)); 
  }
  else {
     return ((float)(realmHeight - y) / (float)(realmBorder)); 
  }
}

void setPixel(int index, RGB colour) {
 strip.setPixelColor(index, strip.Color(colour.r >> 1, colour.g >> 1, colour.b >> 1));
}

RGB hsvToColour(unsigned int h, unsigned int s, unsigned int v) {
  unsigned char region, remainder, p, q, t;
  h = (h+256) % 256;
  if (s > 255) s = 255;
  if (v > 255) v = 255;
  else v = (v * v) >> 8;
  if (s == 0) return (RGB){v, v, v};
  region = h / 43;
  remainder = (h - (region * 43)) * 6; 
  p = (v * (255 - s)) >> 8;
  q = (v * (255 - ((s * remainder) >> 8))) >> 8;
  t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
  switch (region) {
  case 0:
    return (RGB){v, p, t};
  case 1:
    return (RGB){q, p, v};
  case 2:
    return (RGB){p, t, v};
  case 3:
    return (RGB){p, v, q};
  case 4:
    return (RGB){t, v, p};
  }
  return (RGB){v, q, p};
}
