/*
 * noisyTod2 -- Simple noisy test synth for NoiseShield
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

const int MAXVOICES = 6;


void setup() 
{
  Serial.begin(19200);
  Serial.println("noisyTod2");

  pinMode( button1Pin, INPUT);
  pinMode( button2Pin, INPUT);
  pinMode( button3Pin, INPUT);

  digitalWrite( button1Pin, HIGH); // turn on internal pull-up
  digitalWrite( button2Pin, HIGH); // turn on internal pull-up
  digitalWrite( button3Pin, HIGH); // turn on internal pull-up

  // (taken from beatvox)
  // Timer1 is configured in Mode 14 (WGM1_3:0=1110),
  // Fast PWM mode with ICR1 as TOP, no prescale.
  // The OC1A/OC1B outputs are configured to clear on compare match 
  // and set at BOTTOM.
  // The TOP value (ICR1) is set to 725 to generate a sample rate 
  // of 16E6/(725+1) = 22039 Hz.
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM12) | _BV(WGM13) | _BV(CS10);
  ICR1 = (726-1);
  //TIMSK1 = _BV(TOIE1); // enable overflow int, every 1/22039Hz = 45.375us.
  OCR1A = OCR1B = 121*MAXVOICES/2;// Begin w/ "quiet" on all voices (3*121=363)
 
}

void loop() 
{

  for( int i=0; i<100; i++) { 
    OCR1A = 100 + 10 * i;
    OCR1B = 100 + 10 * i;
    delay(10);
  }

}


ISR(TIMER1_OVF_vect)
{
  OCR1A = soundL;
  OCR1B = soundR;
}




