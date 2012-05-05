// originally from:
//  http://code.google.com/p/rogue-code/wiki/ToneLibraryDocumentation
//
/// Duelling Tones - Simultaneous tone generation.
//
// This example plays notes 'a' through 'g' sent over the Serial Monitor.
// 's' stops the current playing tone.  Use uppercase letters for the second.

#include "ToneRogue.h"

// pins used on NoiseShield
const int pad1Pin    = 2;
const int padCommPin = 3;
const int pad2Pin    = 4;

const int button1Pin = 5;
const int button2Pin = 6;
const int button3Pin = 7;

const int audioLPin  = 9;
const int audioRPin  = 10;


int notes[] = { NOTE_A3,
                NOTE_B3,
                NOTE_C4,
                NOTE_D4,
                NOTE_E4,
                NOTE_F4,
                NOTE_G4 };

Tone notePlayerL;
Tone notePlayerR;

void setup(void)
{
  Serial.begin(19200);
  Serial.println("ToneTest");
  Serial.println("type a-g, A-G, s/S\n");

  pinMode( button1Pin, INPUT);
  pinMode( button2Pin, INPUT);
  pinMode( button3Pin, INPUT);
  digitalWrite( button1Pin, HIGH); // turn on internal pull-up
  digitalWrite( button2Pin, HIGH); // turn on internal pull-up
  digitalWrite( button3Pin, HIGH); // turn on internal pull-up

  notePlayerL.begin(9);
  notePlayerR.begin(10);
}

boolean playIt = false;
int n;

void loop(void)
{
  char c;

  boolean playItNow = !digitalRead( button1Pin );
  boolean shift     = !digitalRead( button2Pin );

  if( playItNow ) { 
    playIt = true;
    n = (n+1) % 6;
    int ival = (shift) ? 3 : 7;
    float n1 = notes[ n ];
    float n2 = notes[ (n+ival)%6 ];
    //float n2 = n1 + (ival*(1.05946));  // 5th up (7 semitones)
    notePlayerL.play( n1 );
    notePlayerR.play( n2 );
    delay(400);
    for( int i=0; i< 10; i++) { 
      notePlayerL.play( n1 );
      notePlayerR.play( n2 );
      n1 = n1 * 0.99;
      n2 = n2 * 0.99;
      delay(10);
    }
  }
  else { 
    if( playIt ) { 
      notePlayerL.stop();
      notePlayerR.stop();
      playIt = false;
    }
  }
      

  if(Serial.available()) {
    c = Serial.read();
    
    switch(c) {
      case 'a'...'g':
        notePlayerL.play(notes[c - 'a']);
        Serial.println(notes[c - 'a']);
        break;
      case 's':
        notePlayerL.stop();
        break;
      case 'A'...'G':
        notePlayerR.play(notes[c - 'A']);
        Serial.println(notes[c - 'A']);
        break;
      case 'S':
        notePlayerR.stop();
        break;
      case ',':
        delay(300);
        break;
      default:
        notePlayerR.stop();
        notePlayerL.play(NOTE_B2);
        delay(300);
        notePlayerL.stop();
        delay(100);
        notePlayerR.play(NOTE_B2);
        delay(300);
        notePlayerR.stop();
        break;
    }
  }
}
