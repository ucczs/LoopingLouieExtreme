
const int motorPin1 = 3;
const int motorPin2 = 5;

const int potiAnalogPin = 0;

int potiValue = 0;
int speed_motor = 0;


void setup() {
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  Serial.begin(9600);

}

void loop() {

  potiValue = analogRead(potiAnalogPin);

  speed_motor = 255 - (25 + 0.18 * potiValue);

  Serial.print(speed_motor);
  Serial.print("\n");

}
