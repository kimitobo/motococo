/*

   _______ ___________  /______________________________ 
  __  __ `__ \  __ \  __/  __ \  ___/  __ \  ___/  __ \
  _  / / / / / /_/ / /_ / /_/ / /__ / /_/ / /__ / /_/ /
  /_/ /_/ /_/\____/\__/ \____/\___/ \____/\___/ \____/

=========================================================================================================
  Hardware Platform: Attiny85 + audiobootloader
                   
                             ATTINY85 Pins
                             =============
                                 _______
                               |   U   |                                     
  Reset/Sync out <- D5/A0  PB5-|       |- VCC                                
  SoundPrg/Btn   -> D3/A3  PB3-| ATTINY|- PB2  D2    -> Not used
  Analog Sensor  -> D4/A2  PB4-|   85  |- PB1  D1    -> Audio out
                           GND-|       |- PB0  D0    -> Neopixels (x20)
                               |_______|
    
=========================================================================================================
  
  MOTOCOCO LJUSPIXLAR 1.0
  ----------------------------------
  Various LIGHT animations using strip of Neopixels (in this example x20). There is also analog sensor
  that triggers and/or controle different LED animations. This code has three variations, one using
  microphone sensor, one using IR-proximity sensor(SHARP GP2D12) and one using light sensor(LDR).

********************************************* list of authors *******************************************
  
 Based on TinyAudioBoot & hex2wav by Chris Haberer, Fredrik Olofsson, Budi Prakosa and 8BitMixtape 
 project by Marc Dusjagr Dusseiller & co.
 https://github.com/ChrisMicro/AttinySound
 http://wiki.8bitmixtape.cc
  
 LED VU meter for Arduino and Adafruit NeoPixel LEDs.
 Written by Adafruit Industries.  Distributed under the BSD license.
 This paragraph must be included in any redistribution.
 https://learn.adafruit.com/led-ampli-tie/the-code
 
 scale function:
 Floating Point Autoscale Function V0.1
 Written by Paul Badger 2007
 Modified from code by Greg Shakar

 Mashed-up for Motococo board by Tuomo Tammenpää | @kimitobo

 It is mandatory to keep the list of authors in this code.
 Please add your name if you improve/extend something

************************************************* licence **********************************************

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
****************************************************************************************************** */

#include <Adafruit_NeoPixel.h>
#include <math.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define SPEAKERPIN      1
#define ANALOG_IN       A2  // Microphone, IR-range or LDR
#define SAMPLE_WINDOW   10  // Sample window for average level
#define PEAK_HANG       24  // Time of pause before peak dot falls
#define PEAK_FALL        4  // Rate of falling peak dot
#define INPUT_FLOOR     10  // Lower range of analogRead input
#define INPUT_CEILING  500  // Max range of analogRead input, the lower the value the more sensitive (1023 = max)

#define NUMPIXELS      20 
#define NEOPIXELPIN     0

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);


byte peak = 16;        // Peak level of column; used for falling dots
unsigned int sample;
byte dotCount = 0;     //Frame counter for peak dot
byte dotHangCount = 0; //Frame counter for holding peak dot

float fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* the setup routine runs once when you start the Motococo or press reset
========================================================================================================================
   _________            _                   
  | setup() |   ___ ___| |_ _ _ _ _ 
  |  o___o  |  |_ -| -_|  _| | | . |
  |__/___\__|  |___|___|_| |___|  _|
                               |_|    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void setup() 
{
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  
  pixels.begin();
  pixels.setBrightness(10);       // Neopixel brightness (255 = max)
  pinMode(SPEAKERPIN, OUTPUT);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Specific functions of MOTOCOCO LJUSPIXLAR*/

//Used to draw a line between two points of a given color
void drawLine(uint8_t from, uint8_t to, uint32_t c) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int i=from; i<=to; i++){
    pixels.setPixelColor(i, c);
  }
}


float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }

  return rangedValue;
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void randomizer() {
  if(random(50) > 47) {
    uint8_t pixNum = random(20);
    uint8_t colorR = random(255);
    uint8_t colorG = random(255);   
    uint8_t colorB = random(255);
    pixels.setPixelColor(pixNum, pixels.Color(colorR, colorG, colorB));
  } 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* The main loop to put all your code
========================================================================================================================
   _________    _    
  | loop()  |  | |___ ___ ___ 
  |  o___o  |  | | . | . | . | 
  |__/___\__|  |_|___|___|  _| 
                         |_| 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void loop() 
{
  //int NUMPIXELS = 20;
  unsigned long startMillis= millis();  // Start of sample window
  float peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  unsigned int c, y;

  // collect data for length of sample window (in mS)
  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(ANALOG_IN);
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
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude

  //Fill the strip with rainbow gradient
  for (int i=0;i<=NUMPIXELS-1;i++){
    pixels.setPixelColor(i,Wheel(map(i,0,NUMPIXELS-1,30,150)));
  }

  //Scale the input logarithmically instead of linearly
  c = fscale(INPUT_FLOOR, INPUT_CEILING, NUMPIXELS, 0, peakToPeak, 2);

  // Turn off pixels that are below volume threshold.
  if(c < peak) {
    peak = c;        // Keep dot on top
    dotHangCount = 0;    // make the dot hang before falling
  }
  if (c <= NUMPIXELS) { // Fill partial column with off pixels
    drawLine(NUMPIXELS, NUMPIXELS-c, pixels.Color(0, 0, 0));
  }

  // Set the peak dot to match the rainbow gradient
  y = NUMPIXELS - peak;
  pixels.setPixelColor(y-1,Wheel(map(y,0,NUMPIXELS-1,30,150)));

  randomizer();   
  pixels.show();


  // Frame based peak dot animation
  if(dotHangCount > PEAK_HANG) { //Peak pause length
    if(++dotCount >= PEAK_FALL) { //Fall rate 
      peak++;
      dotCount = 0;
    }
  } 
  else {
    dotHangCount++; 
  }
}
