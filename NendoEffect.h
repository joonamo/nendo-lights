#pragma once

#include <inttypes.h>
#include "FastLED.h"


struct CRGB;

class NendoEffect {
public:
    //Setup stuff here, remember to call parent constructor
    NendoEffect(CRGB* leds, const uint8_t num_leds)
        : m_leds(leds), m_num_leds(num_leds) {};
    // Destructor will probably not be ran ever
    virtual ~NendoEffect() {};

    // Do update of the effect here. Return if changes were made
    virtual bool Update() { return false; };
    virtual int GetUid() { return 0; };
    // Do teardown here
    virtual void EndPlay() {};
protected:
    CRGB* m_leds;
    const uint8_t m_num_leds;
};