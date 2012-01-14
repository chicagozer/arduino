/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

IRsend irsend;

void setup()
{
  Serial.begin(9600);
  #if defined (__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  Serial.println("Mega!!");
  #endif
}

void loop() {
  
  if (Serial.read() != -1) {
    for (int i = 0; i < 1; i++) {
      irsend.sendNEC(0x4FB38C7, 32); // Sony TV power code
      Serial.println("sent.");
    }
  }
}

