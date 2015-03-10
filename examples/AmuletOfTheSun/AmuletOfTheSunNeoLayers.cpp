#include "AmuletOfTheSunNeoLayers.h"

// 
// Background Magic implementation
// 
NeoLayer_BackgroundMagic::NeoLayer_BackgroundMagic(uint16_t numPixels, uint8_t targetRed1, uint8_t targetGreen1, uint8_t targetBlue1, uint8_t targetRed2, uint8_t targetGreen2, uint8_t targetBlue2, uint16_t colorFadePeriod, uint8_t minBrightness, uint8_t maxBrightness, uint16_t brightnessPulsePeriod, uint16_t updateInterval) 
  : NeoLayer(numPixels) {
            
		// initialize color and brightness
		startingRed = targetRed1;
		startingGreen = targetGreen1;
		startingBlue = targetBlue1;

		targetRed = targetRed2;
		targetGreen = targetGreen2;
		targetBlue = targetBlue2;

		startingBrightness = minBrightness;
		targetBrightness = maxBrightness;

		// Calculate total steps for color fade (one direction)
		totalColorSteps = colorFadePeriod / updateInterval / 2;

		// Calculate total steps for brightness (one direction)
		totalBrightnessSteps = brightnessPulsePeriod / updateInterval / 2;
	}

void NeoLayer_BackgroundMagic::incrementColorIndex() {
  colorStepIndex++;

  // If we've reached the target color, swap them to cycle
  if(colorStepIndex >= totalColorSteps) {
    uint8_t tempRed = startingRed;
    uint8_t tempGreen = startingGreen;
    uint8_t tempBlue = startingBlue;

		startingRed = targetRed;
		startingGreen = targetGreen;
		startingBlue = targetBlue;
		
    targetRed = tempRed;
		targetGreen = tempGreen;
		targetBlue = tempBlue;
		
		// Reset the index so we can count up again
    colorStepIndex = 0;
  }
}
  
void NeoLayer_BackgroundMagic::incrementBrightnessIndex() {
  brightnessStepIndex++;

  // If we've reached the target brightness, swap them to cycle
  if(brightnessStepIndex >= totalBrightnessSteps) {
      uint8_t tempBrightness = startingBrightness;
      startingBrightness = targetBrightness;
      targetBrightness = tempBrightness;
    
      brightnessStepIndex = 0;
  }
}
  
void NeoLayer_BackgroundMagic::update() {
  // Calculate linear interpolation for the next brightness value
  uint8_t brightness = interpolateNextFadeValue(startingBrightness, targetBrightness, brightnessStepIndex, totalBrightnessSteps);

  // Calculate linear interpolation between Color1 and Color2
  // Optimise order of operations to minimize truncation error
  currentRed = applyBrightness(interpolateNextFadeValue(startingRed, targetRed, colorStepIndex, totalColorSteps), brightness);
  currentGreen = applyBrightness(interpolateNextFadeValue(startingGreen, targetGreen, colorStepIndex, totalColorSteps), brightness);
  currentBlue = applyBrightness(interpolateNextFadeValue(startingBlue, targetBlue, colorStepIndex, totalColorSteps), brightness);
  
  // Increment color index
  incrementColorIndex();
  incrementBrightnessIndex();
  
  /*
  Serial.print("colorStepIndex: ");
  Serial.print(colorStepIndex);
  Serial.print(", currentRed:");
  Serial.print(currentRed);
  Serial.print(", currentGreen:");
  Serial.print(currentGreen);
  Serial.print(", currentBlue:");
  Serial.print(currentBlue);
  Serial.print(", brightness:");
  Serial.print(brightness);
  Serial.print("\n");
  */
}

void NeoLayer_BackgroundMagic::setPixelColor(uint16_t pixelIndex, RGBaColor *pixelColor) {
  // Default implementation simply returns a no color data object
  pixelColor->setColorValues(currentRed, currentGreen, currentBlue, 255);
}


/*
  Magic Flare - Greenish White:
  One random pixel flares brightly (1/10 of flare interval) and fades slowly back in line.
  The duration is over an irregular interval between MaxDuration and 50% * MaxDuration (5 seconds?)
  This initiates on an irregular interval of between MaxInterval and 40% of MaxInterval (15 seconds?)
*/
NeoLayer_MagicFlare::NeoLayer_MagicFlare(uint16_t numberOfPixels, uint8_t targetRed, uint8_t targetGreen, uint8_t targetBlue, uint16_t duration, uint16_t period, float flareFadeRatio, uint16_t interval)
:NeoLayer(numberOfPixels) {
  flarePixelTargetRed = targetRed;
  flarePixelTargetGreen = targetGreen;
  flarePixelTargetBlue = targetBlue;
  
  maxPeriod = period;
  maxDuration = duration;
  
  flareRatio = flareFadeRatio;
  
  updateInterval = interval;
}


