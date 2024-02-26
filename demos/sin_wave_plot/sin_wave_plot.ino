// Use Tools->Serial Plotter to see output sin and cos

float x = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  x = x + 3.14/100;
  if (x >= 6.28) x = 0; 
  Serial.print(sin(x));
  Serial.print(",");
  Serial.println(cos(x));
  delay(10);
}
