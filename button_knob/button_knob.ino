// button_knob
// this controls an arduino + neopixels with a button and a knob.
// the button cycles through the light modes.
// the knob adjusts the brightness.

#include <Adafruit_NeoPixel.h>
#include <avr/power.h> // Comment out this line for non-AVR boards (Arduino Due, etc.)

#define LED_PIN       0  // LEDs on Trinket Pin #0
#define POT_PIN       1  // Potentiometer sweep (center) on Trinket Pin #2 (Analog 1)
#define BUTTON_PIN    1  // button on Trinket Pin #1 (digital)

#define MODE_WHITE                  0
//#define MODE_SOLID_COLOR            1
#define MODE_DYNAMIC_COLOR          1
//#define MODE_RAINBOW                2
//#define MODE_THEATER_CHASE_RAINBOW  2
//#define MODE_RAINBOW_CYCLE          3
#define MODE_MAX                    2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(90, LED_PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

volatile int buttonState = 0;
int buttonStateLast = -1;
volatile int potValue = 50;
int potValueLast = -1;
int mode = 0;
byte c = 0;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pinMode(BUTTON_PIN, INPUT);
//  digitalWrite(BUTTON_PIN, HIGH);  // enable pullup resistor
//  attachInterrupt(0, pinChange, RISING); // 0 interrupt is for pin #2
  
  TIMSK |= _BV(OCIE0A);    // Turn on the compare interrupt (below!)


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

}

void loop() {
  //pinChange();

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState != buttonStateLast) {
    if (buttonState == HIGH) {
      mode++;
    }
  }

  if (mode % MODE_MAX == MODE_WHITE) {
    colorFill(strip.Color(CB(255), CB(255), CB(255)));
  }
//  else if (mode % MODE_MAX == MODE_RAINBOW) {
//    rainbow(20);
//  }
//  else if (mode % MODE_MAX == MODE_THEATER_CHASE_RAINBOW) {
//    theaterChaseRainbow(50);
//  }
//  else if (mode % MODE_MAX == MODE_RAINBOW_CYCLE) {
//    rainbowCycle(20);
//  }
//  else if (mode % MODE_MAX == MODE_SOLID_COLOR) {
//    c = potValue;
//    potValue = 50;
//    colorFill(Wheel(c));
//  }
  else if (mode % MODE_MAX == MODE_DYNAMIC_COLOR) {
    colorFill(Wheel(c++));
    delay(300);
  }

  // set last values so we know what changed
  buttonStateLast = buttonState;
  potValueLast = potValue;

//  // Some example procedures showing how to display to the pixels:
//  colorWipe(strip.Color(255, 0, 0), 50); // Red
//  colorWipe(strip.Color(0, 255, 0), 50); // Green
//  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//  // Send a theater pixel chase in...
//  theaterChase(strip.Color(127, 127, 127), 50); // White
//  theaterChase(strip.Color(127,   0,   0), 50); // Red
//  theaterChase(strip.Color(  0,   0, 127), 50); // Blue
//
//  rainbow(20);
//  rainbowCycle(20);
//  theaterChaseRainbow(50);
}

//void pinChange() {
//  // read the state of the pushbutton value:
//  mode++;
//}

// We'll take advantage of the built in millis() timer that goes off
// The SIGNAL(TIMER0_COMPA_vect) function is the interrupt that will be
// Called by the microcontroller every 2 milliseconds
volatile uint8_t counter = 0;
SIGNAL(TIMER0_COMPA_vect) {
  // this gets called every 2 milliseconds
  counter += 2;
  // every 20 milliseconds, read inputs
  if (counter >= 20) {
    counter = 0;
  
    // read the knob
    potValue = analogRead(POT_PIN);
    potValue = map(potValue, 0, 1023, 0, 255);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

// Fill all the dots instantly
void colorFill(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(CB(255 - WheelPos * 3), 0, CB(WheelPos * 3));
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, CB(WheelPos * 3), CB(255 - WheelPos * 3));
  } else {
   WheelPos -= 170;
   return strip.Color(CB(WheelPos * 3), CB(255 - WheelPos * 3), 0);
  }
}

// GetColorWithBrightness
// potValue will be the maximum brightness we want (from 0 - 255), so this changes our color value to be within that range
uint8_t CB(uint8_t c) {
  return map(c, 0, 255, 0, potValue);
}

