#include "SingleColorEffect.h"
#include "FastLED.h"

SingleColorEffect::SingleColorEffect(
  CRGB* leds, const uint8_t num_leds, 
  const CRGB& TargetColor)
  : NendoEffect(leds, num_leds) 
{
  fill_solid(leds, num_leds, TargetColor);
}

int SingleColorEffect::GetUid()
{
  return 255;
}

bool SingleColorEffect::Update() 
{
  if (m_first_update)
  {
    m_first_update = false;
    return true;
  }
  else
  {
    delay(100);
    return false;
  }
}