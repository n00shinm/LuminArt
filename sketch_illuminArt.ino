/*
  LuminArt by Nooshin Mohtashami
  Creation & Computation Experiment 2

  Circuit:
  - Arduino Nano 33 BLE Sense board

  Some of this code is from the public domain (specifically the part 
  reading sound from the microphone.)

  The analog port A0 is connected to a photo resistor and checks for lighting. If the photo resistor
  reads a dark room, an LED is turned on (connected to D2)
  The code also reads audio data from the on-board PDM microphones.
  When the room is dark and there is sound or music detected, digital ports D9-D12 
  randomly light up (function playLEDs())

  
*/

#include <PDM.h>

// buffer to read samples into, each sample is 16-bits
short sampleBuffer[256];

// number of samples read
volatile int samplesRead;

// setting up the LEDs
int leds[4] = {9,10,11,12};
bool darkRoom = false;

void setup() {
  Serial.begin(9600);
  
  while (!Serial);

  // configure the data receive callback
  PDM.onReceive(onPDMdata);


  //setup LEDs 
  for (int i; i <sizeof(leds)/sizeof(int); i++){
    pinMode(leds[i],OUTPUT);
    delay(10);
  }
  resetLEDs();

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}

void loop() {

    // read the light value from photo resistor connected to A0 
    int analogValue = analogRead(A0);
    if (analogValue < 300) {
//      Serial.println("Dark Room");
      //turn on LED when the room is dark and canvas is ON
      digitalWrite(2, HIGH);   // turn on the LED on D2 to indicate the canvas is ON 
      darkRoom = true;
    } else {
      digitalWrite(2, LOW);   // turn off the LED to indicate the room is NOT dark 
      darkRoom = false;
    }


  // wait for samples to be read
  if (samplesRead) {
    for (int i = 0; i < samplesRead; i++) {
      // check if the sound value is higher than 50 & the room is dark
      if (sampleBuffer[i]>=50 && darkRoom ){
        playLEDs();
      } 
    }

    // clear the read count
    samplesRead = 0;
    resetLEDs();
  }
}

void resetLEDs() {
  for (int i; i <sizeof(leds)/sizeof(int); i++){
    digitalWrite(leds[i],LOW);
  }
}

void playLEDs() {
  digitalWrite(leds[random(0,sizeof(leds)/sizeof(int))],HIGH);
  delay(random(100,900));
  digitalWrite(leds[random(0,sizeof(leds)/sizeof(int))],LOW);
}



void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
