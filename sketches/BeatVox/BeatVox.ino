/*
   from: http://ruggedcircuits.com/html/beatvox.html#SampleCode

   BeatVox demonstration code

   Based on software developed by WilliamK @ Wusik Dot Com (c) 2011 -
   http://arduino.wusik.com
  
   NOTE: For ATmega1280/2560 you must connect pins D6/D7 to D9/D10. This is
   because on the ATmega1280/2560 we use 16-bit Timer4 instead
   of Timer1 on the ATmega328P.

   This code is free software; you can redistribute it and/or modify it under
   the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.
  
   This code is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
   for more details.
  
   A copy of the GNU General Public License can be viewed at
   <http://www.gnu.org/licenses>
  
   Rugged Circuits LLC
   http://ruggedcircuits.com
*/
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "Sounds.h"

// ======================================================================================= //

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define PCM_L     6 // Left output channel -- PH3/OC4A
  #define PCM_R     7 // Right output channel -- PH4/OC4B
#else
  #define PCM_L     9 // Left output channel -- PB1/OC1A
  #define PCM_R    10 // Right output channel -- PB2/OC1B
#endif

#define FLASH_SSn   2 // NAND FLASH slave select (active low)
#define DIGITAL_PIN 8 // General-purpose digital input pin (active low)
#define ANALOG_PIN  0 // General-purpose potentiometer input

/*
   There are MAXVOICES voices that can play simultaneously, and NumSounds different sounds
   that can be played. At any time, 0 to 6 voices are playing, where each voice is one
   of the NumSounds sounds stored in FLASH.

   NumSounds is defined in the Sounds.h file.
*/
#define MAXVOICES 6

// Stores the number of samples to play in each voice. Changes when a voice is
// set to play a new sound. If this value is 0 then there is no sound to
// play on the given voice.
unsigned int SampleLen[MAXVOICES];

// Stores the address of the next entry in the sound source array that is to be
// played. If this value is 0 then the voice is free to play another sound.
const unsigned char *SampleSrc[MAXVOICES];

// These are the values to write to OCR1A/OCR1B to set the effective PWM values
// The "0 value" for each channel is halfway between 0-242, or 121. 
// There are 3 voices in each channel so if each voice is quiet (0 value, 
// or 121) then summing three quiet voices gives 3*121 or 363.
unsigned int MixerOut[2] = {121*MAXVOICES/2,121*MAXVOICES/2};

// ========================================================================== //
void setup()
{   
    memset(SampleLen,0,sizeof(SampleLen));
    memset(SampleSrc,0,sizeof(SampleSrc));
    
    pinMode(PCM_L,OUTPUT);
    pinMode(PCM_R,OUTPUT);
    
    // 16 Bit Timer Setup -- Timer1 for ATmega328P, Timer4 for ATmega1280/2560
    // We configure the timer for sample rate of 22039 Hz, which is 16E6 / 726.
    
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    // Timer4 is configured in Mode 14 (WGM4_3:0=1110), Fast PWM mode with ICR4 as TOP, no prescale.
    // The OC4A/OC4B outputs are configured to clear on compare match and set at BOTTOM.
    // The TOP value (ICR4) is set to 725 to generate a sample rate of 16E6/(725+1) = 22039 Hz.
    TCCR4A = _BV(COM4A1) | _BV(COM4B1) | _BV(WGM41);
    TCCR4B = _BV(WGM42) | _BV(WGM43) | _BV(CS40);
    ICR4 = (726-1);
    TIMSK4 = _BV(TOIE4); // Enable timer overflow interrupt, once every 1/22039Hz = 45.375us.
    OCR4A = OCR4B = 121*MAXVOICES/2; // Begin with "quiet" value on all voices (3*121 --> 363)
#else
    // Timer1 is configured in Mode 14 (WGM1_3:0=1110), Fast PWM mode with ICR1 as TOP, no prescale.
    // The OC1A/OC1B outputs are configured to clear on compare match and set at BOTTOM.
    // The TOP value (ICR1) is set to 725 to generate a sample rate of 16E6/(725+1) = 22039 Hz.
    TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
    TCCR1B = _BV(WGM12) | _BV(WGM13) | _BV(CS10);
    ICR1 = (726-1);
    TIMSK1 = _BV(TOIE1); // Enable timer overflow interrupt, once every 1/22039Hz = 45.375us.
    OCR1A = OCR1B = 121*MAXVOICES/2; // Begin with "quiet" value on all voices (3*121 --> 363)
#endif
}

int8_t getFreeVoice(void)
{
  uint8_t voice;

  for (voice=0; voice < MAXVOICES; voice++) {
    if (SampleLen[voice] == 0) return voice;
  }

  return -1;
}

/* Basic sound playing interface. Take the sound number (from 0 to NumSounds-1)
   and start playing it on the next free voice.
*/
void playSound(uint8_t sound)
{
    int8_t voice;

    if (sound >= NumSounds) return; // Illegal sound number
      
    voice = getFreeVoice();
    if (voice < 0) return; // All voices in use

    SampleLen[voice] = SoundLengths[sound];
    SampleSrc[voice] = SoundPointers[sound];
}

void loop()
{
  uint8_t i;

  // First play one sound at a time
  for (i=0; i < NumSounds; i++) {
    playSound(i);
    delay(250);
  }

  // Now play two sounds at once to demonstrate multi-voice capability
  for (i=0; i < NumSounds; i += 2) {
    playSound(i);
    playSound(i+1);
    delay(500);
  }

  // Now play four sounds at once to demonstrate multi-voice capability
  for (i=0; i < NumSounds; i += 4) {
    playSound(i);
    playSound(i+1);
    playSound(i+2);
    playSound(i+3);
    delay(500);
  }

}

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
ISR(TIMER4_OVF_vect)
#else
ISR(TIMER1_OVF_vect)
#endif
{   
  uint8_t voice;

  // Do this first so the PWM is updated at equally spaced intervals
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  OCR4A = MixerOut[0];
  OCR4B = MixerOut[1];
#else
  OCR1A = MixerOut[0];
  OCR1B = MixerOut[1];
#endif

  // Now calculate the next PWM output values
  MixerOut[0]=MixerOut[1]=0;
  for (voice=0; voice < MAXVOICES/2; voice++) {
    if (SampleLen[voice]) {
      SampleLen[voice]--;
      MixerOut[0] += pgm_read_byte(SampleSrc[voice]++);
    } else {
      MixerOut[0] += 121;
    }
  }
  for (       ; voice < MAXVOICES; voice++) {
    if (SampleLen[voice]) {
      SampleLen[voice]--;
      MixerOut[1] += pgm_read_byte(SampleSrc[voice]++);
    } else {
      MixerOut[1] += 121;
    }
  }
}
// vim: syntax=cpp ai cindent ts=2 sw=2 expandtab
