
#include <FastLED.h>


CRGB leds[12];
int count = 0;

void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812B, 13, GRB>(leds, 12);
  CHSV red = CHSV(0, 255, 120);
  CHSV green = CHSV(86, 255, 150);
  CHSV white = CHSV(0, 0, 40);
  paint(red, white, 0, 3);
  paint(white, green, 3, 6);
  paint(green, white, 6, 9);
  paint(white, red, 9, 11);
}

void loop() {
  rotate();
  FastLED.show();
  delay(50);

}

/*
    Paint a smooth gradation from one HSV value to another, copes with negative ranges.
*/
void paint(CHSV a, CHSV b, int led_a, int led_b) {
  int delta = 1;
  int start_index = led_a;
  int end_index = led_b;
  if (led_a > led_b) {
    start_index = led_b;
    end_index = led_a;
  }
  leds[led_a] = a;
  leds[led_b] = b;
  for (int led = start_index + 1; led < end_index; led++) {
    uint8_t fraction = map(led, start_index, end_index, 0, 255);
    CRGB colour = CRGB(lerp8by8(leds[start_index].r, leds[end_index].r, fraction),
                       lerp8by8(leds[start_index].g, leds[end_index].g, fraction),
                       lerp8by8(leds[start_index].b, leds[end_index].b, fraction)) ;
    leds[led] = colour;
  }
}

/*
   Rotate all the LEDs around one, so moves the colour on LED n to LED n-1, with LED 11 taking LED 0's colour
*/
void rotate() {
  CRGB colour_0 = leds[0];
  for (int i = 0; i < 11; i++) {
    leds[i] = leds[i + 1];
  }
  leds[11] = colour_0;
}

/*
   Shift in a colour, pushing all other ones down the gecko
*/
void cycle(CHSV newValue) {
  for (int i = 0; i < 11; i++) {
    leds[i] = leds[i + 1];
  }
  leds[11] = newValue;
}

