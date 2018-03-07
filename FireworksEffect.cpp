#include "FireworksEffect.h"
#include "FastLED.h"

FireworksEffect::FireworksEffect(CRGB* leds, const uint8_t num_leds)
  : NendoEffect(leds, num_leds)
  , fireworks_update(50)
  , new_fireworks_update(16)
  , brightness_update(16)
  , bg_update(100)
{
  for (uint8_t i = 0; i < MAX_FIREWORKS; ++i)
  {
    ActiveFireworks[i].state = 0; 
  }
}

int FireworksEffect::GetUid() 
{
  return 2000;
}

void FireworksEffect::add_firework()
{
  for (uint8_t i = 0; i < MAX_FIREWORKS; ++i) {
    if (ActiveFireworks[i].state == 0) {
      uint8_t width = random8() % 6 + 1;
      ActiveFireworks[i].width = width;
      ActiveFireworks[i].location = width + random8() % (m_num_leds - width * 2);
      ActiveFireworks[i].color = random8();
      ActiveFireworks[i].state = ActiveFireworks[i].width;
      return;
    }
  }
}

void FireworksEffect::update_firework(uint8_t i)
{
  uint8_t dist = ActiveFireworks[i].width - ActiveFireworks[i].state;
  // for (uint8_t loc = ActiveFireworks[i].location - dist; loc <= ActiveFireworks[i].location + dist; ++loc)
  //   leds[loc] = CHSV(ActiveFireworks[i].color, 255, 255);
  m_leds[ActiveFireworks[i].location + dist] = CHSV(ActiveFireworks[i].color, 255, 255);
  m_leds[ActiveFireworks[i].location - dist] = CHSV(ActiveFireworks[i].color, 255, 255);
  ActiveFireworks[i].state--;
}

bool FireworksEffect::Update()
{
  bool something_changed = false;
  if (bg_update.check() == 1)
  {
    background_color.h++;
  }

  if (brightness_update.check() == 1)
  {
    for(int i = 0; i < m_num_leds; i++) { 
      //m_leds[i].fadeToBlackBy(fade_rate); 
      m_leds[i] = blend(m_leds[i], CRGB(background_color), fade_rate);
    }

    something_changed = true;
  }

  if (new_fireworks_update.check() == 1) {
    add_firework();
    new_fireworks_update.interval(random16() % 1500);
    fireworks_update.interval((random8() % 100) + 10);
    fade_rate = (random8() % 10) + 5;
    new_fireworks_update.reset();
    
    something_changed = true;
  }

  if (fireworks_update.check())
  {
    for (uint8_t i = 0; i < MAX_FIREWORKS; ++i) {
      if (ActiveFireworks[i].state != 0)
      {
        update_firework(i);
      }
    }
    something_changed = true;
  }

  return something_changed;
}