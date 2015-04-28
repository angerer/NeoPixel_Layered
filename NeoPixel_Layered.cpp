#include "NeoPixel_Layered.h"

// Put together strategy for applying arbitrary layers. Have the neoPixel parent class own a set of layers, in it's update method,
// calculate the layer then call a method to have it update the ring/strip with the layer information.

RGBaColor::RGBaColor() {
  hasColorValue=0;
  red=0;
  blue=0;
  green=0;
  alpha=0;
}

void RGBaColor::setColorValues(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  hasColorValue=1;
  red=r;
  green=g;
  blue=b;
  alpha=a;
}

NeoLayer::NeoLayer(uint16_t numberOfPixels) {
  numPixels = numberOfPixels;
}

// Default implementation of this simply indicates no color value.
void NeoLayer::setPixelColor(uint16_t pixelIndex, RGBaColor *pixelColor) {
  // Default implementation simply returns a no color data object
  pixelColor->hasColorValue=0;
}

// Utility Methods

/** Uses linear interpolation to calculate the next value in a fade between two color components */
uint8_t NeoLayer::interpolateNextFadeValue(uint8_t startingValue, uint8_t targetValue, uint16_t currentStep, uint16_t transitionSteps) {
	if(startingValue == targetValue) {
		// No transition to calculate between the values
		return startingValue;
	}
	
#ifdef DEBUG_LAYERS
	// uint16_t step1 = (transitionSteps - currentStep);
	// uint16_t step2 = (uint16_t)startingValue * step1;
	// uint16_t step3 = (uint16_t)targetValue * currentStep;
	// uint16_t step4 = step2 + step3;
	// uint16_t step5 = step4 / transitionSteps;
	//
	// Serial.print("Current index: ");
	// Serial.print(currentStep);
	//   Serial.print(" step1: ");
	//   Serial.print(step1);
	//   Serial.print(" step2: ");
	//   Serial.print(step2);
	//   Serial.print(" step3: ");
	//   Serial.print(step3);
	//   Serial.print(" step4: ");
	//   Serial.print(step4);
	//   Serial.print(" step5: ");
	//   Serial.print(step5);
#endif
	
	uint16_t workingValue = (((uint16_t)startingValue * (transitionSteps - currentStep)) + ((uint16_t)targetValue * currentStep)) / transitionSteps;

#ifdef DEBUG_LAYERS
  // Serial.print(" startingValue: ");
  // Serial.print(startingValue);
  // Serial.print(", targetValue:");
  // Serial.print(targetValue);
  // Serial.print(", currentStep:");
  // Serial.print(currentStep);
  // Serial.print(", transitionSteps:");
  // Serial.print(transitionSteps);
  // Serial.print(", calculatedValue:");
  // Serial.print(workingValue);
  // Serial.print("\n");
#endif

  if(workingValue > 255) {
      return 255;
  } else {
      return (uint8_t) workingValue;
  }
}

uint8_t NeoLayer::applyBrightness(uint8_t startingColor, uint8_t brightness) {
  uint8_t workingColor = startingColor * brightness / 255;
  if(workingColor > 255) {
    return 255;
  } else {
    return workingColor;
  }
}

/**
 * Color and Alpha blending layer implementation for NeoPixel arrays.
 */
NeoPixel_Layered::NeoPixel_Layered(uint16_t pixels, uint8_t pin, uint8_t type, uint16_t interval)
:Adafruit_NeoPixel(pixels, pin, type)
{
    updateInterval = interval;
}

// Update the pattern
void NeoPixel_Layered::update()
{
    unsigned long currentTime = millis();
    if((currentTime - lastUpdate) >= updateInterval) // time to update
    {
       lastUpdate = currentTime;
       updateLayers();
       show();
    }
}

void NeoPixel_Layered::applyLayer(NeoLayer* nl) {
  for (uint16_t i = 0; i < numPixels(); i++)
  {
    RGBaColor *pixelColor = new RGBaColor();
    nl->setPixelColor(i, pixelColor);
    if(pixelColor->hasColorValue) {
/*
      Serial.print("PixelColor red:");
      Serial.print(pixelColor->red);
      Serial.print(", green:");
      Serial.print(pixelColor->green);
      Serial.print(", blue:");
      Serial.print(pixelColor->blue);
      Serial.print(", alpha:");
      Serial.print(pixelColor->alpha);
      Serial.print("\n");
*/
      if(pixelColor->alpha == 255) {
        setPixelColor(i, pixelColor->red, pixelColor->green, pixelColor->blue);
      } else {
        uint32_t currentColor = getPixelColor(i);
        
        uint8_t red = (pixelColor->alpha * (pixelColor->red - redComponent(currentColor))/255) + redComponent(currentColor);
        uint8_t green = (pixelColor->alpha * (pixelColor->green - greenComponent(currentColor))/255) + greenComponent(currentColor);
        uint8_t blue = (pixelColor->alpha * (pixelColor->blue - blueComponent(currentColor))/255) + blueComponent(currentColor);
        
        setPixelColor(i, red, green, blue);
      }
    }
    delete pixelColor;
  }
}

// Fill the dots one after the other with a color
void NeoPixel_Layered::colorWipe(uint8_t r, uint8_t g, uint8_t b) {
  for(uint16_t i=0; i<numPixels(); i++) {
    setPixelColor(i, Color(r, g, b));
    show();
  }
}

// Returns the Red component of a 32-bit color
uint8_t NeoPixel_Layered::redComponent(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

// Returns the Green component of a 32-bit color
uint8_t NeoPixel_Layered::greenComponent(uint32_t color)
{
    return (color >> 8) & 0xFF;
}

// Returns the Blue component of a 32-bit color
uint8_t NeoPixel_Layered::blueComponent(uint32_t color)
{
    return color & 0xFF;
}
