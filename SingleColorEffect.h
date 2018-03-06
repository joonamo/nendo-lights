#pragma once

#include "NendoEffect.h"

class SingleColorEffect : public NendoEffect {
public:
  SingleColorEffect(
    CRGB* leds, const uint8_t num_leds, 
    const CRGB& TargetColor);

  virtual int GetUid() override;
  virtual bool Update() override;

protected:
  bool m_first_update = true;
};