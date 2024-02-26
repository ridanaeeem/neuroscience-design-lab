/*
  LED bar graph

  Turns on a series of LEDs based on the value of an analog sensor.
  This is a simple way to make a bar graph display. Though this graph uses 10
  LEDs, you can use any number by changing the LED count and the pins in the
  array.

  This method can be used to control any series of digital outputs that depends
  on an analog input.

  The circuit:
  - LEDs from pins 2 through 11 to ground

  created 4 Sep 2010
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/BarGraph
*/

// these constants won't change:
const int analogPin = A0;  // the pin that the potentiometer is attached to
const int ledCount = 4;   // the number of LEDs in the bar graph

int ledPins[] = {
  2, 3, 4, 5
};  // an array of pin numbers to which LEDs are attached


void setup() {
  // loop over the pin array and set them all to output:
  Serial.begin(9600);
  for (int thisLed = 0; thisLed < ledCount; thisLed++) {
    pinMode(ledPins[thisLed], OUTPUT);
  }
}

void loop() {
  // read the potentiometer:
  // int sensorReading = analogRead(analogPin);
  // Serial.println(sensorReading);
  // map the result to a range from 0 to the number of LEDs:
  // int ledLevel = map(sensorReading, 0, 1023, 0, ledCount);
  
  for (int ledLevel=0; ledLevel<5; ledLevel++){
    for (int thisLed = 0; thisLed < ledCount; thisLed++) {
      if (thisLed < ledLevel) digitalWrite(ledPins[thisLed], HIGH);
      else digitalWrite(ledPins[thisLed], LOW);
      delay(200);
    }
  }

  // loop over the LED array:
  // for (int thisLed = 0; thisLed < ledCount; thisLed++) {
  //   // if the array element's index is less than ledLevel,
  //   // turn the pin for this element on:
  //   if (thisLed < ledLevel) {
  //     digitalWrite(ledPins[thisLed], HIGH);
  //   }
  //   // turn off all pins higher than the ledLevel:
  //   else {
  //     digitalWrite(ledPins[thisLed], LOW);
  //   }
  // }
}
