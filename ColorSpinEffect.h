#pragma once

#include "NendoEffect.h"
#include "Metro.h"

class ColorSpinEffect : public NendoEffect {
public:
  ColorSpinEffect(
    CRGB* leds, const uint8_t num_leds, 
    unsigned long highlight_delay,
    unsigned long bg_delay, 
    unsigned long offset_delay,
    uint8_t bg_index);

  virtual ~ColorSpinEffect() {};

  virtual int GetUid() override;
  virtual bool Update() override;
protected:
  Metro m_highlight_update;
  Metro m_bg_update;
  Metro m_offset_update;

  static const uint8_t m_highlight_aa = 10;
  int m_highlight_width; // = 10 * highlight_aa;

  int m_highlight_index = 0;
  CRGB m_base_color;

  uint8_t m_bg_index = 0;
  int8_t m_bg_dir = 1;

  static const uint8_t m_bg_max = 120;
  static const uint8_t m_bg_offset = 120; 

  int m_offset_index = 0;
  int m_offset_width; // = 10 * highlight_aa;
  int m_offset_scale = 20;
  int m_offset_dir = 1;
};