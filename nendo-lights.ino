
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

uint8_t brightness = 64;
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

int8_t bg_index;

int offset_index = 0;
int offset_width = 30 * highlight_aa;
int offset_scale = 90;

Metro highlight_update = Metro(33);
Metro bg_update = Metro(1234);
Metro offset_update = Metro(64);
Metro brightness_update = Metro(16);

void setup() {
  pinMode(POWER_BUTTON, INPUT_PULLUP);
  pinMode(BRIGHTNESS_MINUS, INPUT_PULLUP);
  pinMode(BRIGHTNESS_PLUS, INPUT_PULLUP);
  pinMode(SATURATION_MINUS, INPUT_PULLUP);
  pinMode(SATURATION_PLUS, INPUT_PULLUP);

  analogWrite(INDICATOR_LIGHT_GREEN, 255);
  attachInterrupt(POWER_BUTTON, power_switch, FALLING);

  if (EEPROM.read(0) == EEPROM_MAGICVALUE)
  {
    EEPROM.get(EEPROM_BRIGHTNESS_LOC, brightness);
    EEPROM.get(EEPROM_SATURATION_LOC, saturation);
  }

  delay(2000);

  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);

  random16_set_seed(analogRead(0));
  bg_index = random8();

  highlight_update.reset();
  bg_update.reset();
  offset_update.reset();
  brightness_update.reset();

}

void loop() {
  if (brightness_update.check() == 1)
  {
    if (indicator_state_red > 0)
    {
      --indicator_state_red;
    }
    if (indicator_state_green > 0)
    {
      --indicator_state_green;
    }
    analogWrite(INDICATOR_LIGHT_RED, ease8InOutApprox(indicator_state_red));
    analogWrite(INDICATOR_LIGHT_GREEN, ease8InOutApprox(indicator_state_green));
  
    if (!digitalRead(BRIGHTNESS_PLUS))
    {
      indicator_state_green = 255;
      if (brightness < 255)
      {
        ++brightness;
      }
      else
      {
        error_blink = 1 - error_blink;
        indicator_state_green *= error_blink;
      }
    }
    if (!digitalRead(BRIGHTNESS_MINUS))
    {
      indicator_state_red = 255;
      if (brightness > 1)
      {
        --brightness;
      }
      else
      {
        error_blink = 1 - error_blink;
        indicator_state_red *= error_blink;
      }
    }

    if (!digitalRead(SATURATION_PLUS))
    {
      indicator_state_green = 255;
      if (saturation < 255)
      {
        ++saturation;
      }
      else
      {
        error_blink = 1 - error_blink;
        indicator_state_green *= error_blink;
      }
    }
    if (!digitalRead(SATURATION_MINUS))
    {
      indicator_state_red = 255;
      if (saturation > 0)
      {
        --saturation;
      }
      else
      {
        error_blink = 1 - error_blink;
        indicator_state_red *= error_blink;
      }
    }

  }

  if (power == 0)
  {
    delay(100);
    return;
  }

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
      bg_index + scale8(offset_scale, offset_dist), 
      saturation, 
      scale8(brightness, 128 + (saturation / 2)));

    nblend(leds[i], CHSV(0, 0, brightness), 
      /*ease8InOutApprox*/(255 - 255 * highlight_dist / highlight_width));
  }

  FastLED.show();
}

void power_switch() {
  power = 1 - power;
  if (power == 1)
  {
    indicator_state_green = 255;
    indicator_state_red = 0;
  } 
  else
  {
    indicator_state_green = 0;
    indicator_state_red = 255;

    EEPROM.update(0, EEPROM_MAGICVALUE);
    EEPROM.update(EEPROM_BRIGHTNESS_LOC, brightness);
    EEPROM.update(EEPROM_SATURATION_LOC, saturation);

    FastLED.clear();
    FastLED.show();
  }
}