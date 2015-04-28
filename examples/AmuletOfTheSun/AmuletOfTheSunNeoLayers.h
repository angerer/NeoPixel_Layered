#ifndef AmuletOfTheSunNeoLayers_h
#define AmuletOfTheSunNeoLayers_h

#include <Adafruit_NeoPixel.h>
#include <NeoPixel_Layered.h>

/* 
 * This Layer creates a pulsing effect across the entire NeoPixel array. The pulse is both in brightness
 * and in a fade between two colors.
 * Background pulses - 3 second cycle
 * Background shifts colors smoothly (irregularly) from red to yellow and back - 10 second cycle
 */
class NeoLayer_BackgroundMagic : public NeoLayer {

private: 
  uint8_t startingRed;
  uint8_t startingGreen;
  uint8_t startingBlue;

  uint8_t targetRed; // rgb color
  uint8_t targetGreen; // rgb color
  uint8_t targetBlue; // rgb color
		
  uint8_t startingBrightness; // 0-255
  uint8_t targetBrightness; // 0-255
  
  uint16_t totalColorSteps;
  uint16_t totalBrightnessSteps;
  
  uint16_t colorStepIndex;
  uint16_t brightnessStepIndex;
  
  uint8_t currentRed;
  uint8_t currentGreen;
  uint8_t currentBlue;
    
public:
  NeoLayer_BackgroundMagic(uint16_t numPixels, uint8_t targetRed1, uint8_t targetGreen1, uint8_t targetBlue1, uint8_t targetRed2, uint8_t targetGreen2, uint8_t targetBlue2, uint16_t colorFadePeriod, uint8_t minBrightness, uint8_t maxBrightness, uint16_t brightnessPulsePeriod, uint16_t updateInterval);

  void update();

  void setPixelColor(uint16_t pixelIndex, RGBaColor *pixelColor);

private:
  void incrementColorIndex();
    
  void incrementBrightnessIndex();
    
};

/*
  Magic Flare - Greenish White:
  One random pixel flares brightly (1/10 of flare interval) and fades slowly back in line.
  The duration is over an irregular interval between MaxDuration and 50% * MaxDuration (5 seconds?)
  This initiates on an irregular interval of between MaxInterval and 40% of MaxInterval (15 seconds?)
*/
class NeoLayer_MagicFlare : public NeoLayer {

private:
  // Static Data
  uint8_t flarePixelTargetRed; // Flare color
  uint8_t flarePixelTargetGreen; // Flare color
  uint8_t flarePixelTargetBlue; // Flare color
  uint16_t maxPeriod; // time between flares
  uint16_t maxDuration;
  uint8_t flareRatio;
  uint16_t updateInterval;

  // Changes once per flare
  uint16_t pixelIndexForCurrentFlare; // Position of the pixel that is flaring
  uint16_t totalStepsInCurrentFlare; // Number of transition steps required for the current flare
  unsigned long nextFlareStart = random(4000, 15000); // millis() value when flare should next initiate
  uint8_t flaring = 0;
  
  // Working Data - changes throughout the flaring process
  uint16_t flareIndex; // current step through the flare transition
  uint8_t currentFlareOpacity = 0;

public:
  NeoLayer_MagicFlare(uint16_t numberOfPixels, uint8_t targetRed, uint8_t targetGreen, uint8_t targetBlue, uint16_t duration, uint16_t period, uint8_t flareFadeRatio, uint16_t interval);

  void update();
  void setPixelColor(uint16_t pixelIndex, RGBaColor *pixelColor);
};


/**
 * This is the class that has the specifics of how the amulet LEDs will be configured to run
 * and the interplay of layers.
 */
class AmuletOfTheSun : public NeoPixel_Layered {

public:
  AmuletOfTheSun(uint16_t pixels, uint8_t pin, uint8_t type, uint16_t interval);
    
	void updateLayers();
    
private:
  NeoLayer_BackgroundMagic* backgroundLayer;
  NeoLayer_MagicFlare* pinkFlare;
  NeoLayer_MagicFlare* greenFlare;
  NeoLayer_MagicFlare* blueFlare;
};

#endif
