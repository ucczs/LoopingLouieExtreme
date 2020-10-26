int potPin = 0;    // select the input pin for the potentiometer
int val = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  val = analogRead(potPin);    // read the value from the sensor

  Serial.print(val);
  Serial.print("\n");
}