void NeoLayer_MagicFlare::update() {

  // Are we flaring?
  if(flaring) {
     // Determine if we are Flaring up or Fading
    uint16_t flareUpSteps = (uint16_t) (totalStepsInCurrentFlare * flareRatio);
    
#ifdef DEBUG
    Serial.print("Flare Color: ");
    Serial.print(flarePixelTargetRed + flarePixelTargetGreen + flarePixelTargetBlue);
    Serial.print(", flareUpSteps: ");
    Serial.print(flareUpSteps);
    Serial.print(", flareIndex: ");
    Serial.print(flareIndex);
    Serial.print(", currentFlareOpacity: ");
    Serial.print(currentFlareOpacity);
    Serial.print("\n");
#endif
  
    if(flareIndex < flareUpSteps) {
      // Flaring - Increase opacity of the flare pixel
      currentFlareOpacity = interpolateNextFadeValue(0, 255, flareIndex, flareUpSteps);
    } else {
      // Fading - Decrease opacity of the flare pixel
      currentFlareOpacity = interpolateNextFadeValue(255, 0, flareIndex - flareUpSteps, totalStepsInCurrentFlare - flareUpSteps);
    }

    // Increment FlareIndex
    flareIndex++;

    // Flare completed
    if(flareIndex >= totalStepsInCurrentFlare) {
      // Indicate that there is no more flare happening
      flaring = 0;
      currentFlareOpacity=0;
    }

  // Not currently flaring
  } else {
    // Time to start a flare
    if(nextFlareStart <= millis()) {
			
      // Start the flare
      flaring = 1;
      
      // Choose a pixel to flare
      pixelIndexForCurrentFlare = random(numPixels -1);
    
      // Generate a duration
      uint16_t totalFlareDuration = random(maxDuration/3, maxDuration);
      
      // Calculate number of steps
      totalStepsInCurrentFlare = totalFlareDuration / updateInterval;
      
      // Reset FlareIndex and FadeIndex
      flareIndex = 0;
      
      // generate nextFlareStart time
      nextFlareStart = millis() + random(maxPeriod*2/3, maxPeriod);

#ifdef DEBUG
      Serial.print("Starting Flare: ");
      Serial.print(nextFlareStart);
      Serial.print(", current millis: ");
      Serial.print(millis());
      Serial.print(", duration: ");
      Serial.print(totalFlareDuration);
      Serial.print(", total steps in flare: ");
      Serial.print(totalStepsInCurrentFlare);
      Serial.print(", flare pixel: ");
      Serial.print(pixelIndexForCurrentFlare);
      Serial.print(", next flare: ");
      Serial.print(nextFlareStart);
      Serial.print("\n");
#endif
		
    }
  }
}

void NeoLayer_MagicFlare::setPixelColor(uint16_t pixelIndex, RGBaColor *pixelColor) {
  if(pixelIndexForCurrentFlare == pixelIndex && currentFlareOpacity > 0) {
    pixelColor->setColorValues(flarePixelTargetRed, flarePixelTargetGreen, flarePixelTargetBlue, currentFlareOpacity);

#ifdef DEBUG
    Serial.print("currentRed:");
    Serial.print(pixelColor->red);
    Serial.print(", currentGreen:");
    Serial.print(pixelColor->green);
    Serial.print(", currentBlue:");
    Serial.print(pixelColor->blue);
    Serial.print(", alpha:");
    Serial.print(pixelColor->alpha);
    Serial.print("\n");
#endif

  } else {
      pixelColor->hasColorValue=0;
  }
}


AmuletOfTheSun::AmuletOfTheSun(uint16_t pixels, uint8_t pin, uint8_t type, uint16_t interval)
:NeoPixel_Layered(pixels, pin, type, interval) {
	backgroundLayer = new NeoLayer_BackgroundMagic(pixels, 255, 0, 0, 255, 180, 0, 8000, 16, 48, 6000, interval);
  pinkFlare = new NeoLayer_MagicFlare(pixels, 128, 40, 80, 4000, 30000, (float) 0.15, interval);
  greenFlare = new NeoLayer_MagicFlare(pixels, 10, 180, 38, 3000, 20000, (float) 0.2, interval);
  blueFlare = new NeoLayer_MagicFlare(pixels, 10, 80, 200, 6000, 25000, (float) 0.1, interval);
}

void AmuletOfTheSun::updateLayers() {
  backgroundLayer->update();
  applyLayer(backgroundLayer);
  pinkFlare->update();
  applyLayer(pinkFlare);
  greenFlare->update();
  applyLayer(greenFlare);
  blueFlare->update();
  applyLayer(blueFlare);
}
