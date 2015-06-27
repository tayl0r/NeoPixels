/****************************************
Example Sound Level Sketch for the 
Adafruit Microphone Amplifier
****************************************/

#include <Adafruit_NeoPixel.h>
#include <avr/power.h> // Comment out this line for non-AVR boards (Arduino Due, etc.)

#define PIN 6

const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;

const unsigned int maxColor = 255;
const double micLowThreshold = 50.0;
const double micStep = 130.0;
const unsigned int micStep1 = micLowThreshold + micStep;
const unsigned int micStep2 = micLowThreshold + micStep1 * 2;
const unsigned int micStep3 = micLowThreshold + micStep1 * 3;
const unsigned int micStep4 = micLowThreshold + micStep1 * 4;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(90, PIN, NEO_GRB + NEO_KHZ800);
 
void setup() 
{
  // for the mic
  Serial.begin(9600);

  // for the lights
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}
 
 
void loop() 
{
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int lvl = 0;   // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
 
   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   lvl = signalMax - signalMin;  // max - min = peak-peak amplitude
   double v = lvl;

   //double volts = (peakToPeak * 3.3) / 1024;  // convert to volts

   unsigned int red = 0;
   unsigned int green = 0;
   unsigned int blue = 0;
   if (v <= micLowThreshold) {
    blue = 1;
   }
   else if (v <= micStep1) {
    blue = ((v-micLowThreshold)/micStep) * maxColor;
   }
   else if (v <= micStep2) {
    blue = ((micStep2-v)/micStep) * maxColor;
    green = ((v-micStep1)/micStep) * maxColor;
   }
   else if (v <= micStep3) {
    blue = 0;
    green = ((micStep3-v)/micStep) * maxColor;
    red = ((v-micStep2)/micStep) * maxColor;
   } else if (v <= micStep4) {
    blue = 0;
    green = 0;
    red = ((micStep4-v)/micStep) * maxColor;
   } else {
    red = maxColor;
   }
   colorWipe(strip.Color(red, green, blue));
   
   Serial.println(v);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      //delay(wait);
  }
  strip.show();
}
