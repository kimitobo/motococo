#ifdef __AVR__
#include <avr/power.h>
#endif

#include <SoftwareSerial.h>
SoftwareSerial TinySerial(11, 12); // RX, TX

void setup() {
  
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1); //what is this?
#endif

  
  TinySerial.begin(31250);
  pinMode(11, INPUT);
  pinMode(12, OUTPUT);
  
}

                        
void loop() {
  // play notes from F#-0 (0x1E) to F#-5 (0x5A):
  for (int note = 0xB0; note < 0xB9; note ++) { // B=CC, 0-9 channel
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    noteOn(note, 43, random(0,127));  // 43 = sample speed
    delay(100);
    //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
    //noteOn(0x90, note, 0x00);
    //delay(100);
  }
}

void noteOn(int cmd, int pitch, int velocity) {
  TinySerial.print(cmd);
  TinySerial.print(pitch);
  TinySerial.print(velocity);
} 

