// These constants won't change:
const int sensorPin = A0;  // pin that the sensor is attached to
const int ledPin = 9;      // pin that the LED is attached to

// variables:
int sensorValue = 0;   // the sensor value
int sensorMin = 1023;  // minimum sensor value
int sensorMax = 0;     // maximum sensor value

// number of times we switch from high <--> low
int switches = 0;
// can we print a frequency value
bool canPrint = false;
// used to help measure when we should increment switch, 2 is an arbitrary value
int cur = 2;
// t1
unsigned long prevTimeOn = 0;
// t2
unsigned long curTimeOn = 0;
// the period
float T = 0;
// the frequence
float freq = 0;
// array of frequencies for averaging purposes
float freqs[5];
// index of that array
int freqI = 0;
// array of previous sensor readings for calibration purposes
float lastSeveral[400];
// index of that array
int lastI = 0;

void setup() {
  Serial.begin(9600);
  // turn on LED to signal the start of the calibration period:
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  // calibrate during the first five seconds
  while (millis() < 5000) {
    sensorValue = analogRead(sensorPin);

    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
    }

    // record the minimum sensor value
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
    }
  }

  // signal the end of the calibration period
  digitalWrite(13, LOW);
}

void loop() {
  // https://www.arduino.cc/en/Tutorial/BuiltInExamples/Calibration
  // read the sensor:
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  // in case the sensor value is outside the range seen during calibration
  sensorValue = constrain(sensorValue, sensorMin, sensorMax);
  // apply the calibration to the sensor reading
  // maps Min --> 0, Max --> 255, and everything else in between
  sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 255);


  // recalibrating
  if (lastI < 400) {
    lastSeveral[lastI] = sensorValue;
  } else {
    float minArray = lastSeveral[0];
    float maxArray = lastSeveral[0];
    for (int i=0; i < 400; i++){
      if (lastSeveral[i] < minArray){
        minArray = lastSeveral[i];
      } else if (lastSeveral[i] > maxArray){
        maxArray = lastSeveral[i];
      }
    }
    sensorMin = minArray;
    sensorMax = maxArray;
    lastI = 0;
  }


  // get time information
  // fade the LED using the calibrated value:
  // if (sensorValue < sensorMax /2){
  // below threshold
  if (sensorValue < (255 / 2)) {
    // if it was just on, aka we just switched off
    // cur for below threshold
    cur = 0;
  } else {
    // if it was just below threshold but now isn't
    if (cur != 1) {
      // switches from below --> above
      switches += 1;

      // want to update when we switch from below threshold to above threshold
      // if no previous time on, set this to be the that
      if (prevTimeOn == 0) {
        prevTimeOn = millis();
      } else {
        curTimeOn = millis();
        T = curTimeOn - prevTimeOn;
        prevTimeOn = curTimeOn;
        canPrint = true;
      }
    }
    // cur for above threshold
    cur = 1;

  }

  // calculate frequency if an even number of switches have happened
  // don't want to keep printing while this is true, just do it once
  // if (canPrint && switches > 0 && switches % 2 == 0 && cur == 1){
  if (canPrint && cur == 1) {
    freq = 1000.0 / T;
    if (cur == 1) {
      freqs[freqI] = freq;
      if (freqI < 4) {
        freqI += 1;
      } else {
        float avg = 0;
        float sum = 0;
        for (int i = 0; i < 5; i++) {
          sum += freqs[i];
        }
        avg = sum / 5.0;
        // Serial.println("the average frequency is...");
        Serial.println(avg);
        freqI = 0;
      }
    }

    canPrint = false;
  }
}
