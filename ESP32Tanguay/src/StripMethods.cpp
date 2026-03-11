// AS in methods for the LED strip

#include <Arduino.h>

#include <FastLED.h>
using namespace std;

#define NUM_LEDS 1
inline CRGB onboardLED[1];
 
inline CRGB testStrip[10];

inline void ShowAll()
{

}

inline void SetBrightness(int i)
{

}

inline void BeginStrips(int brightness = 50)
{
    //FastLED.addLeds<WS2812B, 48, GRB>(onboardLED, 1);
    FastLED.addLeds<WS2812B, 13, GRB>(testStrip, 10);
}

inline void SetOnboardLED(CRGB color) {
    // onboardLED[0] = color;
    // FastLED.show();
    // delay(500);
}



inline void SetAllLED(CRGB color) {
    for (size_t i = 0; i < 10; i++)
    {
        testStrip[i] = color;
    }
    
    
    FastLED.show();
    delay(500);
}

inline CRGB Wheel(byte WheelPosition) {
  static byte c[3];
 
  if(WheelPosition < 85) {
   c[0] = WheelPosition * 3;
   c[1] = 255 - WheelPosition * 3;
   c[2] = 0;
  }
  else if(WheelPosition < 170) {
   WheelPosition -= 85;
   c[0] = 255 - WheelPosition * 3;
   c[1] = 0;
   c[2] = WheelPosition * 3;
  }
  else {
   WheelPosition -= 170;
   c[0] = 0;
   c[1] = WheelPosition * 3;
   c[2] = 255 - WheelPosition * 3;
  }

  
  return CRGB(c[0], c[1], c[2]);
}

inline void Rainbow() {
    for (size_t i = 0; i < 10; i++)
    {
    testStrip[i] = Wheel(millis() / 10);
    }
    FastLED.show();
}