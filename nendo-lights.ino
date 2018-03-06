#include "FastLED.h"
#include "Metro.h"
#include <EEPROM.h>
#include <new>
#include <IRremote.h>

#include "NendoEffect.h"
#include "ColorSpinEffect.h"
#include "SingleColorEffect.h"

static const uint8_t num_leds = 60;
#define DATA_PIN 13
#define IR_RECV_PIN 12

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

uint8_t brightness = 32;
uint8_t saturation = 255;

CRGB leds[num_leds];

Metro debug_print_timer(1000);

// ir stuff
IRrecv irrecv(IR_RECV_PIN);
decode_results ir_results;

struct EffectContainer {
  EffectContainer() { 
    m_current_effect_ptr = new(&m_current_effect) NendoEffect(leds, num_leds); 
  };
  ~EffectContainer() { m_current_effect.EndPlay(); };

  uint8_t m_current_effect_idx = -1;
  NendoEffect* m_current_effect_ptr = nullptr;
  union {
    NendoEffect m_current_effect;
    ColorSpinEffect m_colorspin;
    SingleColorEffect m_singlecolor;
  };
};
EffectContainer e;

void change_effect(int idx)
{
  e.m_current_effect.EndPlay();
  switch (idx)
  {
    case 0:
    {
      new(&e.m_colorspin) ColorSpinEffect(leds, num_leds, 33, 1234, 64, millis());
      break;
    }
    case 1:
    {
      new(&e.m_singlecolor) SingleColorEffect(leds, num_leds, CRGB::White);
      break;
    }
    case 2:
    {
      new(&e.m_singlecolor) SingleColorEffect(leds, num_leds, CRGB::DeepPink);
      break;
    }
    case 3:
    {
      new(&e.m_singlecolor) SingleColorEffect(leds, num_leds, CHSV(millis(), 255, 255));
      break;
    }
    case 4:
    {
      new(&e.m_singlecolor) SingleColorEffect(leds, num_leds, CRGB::White);
      break;
    }
    default:
    break;
  }
  e.m_current_effect_idx = idx;
}

void setup() {
  // pinMode(POWER_BUTTON, INPUT_PULLUP);
  pinMode(BRIGHTNESS_MINUS, INPUT_PULLUP);
  pinMode(BRIGHTNESS_PLUS, INPUT_PULLUP);
  pinMode(SATURATION_MINUS, INPUT_PULLUP);
  pinMode(SATURATION_PLUS, INPUT_PULLUP);

  analogWrite(INDICATOR_LIGHT_GREEN, 255);
  //attachInterrupt(POWER_BUTTON, power_switch, FALLING);

  if (EEPROM.read(0) == EEPROM_MAGICVALUE)
  {
    EEPROM.get(EEPROM_BRIGHTNESS_LOC, brightness);
    EEPROM.get(EEPROM_SATURATION_LOC, saturation);
  }

  delay(2000);

  irrecv.enableIRIn();

  FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, num_leds);
  FastLED.setBrightness(255);

  random16_set_seed(analogRead(0));
  change_effect(0);
  // bg_index = random8();

  fill_solid(leds, num_leds, CRGB::White);
  FastLED.show();

  Serial.begin(9600);
}

void loop() {
  // if (brightness_update.check() == 1)
  // {
  //   update_control_panel();
  // }

  // if (power == 0)
  // {
  //   delay(100);
  //   return;
  // }
  if (irrecv.decode(&ir_results)) {
    if (ir_results.value < 0xFFFFFFFF)
    {
      switch (ir_results.value)
      {
        case 0x1FE40BF:
        change_effect(0);
        break;

        case 0x1FE609F:
        change_effect(1);
        break;

        case 0x1FE10EF:
        change_effect(2);
        break;

        case 0x1FE50AF:
        change_effect(3);
        break;

        case 0x1FEB04F:
        change_effect(4);
        break;

        default:
        break;
      }
    }
    irrecv.resume();
  }

  if (debug_print_timer.check())
  {
    Serial.println(e.m_current_effect_ptr->GetUid());
  }

  if (e.m_current_effect_ptr->Update()) // gotta use the vtable here
  {
    FastLED.show();
  }
}

// void power_switch() {
//   power = 1 - power;
//   if (power == 1)
//   {
//     indicator_state_green = 255;
//     indicator_state_red = 0;
//   } 
//   else
//   {
//     indicator_state_green = 0;
//     indicator_state_red = 255;

//     EEPROM.update(0, EEPROM_MAGICVALUE);
//     EEPROM.update(EEPROM_BRIGHTNESS_LOC, brightness);
//     EEPROM.update(EEPROM_SATURATION_LOC, saturation);

//     FastLED.clear();
//     FastLED.show();
//   }
// }

// void update_control_panel()
// {
//   if (indicator_state_red > 0)
//     {
//       --indicator_state_red;
//     }
//     if (indicator_state_green > 0)
//     {
//       --indicator_state_green;
//     }
//     analogWrite(INDICATOR_LIGHT_RED, ease8InOutApprox(indicator_state_red));
//     analogWrite(INDICATOR_LIGHT_GREEN, ease8InOutApprox(indicator_state_green));
  
//     if (!digitalRead(BRIGHTNESS_PLUS))
//     {
//       indicator_state_green = 255;
//       if (brightness < 255)
//       {
//         ++brightness;
//         FastLED.setBrightness(brightness);
//       }
//       else
//       {
//         error_blink = 1 - error_blink;
//         indicator_state_green *= error_blink;
//       }
//     }
//     if (!digitalRead(BRIGHTNESS_MINUS))
//     {
//       indicator_state_red = 255;
//       if (brightness > 1)
//       {
//         --brightness;
//         FastLED.setBrightness(brightness);
//       }
//       else
//       {
//         error_blink = 1 - error_blink;
//         indicator_state_red *= error_blink;
//       }
//     }

//     if (!digitalRead(SATURATION_PLUS))
//     {
//       if (saturation < 255)
//       {
//         ++saturation;
//       }
//       else
//       {
//         error_blink = 1 - error_blink;
//         indicator_state_green *= error_blink;
//       }
//     }
//     if (!digitalRead(SATURATION_MINUS))
//     {
//       if (saturation > 0)
//       {
//         --saturation;
//       }
//       else
//       {
//         error_blink = 1 - error_blink;
//         indicator_state_red *= error_blink;
//       }
//     }

// }
