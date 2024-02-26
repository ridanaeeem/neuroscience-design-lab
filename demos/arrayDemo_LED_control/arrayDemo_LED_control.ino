/*
 * Demonstrate how arrays can be used to simplify code
 * Connect buttons (or any input device) and LEDs to pins listed
 */
 
int buttonPins[] = {2, 4};
int ledPins[] = {3, 5};

void setup() {
  for (int ii = 0; ii < 2; ii++) pinMode(buttonPins[ii], INPUT);
  for (int ii = 0; ii < 2; ii++) pinMode(ledPins[ii], OUTPUT);
}

void loop() {
  for (int ii = 0; ii < 2; ii++)
    digitalWrite(ledPins[ii], !digitalRead(buttonPins[ii]));
}
