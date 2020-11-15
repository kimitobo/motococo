#ifdef __AVR__
#include <avr/power.h>
#endif

#include <MIDI.h>  // Add Midi Library
#include <SoftwareSerial.h>

SoftwareSerial swSerial(2, 1); // RX, TX

MIDI_CREATE_INSTANCE(SoftwareSerial, swSerial, midiSw1);

#define LED 13    // Arduino Board LED is on Pin 13

void setup()
{
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1); //what is this?
  #endif
  
  midiSw1.begin(MIDI_CHANNEL_OMNI);
  pinMode(2, INPUT);
  pinMode(1, OUTPUT);
}

void loop()
{
  for (int note = 0xB0; note < 0xB9; note ++) { // B=CC, 0-9 channel
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    noteOn(note, 41, random(0,127));  // 43 = sample speed
    noteOn(note, 43, random(0,127));  // 43 = sample speed
    delay(100);
    //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
    //noteOn(0x90, note, 0x00);
    //delay(100);
  }
}

void noteOn(int cmd, int pitch, int velocity) {
  swSerial.write(cmd);
  swSerial.write(pitch);
  swSerial.write(velocity);
} 
