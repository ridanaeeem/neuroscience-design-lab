/*
   Use analog read to measure voltage across a potentiometer
   Map this integer reading between 0 and 5 volts
   Report both raw reading and converted voltage
*/

int sensorPin = A0;    // input pin for the potentiometer
int sensorValue = 0;  // store value coming from the sensor
int prevSensorValue = 0;
float voltageValue = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(sensorPin); // read the value
  if (sensorValue != prevSensorValue) {
    voltageValue = 5.0 * sensorValue / 1023; // convert to volts
    // Report the value
    Serial.print("sensorValue = ");
    Serial.print(sensorValue);
    Serial.print(", voltageValue = ");
    Serial.print(voltageValue);
    Serial.println(" V");
  }
  prevSensorValue = sensorValue;
  delay(100);
}
