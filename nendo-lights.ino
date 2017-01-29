
#include "FastLED.h"
#include "Metro.h"

#define NUM_LEDS 60
#define DATA_PIN 17
#define BRIGHTNESS 128

int highlight_aa = 10;
int highlight_width = 10 * highlight_aa;

CRGB leds[NUM_LEDS];

int highlight_index = 0;
CRGB base_color;

int8_t bg_index;

int offset_index = 0;
int offset_width = 30 * highlight_aa;
int offset_scale = 90;

Metro highlight_update = Metro(33);
Metro bg_update = Metro(1234);
Metro offset_update = Metro(64);

void setup() {
  delay(2000);

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  random16_set_seed(analogRead(0));
  bg_index = random8();

}


void loop() {
  if (bg_update.check() == 1)
  {
    ++bg_index;
  }
  if (highlight_update.check() == 1)
  {
    highlight_index = (highlight_index + 1) % (NUM_LEDS * highlight_aa);
  }
  if (offset_update.check() == 1)
  {
    --offset_index;
    if (offset_index < 0)
      offset_index = NUM_LEDS * highlight_aa;
  }


  // fill_solid(leds, NUM_LEDS, );
  for (int i = 0; i < NUM_LEDS; i += 1)
  {
    int highlight_dist = min(min(
      abs((highlight_index) - ((i - NUM_LEDS) * highlight_aa)),
      abs((highlight_index) - ((i + NUM_LEDS) * highlight_aa))),
      abs((highlight_index) - (i * highlight_aa)));
    
    if (highlight_dist > highlight_width)
      highlight_dist = highlight_width;

    int offset_dist = min(min(
      abs((offset_index) - ((i - NUM_LEDS) * highlight_aa)),
      abs((offset_index) - ((i + NUM_LEDS) * highlight_aa))),
      abs((offset_index) - (i * highlight_aa)));
    if (offset_dist > offset_width)
      offset_dist = offset_width;

    offset_dist = /*ease8InOutApprox*/(255 - 255 * offset_dist / offset_width);
    leds[i] = CHSV(
      bg_index + scale8(offset_scale, offset_dist), 255, 255);

    nblend(leds[i], CRGB::White, 
      /*ease8InOutApprox*/(255 - 255 * highlight_dist / highlight_width));
  }

  FastLED.show();
}
