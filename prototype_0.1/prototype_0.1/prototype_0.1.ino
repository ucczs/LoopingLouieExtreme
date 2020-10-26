//**************************************************************//
//  Name    : shiftOutCode, Predefined Dual Array Style         //
//  Author  : Carlyn Maw, Tom Igoe                              //
//  Date    : 25 Oct, 2006                                      //
//  Version : 1.0                                               //
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : to count from 0 to 255                            //
//****************************************************************

/////// PIN-BELEGUNG ///////////////////////

// Schieberegister
//Pin connected to ST_CP of 74HC595
int latchPin = 9;
//Pin connected to SH_CP of 74HC595
int clockPin = 13;
////Pin connected to DS of 74HC595
int dataPin = 7;

// Potentiometer
int potiPin = A7; // Pin A7
int potiValue = 0;

// Motor
int motorPin1 = 3;
int motorPin2 = 5;
int motor_speed = 0;

// Taster
int taster1_speedUp = 2;
int taster1_slowDown = 9;
int taster2_speedUp = 10;
int taster2_slowDown = 11;
int taster3_speedUp = 12;
int taster3_slowDown = 13;
int taster4_speedUp = 14; // analog pin 0
int taster4_slowDown = 15; // analog pin 1


//item variables

int result1_speedUp;
int result1_slowDown;
int result2_speedUp;
int result2_slowDown;
int result3_speedUp;
int result3_slowDown;
int result4_speedUp;
int result4_slowDown;

int item_active = 0;

//holders for infromation you're going to pass to shift register
byte dataLED_SHOW;
byte dataLED_ITEMS;
byte dataArrayLED_SHOW[10];
byte dataArrayLED_ITEMS[10];

//variables for timing (multitasking)
unsigned long currentMillis = 0;
unsigned long lastMillis_LED = 0;
unsigned long lastMillis_Motor = 0;

unsigned long time_LED_show = 200;
unsigned long time_item_active = 1500;


//loop variables (counter)
int cLED_show = 0;

//control flags
int poti_flag = 0;
int taster_flag = 0;






//*************************************************************************setup**

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  pinMode(taster1_speedUp, INPUT);
  pinMode(taster1_slowDown, INPUT); 
  pinMode(taster2_speedUp, INPUT);
  pinMode(taster2_slowDown, INPUT); 
  pinMode(taster3_speedUp, INPUT);
  pinMode(taster3_slowDown, INPUT); 
  pinMode(taster4_speedUp, INPUT);
  pinMode(taster4_slowDown, INPUT); 

  //Arduino doesn't seem to have a way to write binary straight into the code 
  //so these values are in HEX.  Decimal would have been fine, too. 
  dataArrayLED_SHOW[0] = 0xFF; //11111111
  dataArrayLED_SHOW[1] = 0xFE; //11111110
  dataArrayLED_SHOW[2] = 0xFC; //11111100
  dataArrayLED_SHOW[3] = 0xF8; //11111000
  dataArrayLED_SHOW[4] = 0xF0; //11110000
  dataArrayLED_SHOW[5] = 0xE0; //11100000
  dataArrayLED_SHOW[6] = 0xC0; //11000000
  dataArrayLED_SHOW[7] = 0x80; //10000000
  dataArrayLED_SHOW[8] = 0x00; //00000000
  dataArrayLED_SHOW[9] = 0xE0; //11100000

  //Arduino doesn't seem to have a way to write binary straight into the code 
  //so these values are in HEX.  Decimal would have been fine, too. 
  dataArrayLED_ITEMS[0] = 0xFF; //11111111
  dataArrayLED_ITEMS[1] = 0x7F; //01111111
  dataArrayLED_ITEMS[2] = 0x3F; //00111111
  dataArrayLED_ITEMS[3] = 0x1F; //00011111
  dataArrayLED_ITEMS[4] = 0x0F; //00001111
  dataArrayLED_ITEMS[5] = 0x07; //00000111
  dataArrayLED_ITEMS[6] = 0x03; //00000011
  dataArrayLED_ITEMS[7] = 0x01; //00000001
  dataArrayLED_ITEMS[8] = 0x00; //00000000
  dataArrayLED_ITEMS[9] = 0x07; //00000111

  //function that blinks all the LEDs
  //gets passed the number of blinks and the pause time
  blinkAll_2Bytes(2,500); 

  Serial.begin(9600);
}








