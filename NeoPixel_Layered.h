#ifndef NeoPixel_Layered_h
#define NeoPixel_Layered_h

#include <Adafruit_NeoPixel.h>


// Put together strategy for applying arbitrary layers. Have the neoPixel parent class own a set of layers, in it's update method,
// calculate the layer then call a method to have it update the ring/strip with the layer information.

class RGBaColor {
  public:
    uint8_t hasColorValue;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
    
    RGBaColor();

    void setColorValues(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};

class NeoLayer {
  public:
    NeoLayer(uint16_t numberOfPixels);
    
    // This will be called by the enclosing NeoPixel
    virtual void update() = 0;
    
    // Default implementation of this simply indicates no color value.
    virtual void setPixelColor(uint16_t pixelIndex, RGBaColor *pixelColor);
  protected:
    uint16_t numPixels = 0;
    
    // Uses linear interpolation to calculate the next value in a fade between two color components
    uint8_t interpolateNextFadeValue(uint8_t startingValue, uint8_t targetValue, uint16_t currentStep, uint16_t transitionSteps);
    
    uint8_t applyBrightness(uint8_t startingColor, uint8_t brightness);   
};


/**
 * Color and Alpha blending layer implementation for NeoPixel arrays.
 */
class NeoPixel_Layered : public Adafruit_NeoPixel {
public:
    NeoPixel_Layered(uint16_t pixels, uint8_t pin, uint8_t type, uint16_t interval);

    // Update the pattern
    void update();

    // Fill the dots one after the other with a color
    void colorWipe(uint8_t r, uint8_t g, uint8_t b);


  protected:
    unsigned long updateInterval;   // milliseconds between updates
    virtual void updateLayers() = 0;
    void applyLayer(NeoLayer* nl);

    //TODO: Maybe these just become color
    // Returns the Red component of a 32-bit color
    uint8_t redComponent(uint32_t color);
 
    // Returns the Green component of a 32-bit color
    uint8_t greenComponent(uint32_t color);
 
    // Returns the Blue component of a 32-bit color
    uint8_t blueComponent(uint32_t color);
    
  private:
    unsigned long lastUpdate; // last time that the array was updated
};

#endif