#include <FastLED.h>
#include <Metro.h>

#define LED_PIN     17
#define LED_PIN2    5
#define NUM_LEDS    30
#define BRIGHTNESS  128
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB background[NUM_LEDS];
CRGB background2[NUM_LEDS];
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];
CRGB highlight_color;

#define SATURATION 255
#define HIGHLIGHT_SATURATION 255
#define OFFSET 30
#define UPDATES_PER_SECOND 60

CRGBPalette16 leftPalette;
CRGBPalette16 rightPalette;
// CRGBPalette16 currentPalette;
TBlendType currentBlending;


uint8_t colorIndexLeft = random8();
uint8_t colorIndexRight = random8();

uint8_t colorIndexLeft2 = random8();
uint8_t colorIndexRight2 = random8();

uint8_t sindex = 0;
uint8_t highlight_index = random8();

Metro bg_update = Metro(5000);
Metro sin_update = Metro(333);

void setup() {
  currentBlending = LINEARBLEND;
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  // fill_gradient(background, NUM_LEDS, CHSV(colorIndexLeft, SATURATION, 255), CHSV(colorIndexLeft + OFFSET * 2, SATURATION, 255));
  // fill_gradient(background2, NUM_LEDS, CHSV(colorIndexLeft + OFFSET, SATURATION, 255), CHSV(colorIndexLeft + OFFSET * 3, SATURATION, 255));

  random16_set_seed(analogRead(0) + analogRead(1) + analogRead(3));
  colorIndexLeft = random8();

  memcpy(leds, background, sizeof(CRGB) * NUM_LEDS);
  memcpy(leds2, background2, sizeof(CRGB) * NUM_LEDS);

  highlight_color = CHSV(highlight_index, HIGHLIGHT_SATURATION, 255);

  FastLED.show();

  return;

  // fill_solid(leds, NUM_LEDS, neutralColor);
  // sourcePos = random8() % NUM_LEDS;
  // sourceColor = CHSV( random8(), 255, random8());
  // fill_solid(neutralStrip, NUM_LEDS, neutralColor);
  // fill_solid(minusStrip, NUM_LEDS, CRGB::Black);
    
}

void loop()
{
  if (bg_update.check() == 1)
  {
    ++colorIndexLeft;
    fill_gradient(background, NUM_LEDS, CHSV(colorIndexLeft, SATURATION, 255), CHSV(colorIndexLeft + OFFSET * 2, SATURATION, 255));
    fill_gradient(background2, NUM_LEDS, CHSV(colorIndexLeft + OFFSET, SATURATION, 255), CHSV(colorIndexLeft + OFFSET * 3, SATURATION, 255));
    
    highlight_color = CHSV(colorIndexLeft + OFFSET, HIGHLIGHT_SATURATION, 255);
  }

  // if (highlight_update.check() == 1)
  // {
  //   highlight_color = CHSV(highlight_index++, HIGHLIGHT_SATURATION, 255);
  // }

  memcpy(leds, background, sizeof(CRGB) * NUM_LEDS);
  memcpy(leds2, background2, sizeof(CRGB) * NUM_LEDS);

  if (sin_update.check())
  {
    --sindex;
  }

  for (size_t i = 0; i < NUM_LEDS; i += 1)
  {
    nblend(leds[i], /*highlight_color*/ CRGB::White, sin8(sindex + i * 4) / 2 + 8);
    nblend(leds2[i], /*highlight_color*/ CRGB::White, sin8(sindex + (i * 4 + NUM_LEDS / 2)) / 4);
  }
  FastLED.show();
  //FastLED.delay(1000/UPDATES_PER_SECOND);
}

void draw_bubble(CRGB* targetleds, int center, int width, uint8_t phase)
{
  width = sin8(phase) * width / 255 * 2;
  int end = min(center + width / 2, NUM_LEDS);
  int start = max(0, center - width / 2);
  int phasecounter = start;
  for (int i = start; i < end; ++i)
  {
    targetleds[i] = nblend(targetleds[i], CRGB::Red, 255 - cos8(phasecounter * 255 / width));
    ++phasecounter;
  }
}
