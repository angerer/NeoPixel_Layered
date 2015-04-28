#include <Adafruit_NeoPixel.h>
#include <NeoPixel_Layered.h>
#include "AmuletOfTheSunNeoLayers.h"

/* These are the standard setup() and loop() functions to run the microcontroller */
AmuletOfTheSun amulet(16, 2, NEO_GRB + NEO_KHZ800, 30);
 
// Initialize everything and prepare to start
void setup()
{
//  Serial.begin(115200);
    
  // Initialize all the pixelStrips
  amulet.begin();
  amulet.colorWipe(5, 5, 5);
}
 
// Main loop
void loop()
{
    // Update the rings.
    amulet.update();
}
