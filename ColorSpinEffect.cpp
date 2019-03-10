#include "ColorSpinEffect.h"
#include "FastLED.h"

#define SATURATION 255
#define BRIGHTNESS 255

ColorSpinEffect::ColorSpinEffect(CRGB* leds, const uint8_t num_leds, 
  unsigned long highlight_delay, 
  unsigned long bg_delay, 
  unsigned long offset_delay,
  uint8_t bg_index)
  : NendoEffect(leds, num_leds) 
  , m_highlight_update(highlight_delay, 1)
  , m_bg_update(bg_delay, 1)
  , m_offset_update(offset_delay, 1)
  , m_bg_index(bg_index)
{
  m_highlight_width = 10 * m_highlight_aa;
  m_offset_width = 10 * m_highlight_aa;
} 

int ColorSpinEffect::GetUid()
{
  return 39;
}

bool ColorSpinEffect::Update() 
{
  bool something_changed = false;
  if (m_bg_update.check() == 1)
  {
    if (m_bg_index >= m_bg_max)
      m_bg_dir = -1;
    else if (m_bg_index <= 0)
      m_bg_dir = 1;
    m_bg_index += m_bg_dir;
    something_changed = true;
  }
  if (m_highlight_update.check() == 1)
  {
    m_highlight_index = (m_highlight_index + 1) % (m_num_leds * m_highlight_aa);
  }
  if (m_offset_update.check() == 1)
  {
    --m_offset_index;
    if (m_offset_index < 0)
      m_offset_index = m_num_leds * m_highlight_aa;

    if (m_offset_scale >= m_bg_max * 2)
      m_offset_dir = -1;
    else if (m_offset_scale <= 0)
      m_offset_dir = 1;
    m_offset_scale += m_offset_dir;
    something_changed = true;
  }

  // fill_solid(leds, m_num_leds, );
  if (something_changed)
  {
    for (int i = 0; i < m_num_leds; i += 1)
    {
      int highlight_dist = min(min(
        abs((m_highlight_index) - ((i - m_num_leds) * m_highlight_aa)),
        abs((m_highlight_index) - ((i + m_num_leds) * m_highlight_aa))),
        abs((m_highlight_index) - (i * m_highlight_aa)));
      
      if (highlight_dist > m_highlight_width)
        highlight_dist = m_highlight_width;

      int offset_dist = min(min(
        abs((m_offset_index) - ((i - m_num_leds) * m_highlight_aa)),
        abs((m_offset_index) - ((i + m_num_leds) * m_highlight_aa))),
        abs((m_offset_index) - (i * m_highlight_aa)));
      if (offset_dist > m_offset_width)
        offset_dist = m_offset_width;

      offset_dist = /*ease8InOutApprox*/(255 - 255 * offset_dist / m_offset_width);
      m_leds[i] = CHSV(
        m_bg_index + m_bg_offset + ((100 - m_offset_scale) * offset_dist / 255), 
        SATURATION, BRIGHTNESS);

      nblend(m_leds[i], CHSV(0, 0, 255), 
        (255 - 255 * highlight_dist / m_highlight_width));
    }
  }

  return something_changed;
}