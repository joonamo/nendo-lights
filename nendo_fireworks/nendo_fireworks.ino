
#include "FastLED.h"
#include "Metro.h"
#include <EEPROM.h>

#define NUM_LEDS 60
#define DATA_PIN 17

#define POWER_BUTTON 4
#define BRIGHTNESS_MINUS 0
#define BRIGHTNESS_PLUS 1
#define SATURATION_MINUS 2
#define SATURATION_PLUS 3

#define INDICATOR_LIGHT_GREEN 20
#define INDICATOR_LIGHT_RED 16

#define EEPROM_MAGICVALUE 42
#define EEPROM_BRIGHTNESS_LOC 1
#define EEPROM_SATURATION_LOC 2

#define MAX_FIREWORKS 10
struct FireWork {
  uint8_t location;
  uint8_t color;
  uint8_t width;
  uint8_t state;
};
FireWork ActiveFireworks[MAX_FIREWORKS];

uint8_t brightness = 255;
uint8_t saturation = 255;

volatile uint8_t power = 1;
volatile uint8_t indicator_state_green = 255;
volatile uint8_t indicator_state_red = 0;
volatile uint8_t error_blink = 0;

int highlight_aa = 10;
int highlight_width = 10 * highlight_aa;

CRGB leds[NUM_LEDS];

int highlight_index = 0;
CRGB base_color;

uint8_t bg_index = 0;
int8_t bg_dir = 1;

#define BG_MAX 120
#define BG_OFFSET 120 

uint8_t fade_rate = 10;

Metro fireworks_update = Metro(50);
Metro new_fireworks_update = Metro(16);
Metro brightness_update = Metro(16);
Metro bg_update = Metro(100);

CHSV background_color = CHSV(0, 128, 15);

void add_firework() {
  for (uint8_t i = 0; i < MAX_FIREWORKS; ++i) {
    if (ActiveFireworks[i].state == 0) {
      uint8_t width = random8() % 6 + 1;
      ActiveFireworks[i].width = width;
      ActiveFireworks[i].location = width + random8() % (NUM_LEDS - width * 2);
      ActiveFireworks[i].color = random8();
      ActiveFireworks[i].state = ActiveFireworks[i].width;
      return;
    }
  }
}

void update_firework(uint8_t i) {
  uint8_t dist = ActiveFireworks[i].width - ActiveFireworks[i].state;
  // for (uint8_t loc = ActiveFireworks[i].location - dist; loc <= ActiveFireworks[i].location + dist; ++loc)
  //   leds[loc] = CHSV(ActiveFireworks[i].color, 255, 255);
  leds[ActiveFireworks[i].location + dist] = CHSV(ActiveFireworks[i].color, 255, 255);
  leds[ActiveFireworks[i].location - dist] = CHSV(ActiveFireworks[i].color, 255, 255);
  ActiveFireworks[i].state--;
}

void setup() {

  for (uint8_t i = 0; i < MAX_FIREWORKS; ++i)
  {
    ActiveFireworks[i].state = 0; 
  }

  delay(200);

  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);

  random16_set_seed(analogRead(0));
  // bg_index = random8();

  fireworks_update.reset();
  new_fireworks_update.reset();
  brightness_update.reset();
  bg_update.reset();

  fill_solid(leds, NUM_LEDS, background_color);
  FastLED.show();

}

void loop() {
  if (bg_update.check() == 1)
  {
    background_color.h++;
  }

  if (brightness_update.check() == 1)
  {
    for(int i = 0; i < NUM_LEDS; i++) { 
      //leds[i].fadeToBlackBy(fade_rate); 
      leds[i] = blend(leds[i], CRGB(background_color), fade_rate);
    }
  }

  if (new_fireworks_update.check() == 1) {
    add_firework();
    new_fireworks_update.interval(random16() % 1500);
    fireworks_update.interval((random8() % 100) + 10);
    fade_rate = (random8() % 10) + 5;
    new_fireworks_update.reset();
  }

  if (fireworks_update.check())
  {
    for (uint8_t i = 0; i < MAX_FIREWORKS; ++i) {
      if (ActiveFireworks[i].state != 0)
      {
        update_firework(i);
      }
    }
  }

  FastLED.show();
}
