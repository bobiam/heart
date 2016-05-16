#include "FastLED.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

// modified to work as a simple way to swap between blue and red LEDs for the Heart of the effigy at Flipside 2016 by Bob Eells.
// added a button interrupt with a debounce, added a number of bioluminescentesque patterns.

#if FASTLED_VERSION < 3001000
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    3
const int buttonPin = 2;
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    1
CRGB leds[NUM_LEDS];
bool direction = 0;

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000); // 3 second delay for recovery

  pinMode(buttonPin, INPUT);
  attachInterrupt(0, interrupt_me, CHANGE);

  Serial.begin(9600);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { red, blue };
char* gPatternNames[] = { "blueGreen", "white","red","green","blue","fadeUpDown", "redFadeUpDown", "greenFadeUpDown", "blueFadeUpDown", "popUpFadeDown", "rainbow", "rainbowWithGlitter","sinelon","bpm"};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
  Serial.print("We're now running pattern ");
  Serial.println(gPatternNames[gCurrentPatternNumber]);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  Serial.println(gHue);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void blueGreen(){
  //get us in the 100-175 range
  int thisHue = ( gHue / 3.4 ) + 100;
  leds[0] = CHSV(thisHue,255,255);
}

void fadeUpDown(){
  int how_bright = gHue * 2;
  if(how_bright < 256)
  {
    leds[0] = CHSV(0,0,gHue);
  }else{
    leds[0] = CHSV(0,0,255-gHue);
  }
}

void redFadeUpDown(){
  int how_bright = gHue * 2;
  if(how_bright < 256)
  {
    leds[0] = CRGB(gHue,0,0);
  }else{
    leds[0] = CRGB(255-gHue,0,0);
  }
}

void greenFadeUpDown(){
  int how_bright = gHue * 2;
  if(how_bright < 256)
  {
    leds[0] = CRGB(0,gHue,0);
  }else{
    leds[0] = CRGB(0,255-gHue,0);
  }
}

void blueFadeUpDown(){
  int how_bright = gHue * 2;
  if(how_bright < 256)
  {
    leds[0] = CRGB(0,0,gHue);
  }else{
    leds[0] = CRGB(0,0,255-gHue);
  }
}



void popUpFadeDown(){
  if(gHue == 255)
    direction = !direction;

  if(direction)
    leds[0] = CRGB(gHue,gHue,gHue);

  if(!direction)
    leds[0] = CRGB(255-gHue,255-gHue,255-gHue);
}

void white(){
  leds[0] = CRGB::White;
}

void red(){
  leds[0] = CRGB::Red;
}

void green(){
  leds[0] = CRGB::Green;
}

void blue(){
  leds[0] = CRGB::Blue;
}

void interrupt_me()
{
 static unsigned long last_interrupt_time = 0;
 unsigned long interrupt_time = millis();
 // If interrupts come faster than 200ms, assume it's a bounce and ignore
 if (interrupt_time - last_interrupt_time > 200) 
 {
   nextPattern();
 }
 last_interrupt_time = interrupt_time;
}

