/* 
                   _____                               
  _______ ___________  /______________________________ 
  __  __ `__ \  __ \  __/  __ \  ___/  __ \  ___/  __ \
  _  / / / / / /_/ / /_ / /_/ / /__ / /_/ / /__ / /_/ /
  /_/ /_/ /_/\____/\__/ \____/\___/ \____/\___/ \____/

=========================================================================================================
  Hardware Platform: Attiny85 + audiobootloader / "CocoCore"
                   
                             ATTINY85 Pins
                             =============
                                 _______
                               |   U   |                                     
  Reset/Sync out <- D5/A0  PB5-|       |- VCC                                
  SoundPrg/Btn   -> D3/A3  PB3-| ATTINY|- PB2  D2    -> PWM SERVO_2 
  Analog Sensor  -> D4/A2  PB4-|   85  |- PB1  D1    -> PWM SERVO_1 / Audio out
                           GND-|       |- PB0  D0    -> Neopixels J(3x+)
                               |_______|


    
=========================================================================================================
  
  Servo motor controller on CocoCore
  ----------------------------------
  Move two servos up and down
  
*********************************************************************************************************
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
********************************************* list of outhors *******************************************
  Based on TinyAudioBoot and hex2wav by Chris Haberer, Fredrik Olofsson, Budi Prakosa
  https://github.com/ChrisMicro/AttinySound
  
  Servo code adapted from Arduino forum: 
  http://forum.arduino.cc/index.php?topic=5983.0 
  
  Cleaned up for workshop use by Tuomo Tammenpää | @kimitobo
****************************************************************************************************** */

#ifdef __AVR__
#include <avr/power.h>
#endif

#define POTI_LEFT A2
#define SERVO_1 1
#define SERVO_2 2
#define Vcc            37 // 3.7 V for LiPo
#define Vdiv           26 // measure max Voltage on Analog In

int angle;
int table;
int pwm;

int servoArray[] =  {
  0, 45, 90, 135, 180 
};

uint16_t analogSensor;


void setup()
{
 #if defined (__AVR_ATtiny85__)
   if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
 #endif 
 pinMode(SERVO_1, OUTPUT);
 pinMode(SERVO_2, OUTPUT);
}
 
void servo1Pulse (int servo, int angle)
{
 analogSensor = 1023 - analogReadScaled(POTI_LEFT)>>2;    // Scale analog input value and reduce to 8bit
 pwm = (angle*11) + 500;                                  // Convert angle to microseconds
 digitalWrite(SERVO_1, HIGH);
 delayMicroseconds(pwm);                                  // pwm: 1000μs-2000μs == 0-180° turn
 digitalWrite(SERVO_1, LOW);                              // (depends on servo, here SG90 "miniservo")
 delay(analogSensor);                                     // Delay from analog input, scaled
}

void servo2Pulse (int servo, int angle)
{
 analogSensor = 1023 - analogReadScaled(POTI_LEFT)>>2;    // Scale analog input value and reduce to 8bit
 pwm = (angle*11) + 500;                                  // Convert angle to microseconds
 digitalWrite(SERVO_2, HIGH);
 delayMicroseconds(pwm);                                  // pwm: 1000μs-2000μs == 0-180° turn
 digitalWrite(SERVO_2, LOW);                              // (depends on servo, here SG90 "miniservo")
 delay(analogSensor);                                     // Delay from analog input, scaled
}

uint16_t analogReadScaled(uint8_t channel)                // Analog input scaler
{
  uint16_t value = analogRead(channel);
  value = value * Vcc / Vdiv;
  if (value > 1023) value = 1023;
  return value;
}

void loop ()
{
 for (angle = 0; angle <= 180; angle += 2)  {
   servo1Pulse(SERVO_1, angle);  
   servo2Pulse(SERVO_2, angle);  
   }
 for (angle = 180; angle >= 0; angle -= 2)  {
   servo1Pulse(SERVO_1, angle);  
   servo2Pulse(SERVO_2, angle); 
   }
/*
 for (table = 0; table <=4; table +=1) {
   servo1Pulse(SERVO_1, servoArray[table]);  
   servo2Pulse(SERVO_2, servoArray[table]); 
   }
*/   
}