//*************************************************************************loop**
void loop() {

  // saves current time
  currentMillis = millis();


////////////// POTENTIOMETER //////////////////////////////////////////////

  // value from 0 - 1023
  if( poti_flag == 1 )
  potiValue = analogRead(potiPin);
  else potiValue = 250;


////////////// TASTER /////////////////////////////////////////////////////
  if( item_active == 0 && taster_flag == 1)
  {
    result1_speedUp = digitalRead(taster1_speedUp);
    result1_slowDown = digitalRead(taster1_slowDown);
    result2_speedUp = digitalRead(taster2_speedUp);
    result2_slowDown = digitalRead(taster2_slowDown);
    result3_speedUp = digitalRead(taster3_speedUp);
    result3_slowDown = digitalRead(taster3_slowDown);
    result4_speedUp = digitalRead(taster4_speedUp);
    result4_slowDown = digitalRead(taster4_slowDown);
  }

////////////// LED SHOW ///////////////////////////////////////////////////
  // if enough time passed, go in
  if( currentMillis - lastMillis_LED > time_LED_show )
  {
    // save time of this round
    lastMillis_LED = millis();

    // counter for different show states
    cLED_show++;
    if( cLED_show > 9 ) cLED_show = 0;
    
    dataLED_SHOW = dataArrayLED_SHOW[cLED_show];
    dataLED_ITEMS = dataArrayLED_ITEMS[cLED_show]; 

    Serial.print(cLED_show);
    Serial.print("\n");
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, LOW);
    //move 'em out
    shiftOut(dataPin, clockPin, MSBFIRST, cLED_show);   
    shiftOut(dataPin, clockPin, MSBFIRST, cLED_show);
    //return the latch pin high to signal chip that it 
    //no longer needs to listen for information
    digitalWrite(latchPin, HIGH);           
  }

////////////// MOTOR  ///////////////////////////////////////////////////

  // wenn kein item aktiviert ist, aktuelle Motorgeschwindigkeit berechnen
  if( item_active == 0 )
  {
    motor_speed = ( potiValue/4 ) / 0.6 + 20; // value: 20 - 173    
  }

  // wenn speed-up item aktiviert wurde und aktuell kein item aktiv ist
  if( ( result1_speedUp || result2_speedUp || result3_speedUp || result4_speedUp ) && item_active == 0 && taster_flag == 1 )
  {
    item_active = 1;
    lastMillis_Motor = millis();
    motor_speed = motor_speed + 30;
  }

  // wenn slow-down item aktiviert wurde und aktuell kein item aktiv ist
  if( ( result1_slowDown || result2_slowDown || result3_slowDown || result4_slowDown ) && item_active == 0 && taster_flag == 1 )
  {
    item_active = 1;
    lastMillis_Motor = millis();
    motor_speed = motor_speed - 30;
    // damit Motor nicht zu langsam wird und anhält --> testen!
    if( motor_speed < 20 ) motor_speed = 20;
  }  
  
  currentMillis = millis();
  
  // item nach Zeit wieder deaktivieren
  if( currentMillis - lastMillis_Motor > time_item_active && taster_flag == 1)
  {
    item_active = 0;
  }
  
  analogWrite(motorPin1, motor_speed);   // Motor Vor
  analogWrite(motorPin2, 0);

}




//*************************************************************************functions**

//blinks the whole register based on the number of times you want to 
//blink "n" and the pause between them "d"
//starts with a moment of darkness to make sure the first blink
//has its full visual effect.
void blinkAll_2Bytes(int n, int d) 
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
  delay(200);
  for (int x = 0; x < n; x++) 
  {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 255);
    shiftOut(dataPin, clockPin, MSBFIRST, 255);
    digitalWrite(latchPin, HIGH);
    delay(d);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    shiftOut(dataPin, clockPin, MSBFIRST, 0);
    digitalWrite(latchPin, HIGH);
    delay(d);
  }
}

