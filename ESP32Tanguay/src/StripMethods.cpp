// AS in methods for the LED strip

#include <Arduino.h>

#include <FastLED.h>
using namespace std;

#define NUM_LEDS 1
inline CRGB onboardLED[1];
 

inline void ShowAll()
{

}

inline void SetBrightness(int i)
{

}

inline void BeginStrips(int brightness = 50)
{
    FastLED.addLeds<WS2812B, 48, GRB>(onboardLED, 1);
}

inline void SetOnboardLED(CRGB color) {
    onboardLED[0] = color;
    FastLED.show();
    delay(500);
}