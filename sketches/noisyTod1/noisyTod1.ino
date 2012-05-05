/*
 * noisyTod1 -- Simple noisy test synth for NoiseShield
 * 
 *
 */

// pins used on NoiseShield
const int pad1Pin    = 2;
const int padCommPin = 3;
const int pad2Pin    = 4;

const int button1Pin = 5;
const int button2Pin = 6;
const int button3Pin = 7;

const int audioLPin  = 9;
const int audioRPin  = 10;

// variables for this sketch
float fob = 60;
float fot = 4000;
float f = 1000;
float ratio = 1.01;
int t = 10;

boolean toggle;

void setup() 
{
  Serial.begin(19200);
  Serial.println("toneTod");

  pinMode( button1Pin, INPUT);
  pinMode( button2Pin, INPUT);
  pinMode( button3Pin, INPUT);
  digitalWrite( button1Pin, HIGH); // turn on internal pull-up
  digitalWrite( button2Pin, HIGH); // turn on internal pull-up
  digitalWrite( button3Pin, HIGH); // turn on internal pull-up

}

void loop() 
{
  boolean moving = digitalRead(button1Pin);
  boolean updown = digitalRead(button2Pin);
  boolean normal = digitalRead(button3Pin); 

  if( moving ) { 
    float r = (updown) ? ratio : 1/ratio;    
    f = f * r;
    if( f> fot ) { 
      f = fob;
    } 
    else if( f < fob ) {
      f = fot;
    }
  }
  
  if( !normal ) { // then freakout
    f = random( f - f/10, f + f/10);
  }

  if( toggle ) {
    tone( audioLPin, f, t-1);
  } 
  else {
    tone( audioRPin, f, t-1 );
  }
  toggle=!toggle;

  delay( t );
}






