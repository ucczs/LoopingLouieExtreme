
//Pin connected to ST_CP of 74HC595 (LATCH)
const int latchPin = 6;
//Pin connected to SH_CP of 74HC595 (CLOCK)
const int clockPin = 7;
////Pin connected to DS of 74HC595 (DATA)
const int dataPin = 8;

const int game_modus_pin = 13;


unsigned long int currentTime = 0;
unsigned long int lastTime_LED = 0;
int led_show_time = 250;

int counterLED = 0;
int led_show_array[2][10][19];

int modus_led_output = 1;
int game_modus = 1;
int taster_blocked = 0;
unsigned long int current_time_taster = 0;
int debounce_time = 400;
int game_taster_value = 0;


void setup() {

  //LED Show 1 -- Lauflicht
  led_show_array[0][0][0] = 0x22;
  led_show_array[0][0][1] = 0x06;
  led_show_array[0][0][2] = 0x14;
  led_show_array[0][0][3] = 0x50;
  led_show_array[0][0][4] = 0xC0;
  led_show_array[0][0][5] = 0x81;
  led_show_array[0][0][6] = 0x09;
  led_show_array[0][0][7] = 0x28;
  led_show_array[0][0][8] = 0x22;
  led_show_array[0][0][9] = 0x06;
  led_show_array[0][0][10] = 0x14;
  led_show_array[0][0][11] = 0x50;
  led_show_array[0][0][12] = 0xC0;
  led_show_array[0][0][13] = 0x81;
  led_show_array[0][0][14] = 0x09;
  led_show_array[0][0][15] = 0x28;

  led_show_array[1][0][0] = 0x03;
  led_show_array[1][0][1] = 0x11;
  led_show_array[1][0][2] = 0x09;
  led_show_array[1][0][3] = 0x28;
  led_show_array[1][0][4] = 0x0C;
  led_show_array[1][0][5] = 0x44;
  led_show_array[1][0][6] = 0x06;
  led_show_array[1][0][7] = 0x82;
  led_show_array[1][0][8] = 0x03;
  led_show_array[1][0][9] = 0x11;
  led_show_array[1][0][10] = 0x09;
  led_show_array[1][0][11] = 0x28;
  led_show_array[1][0][12] = 0x0C;
  led_show_array[1][0][13] = 0x44;
  led_show_array[1][0][14] = 0x06;
  led_show_array[1][0][15] = 0x82;

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(game_modus_pin, INPUT);

}

void loop() {


  currentTime = millis();


  game_taster_value = digitalRead(game_modus_pin);

  if ( game_taster_value == HIGH && taster_blocked == 0 )
  {
    taster_blocked = 1;
    
    game_modus++;
    if (game_modus >= 5) game_modus = 1;

    // set LED Output
    if (game_modus == 1) modus_led_output = 1;
    else if (game_modus == 2) modus_led_output = 2;
    else if (game_modus == 3) modus_led_output = 4;
    else if (game_modus == 4) modus_led_output = 8;

    current_time_taster = millis();
  }

  if ( (currentTime - current_time_taster) > debounce_time && taster_blocked == 1 )
  {
    taster_blocked = 0;
  }




  if ( currentTime - lastTime_LED > led_show_time )
  {
    lastTime_LED = millis();

    counterLED++;
    if ( counterLED > 15 )
    {
      counterLED = 0;
    }

    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, LOW);
    //move 'em out
    shiftOut(dataPin, clockPin, MSBFIRST, modus_led_output);
    shiftOut(dataPin, clockPin, LSBFIRST, 255);
    shiftOut(dataPin, clockPin, LSBFIRST, led_show_array[0][0][counterLED]);
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(latchPin, HIGH);

  }


}
