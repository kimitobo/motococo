/* 
                   _____                               
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
  SoundPrg/Btn   -> D3/A3  PB3-| ATTINY|- PB2  D2    -> PWM SERVO_2 
  Analog Sensor  -> D4/A2  PB4-|   85  |- PB1  D1    -> PWM SERVO_1 / Audio out
                           GND-|       |- PB0  D0    -> Neopixels (3x+)
                               |_______|
    
=========================================================================================================
  
  MOTOCOCO BLINK 0.1
  ----------------------------------
  Change RGB values of the Neopixels randomly
  Analogsensor affects on the speed of change

********************************************* list of outhors *******************************************
  
  Based on TinyAudioBoot and hex2wav by Chris Haberer, Fredrik Olofsson, Budi Prakosa
  https://github.com/ChrisMicro/AttinySound

  NEO-Pixel blinks:
  Simplified from Haberer & Dusjagr work on 8Bitmixtape
  https://github.com/8BitMixtape/8Bit-Mixtape-NEO
  
  Servo code adapted from Arduino forum: 
  http://forum.arduino.cc/index.php?topic=5983.0 
  
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

#ifdef __AVR__
#include <avr/power.h>
#endif

// hardware description / pin connections
#define SPEAKERPIN      1
#define NEOPIXELPIN     0
#define POTI_LEFT      A2     // MOTOCOCO! Only one pot / sensor
#define BUTTONS_ADC    A3     // MOTOCOCO! Only one button
#define NUMPIXELS      19     // MOTOCOCO! Number of Neopixels vary

// hardware calibration
#define Vbutton_left   380
#define Vcc            37 // 3.7 V for LiPo
#define Vdiv           26 // measure max Voltage on Analog In

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

// variables
uint16_t colorPoti;
byte buttonState1 = LOW; 
byte lastButtonState1 = LOW;
byte buttonCount1 = 0;
byte buttonReset1 = 4;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Specific functions of the 8Bit Mixtape NEO
 ___ _____ _ _      _____ _     _                  _____ _____ _____ 
| . | __  |_| |_   |     |_|_ _| |_ ___ ___ ___   |   | |   __|     |
| . | __ -| |  _|  | | | | |_'_|  _| .'| . | -_|  | | | |   __|  |  |
|___|_____|_|_|    |_|_|_|_|_,_|_| |__,|  _|___|  |_|___|_____|_____|
                                       |_|                           
========================================================================================================================
   _________    
  | 8Bit(x) |   uint8_t   getButton()                       -> 1 left, 2 right, 3 both 
  |  o___o  |   uint16_t  analogReadScaled(uint8_t channel) -> scale the adc of the voltage divider to a retur 0..1023     
  |__/___\__|   MOTOCOCO!: Only 1 button in Motococo, 2-button code left for compatibility with 8Bitmixtape & Neocococat
                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

uint8_t getButton()
{
  uint8_t button = 0;
  if (analogRead(BUTTONS_ADC) < Vbutton_left) button = 1;

  return button;
}

uint8_t getButtonState()
{
  uint8_t state = 0;
  int x = getButton();
  
  if (x == 0) {
    buttonState1 = LOW; // high means not pressed
  }
  if (x == 1) buttonState1 = HIGH; // low means pressed

  if (buttonState1 != lastButtonState1 && buttonState1 == LOW) {
    //do this on press of button 1
       buttonCount1++;
       if (buttonCount1 >= buttonReset1) {
        buttonCount1 = 0;
       }
  }
  return state;
}

uint16_t analogReadScaled(uint8_t channel)
{
  uint16_t value = analogRead(channel);
  value = 700 - value;
  return value;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Specific functions of the NEO-Pixel Library
 _____ _____ _____        _         _ 
|   | |   __|     |   ___|_|_ _ ___| |
| | | |   __|  |  |  | . | |_'_| -_| |
|_|___|_____|_____|  |  _|_|_,_|___|_|
                     |_|   
                                
========================================================================================================================
   _________    
  | NEO(x)  |   void setColorAllPixel(uint32_t color)                   -> Sets all the pixels to the same color
  |  o___o  |   void displayBinrayValue(uint16_t value, uint32_t color) -> displays binary number
  |__/___\__|   uint32_t Wheel(byte WheelPos)                           -> Input a value 0 to 255 to get a color value.   
                                                                        The colours are a transition r - g - b - back to r. 
                void rainbowCycle(uint8_t wait, uint8_t rounds)         -> makes a Rainbow :-)  
                
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

void setColorAllPixel(uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    pixels.setPixelColor(n, color);
  }
}

void displayBinrayValue(uint16_t value, uint32_t color)
{
  uint8_t n;
  for (n = 0; n < NUMPIXELS; n++)
  {
    if (value & (1 << n)) pixels.setPixelColor(n, color);
    //else pixels.setPixelColor(n,0); // off
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, c);    //turn every third pixel on
      }
      pixels.show();

      delay(wait);

      for (uint16_t i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void setOffOnePixel(uint8_t pixNum){
  pixels.setPixelColor(pixNum, pixels.Color(0, 0, 0));
}


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

  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(128);       // Neopixel brightness (255 = max)
  pinMode(SPEAKERPIN, OUTPUT);

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
  

  for(uint16_t i=0; i<pixels.numPixels(); i++) {  
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
    pixels.setPixelColor(19, pixels.Color(255, 0, 0));
    pixels.show();
  }
  delay(analogRead(POTI_LEFT));

  for(uint16_t i=0; i<pixels.numPixels(); i++) {  
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.setPixelColor(19, pixels.Color(0, 0, 0));
    pixels.show();
  }
  delay(analogRead(POTI_LEFT));
  
  //colorWipe(pixels.Color(0, 0, 0), 20);
  
  //rainbowCycle(1);
  //theaterChase(pixels.Color(255, 0, 0), 50);
  //colorWipe(pixels.Color(0, 255, 0), 50);
  
  //if(analogRead(POTI_LEFT) > 400) {
  //  theaterChase(pixels.Color(255, 255, 255), 50);
  //} 

  //if(random(255) > 253) {
  //  rainbowCycle(1);
  //} 
}

