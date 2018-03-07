#pragma once

#include "NendoEffect.h"
#include "Metro.h"

#define MAX_FIREWORKS 10

struct FireWork {
  uint8_t location;
  uint8_t color;
  uint8_t width;
  uint8_t state;
};

class FireworksEffect : public NendoEffect {
public:
  FireworksEffect(
    CRGB* leds, const uint8_t num_leds);

  virtual int GetUid() override;
  virtual bool Update() override;

protected:
  FireWork ActiveFireworks[MAX_FIREWORKS];
  uint8_t fade_rate = 10;

  Metro fireworks_update;
  Metro new_fireworks_update;
  Metro brightness_update;
  Metro bg_update;

  CHSV background_color = CHSV(0, 128, 15);

  void add_firework();
  void update_firework(uint8_t i);
};