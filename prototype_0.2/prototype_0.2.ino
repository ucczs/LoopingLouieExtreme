//**************************************************************//
//  Name    : shiftOutCode, Predefined Dual Array Style         //
//  Author  : Carlyn Maw, Tom Igoe                              //
//  Date    : 25 Oct, 2006                                      //
//  Version : 1.0                                               //
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : to count from 0 to 255                            //
//****************************************************************



//***** PIN-SETTING ******************************************************

// digital Pins
const int run_switch_pin = 2;
const int motorPin1 = 3;
const int motorPin2 = 5;
const int latchPin_LED = 6;     //Pin connected to ST_CP of 74HC595 (LATCH)
const int clockPin_LED = 7;     //Pin connected to SH_CP of 74HC595 (CLOCK)
const int dataPin_LED = 8;      //Pin connected to DS of 74HC595 (DATA)
const int dataPin_Taster = 9;
const int latchPin_Taster = 10;
const int clockPin_Taster = 11;
const int itemTaster_pin = 12;
const int game_modus_pin = 13;

// Analog Pins
const int potiAnalogPin = 0;


//***** VARIABLES ********************************************************

// LED
unsigned long int currentTime = 0;
unsigned long int lastTime_LED = 0;

int counterLED = 0;

int led_show_array[2][10][19];
int led_show_time = 100;

// LED SHOW
// 0: circle
// 1: turn on all around

enum led_show_modus {
  CIRCLE_ALL_LED = 0,
  FILL_UP_ALL_LED = 1,
  ALTERNATE_BLINKING_ITEMS = 2,
  ALTERNATE_BLINKING = 3
};

int led_show = 0;


// MOTOR

unsigned long int lastTime_Motor = 0;
int run_switch_value = 0;
int speed_motor = 150;


// POTI
int potiValue = 150;


// GAME MODUS
// 1: speed with poti, no items
// 2: random speed & directions, no itmes
// 3:
// 4:
int game_taster_value = 0;
int debounce_time = 400;
int game_modus = 1;     // 1,2,3 or 4
int taster_blocked = 0;
unsigned long int current_time_taster = 0;
int modus_led_output = 1;

// modus 2
unsigned long int current_random_time = 0;
int random_duration = 1000;
int louie_direction = 0;

enum game_modus {
  POTI_MODUS = 1,
  RANDOM_MODUS = 2,
  POTI_ITEM_MODUS = 3,
  RANDOM_MODUS_HARD = 4
};


// ITEMS
unsigned long int current_time_item = 0;
int item_output = B11111111;
int item_taster_value = 0;
int blink_item_flag = 0;
int item_blocked = 0;
int debounce_item = 3000;
int shift_value = 1;
int item_used[8] = {0};

int speed_up_flag = 0;
int slow_down_flag = 0;





//***** FUNCTION PROTOTYPES ********************************************************

void make_led_show(int led_show);
void initialize_led_arrays();



void setup() {

  initialize_led_arrays();

  //set pins to output because they are addressed in the main loop
  pinMode(latchPin_LED, OUTPUT);
  pinMode(clockPin_LED, OUTPUT);
  pinMode(dataPin_LED, OUTPUT);

  pinMode(latchPin_Taster, OUTPUT);
  pinMode(clockPin_Taster, OUTPUT);
  pinMode(dataPin_Taster, OUTPUT);

  pinMode(itemTaster_pin, INPUT);

  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  pinMode(run_switch_pin, INPUT);
  pinMode(game_modus_pin, INPUT);

}

void loop()
{

  currentTime = millis();

  potiValue = analogRead(potiAnalogPin);

  game_taster_value = digitalRead(game_modus_pin);
  run_switch_value = digitalRead(run_switch_pin);


  ////////// MODUS TASTER + DEBOUNCING /////////////////////////////////////////////////////////////////////////////////
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

    counterLED = 0;

    current_time_taster = millis();
  }

  currentTime = millis();

  if ( (currentTime - current_time_taster) > debounce_time && taster_blocked == 1 )
  {
    taster_blocked = 0;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////



  /////// RUN LOUIE ///////////////////////////////////////////////////////////////////////////////////

  if ( run_switch_value == LOW )
  {

    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, 0);

    /*
      CIRCLE_ALL_LED
      FILL_UP_ALL_LED
      ALTERNATE_BLINKING_ITEMS
    */
    make_led_show(ALTERNATE_BLINKING);

    // items wieder zurücksetzen
    item_output = B11111111;
    for (int i = 0; i < 8; i++) item_used[i] = 0;
  }
  else
  {
    switch (game_modus)
    {
      case POTI_MODUS:     // speed über Poti einstellen ohne Items
        speed_motor = 255 - (25 + 0.18 * potiValue);
        analogWrite(motorPin1, speed_motor);
        // je größer der Speed-Wert, desto geringer die Geschwindigkeit
        analogWrite(motorPin2, 0);

        make_led_show(CIRCLE_ALL_LED);

        break;

      case RANDOM_MODUS:     // zufällige Geschwindigkeit
        currentTime = millis();

        make_led_show(FILL_UP_ALL_LED);

        if ( currentTime - current_random_time > random_duration )
        {
          current_random_time = millis();
          speed_motor = random(30, 200);
          louie_direction = random(0, 100);

          // decide about direction
          if (louie_direction > 90)
          {
            analogWrite(motorPin1, 0);
            analogWrite(motorPin2, speed_motor);
          }
          else
          {
            analogWrite(motorPin1, speed_motor);
            analogWrite(motorPin2, 0);
          }

        }
        break;

      case POTI_ITEM_MODUS:       // speed über poti mit Items

        // Taster alle auf High setzen
        digitalWrite(latchPin_Taster, LOW);
        shiftOut(dataPin_Taster, clockPin_Taster, MSBFIRST, 255);
        digitalWrite(latchPin_Taster, HIGH);

        delayMicroseconds(500);

        item_taster_value = digitalRead(itemTaster_pin);

        ///////////////// TASTER CHECK ////////////////////////////
        if ( item_taster_value == HIGH && item_blocked == 0)
        {
          shift_value = 1;

          for (int i = 0; i < 8; i++)
          {
            digitalWrite(latchPin_Taster, LOW);
            shiftOut(dataPin_Taster, clockPin_Taster, MSBFIRST, shift_value);
            digitalWrite(latchPin_Taster, HIGH);

            delayMicroseconds(500);

            item_taster_value = digitalRead(itemTaster_pin);

            if ( item_taster_value == HIGH )
            {
              // prüfen ob Item schon verbraucht
              // bsp. item_output:  B10110001
              // j = 4
              // 1 << 4 =           B00010000
              // ver-&          =   B00010000
              // > 0
              if ( (item_output ^ (1 << i) ) > 0 )
              {
                // turn light off
                switch (i)
                {
                  case 0:         // red button
                    item_output = item_output & B11101111;
                    slow_down_flag = 1;
                    break;
                  case 1:         // green button
                    item_output = item_output & B11111011;
                    speed_up_flag = 1;
                    break;
                  case 2:         // green button
                    item_output = item_output & B11110111;
                    speed_up_flag = 1;
                    break;
                  case 3:         // red button
                    item_output = item_output & B01111111;
                    slow_down_flag = 1;
                    break;
                  case 4:         // red button
                    item_output = item_output & B11011111;
                    slow_down_flag = 1;
                    break;
                  case 5:         // green button
                    item_output = item_output & B11111101;
                    speed_up_flag = 1;
                    break;
                  case 6:         // green button
                    item_output = item_output & B11111110;
                    speed_up_flag = 1;
                    break;
                  case 7:         // red button
                    item_output = item_output & B10111111;
                    slow_down_flag = 1;
                    break;
                }
                item_blocked = 1;

                // if item already used, dont use it
                if ( item_used[i] == 0 ) item_used[i] = 1;
                else
                {
                  slow_down_flag = 0;
                  speed_up_flag = 0;
                  item_blocked = 0;
                }

              }
              else item_blocked = 0;
            }
            shift_value = shift_value << 1;
          }

          current_time_item = millis();

        }

        //////////// DEBOUNCE ITEMS ///////////////////////
        currentTime = millis();

        if ( currentTime - current_time_item > debounce_item && item_blocked == 1 )
        {
          item_blocked = 0;
          // Taster alle auf High setzen
          digitalWrite(latchPin_Taster, LOW);
          shiftOut(dataPin_Taster, clockPin_Taster, MSBFIRST, 255);
          digitalWrite(latchPin_Taster, HIGH);
          speed_up_flag = 0;
          slow_down_flag = 0;
        }



        if (speed_up_flag == 1)
        {
          speed_motor = 255 - (25 + 0.18 * potiValue) + 100;
          if ( speed_motor > 230 ) speed_motor = 230;
        }
        else if (slow_down_flag  == 1)
        {
          speed_motor = 255 - (25 + 0.18 * potiValue) - 70;
          if ( speed_motor < 45 ) speed_motor = 45;
        }
        else speed_motor = 255 - (25 + 0.18 * potiValue);


        ///////////////// TASTER CHECK ////////////////////////////



        analogWrite(motorPin1, speed_motor);
        // je größer der Speed-Wert, desto geringer die Geschwindigkeit
        analogWrite(motorPin2, 0);

        make_led_show(ALTERNATE_BLINKING_ITEMS);



        break;

      case RANDOM_MODUS_HARD:     // zufällige Geschwindigkeit
        currentTime = millis();

        make_led_show(ALTERNATE_BLINKING_ITEMS);

        if ( currentTime - current_random_time > random_duration )
        {
          current_random_time = millis();
          speed_motor = random(80, 190);
          louie_direction = random(0, 100);

          // decide about direction
          if (louie_direction > 60)
          {
            //backwards
            analogWrite(motorPin1, 0);
            analogWrite(motorPin2, speed_motor);
          }
          else
          {
            analogWrite(motorPin1, speed_motor);
            analogWrite(motorPin2, 0);
          }

        }
        break;





        break;
    }       // switch case
  }         // else
}     // loop



void make_led_show(int led_show)
{
  switch (led_show) {
    case CIRCLE_ALL_LED:
      led_show_time = 25 + 0.562 * potiValue;
      currentTime = millis();
      if ( currentTime - lastTime_LED > led_show_time )
      {
        lastTime_LED = millis();

        counterLED++;
        if ( counterLED > 15 )
        {
          counterLED = 0;
        }

        //ground latchPin_LED and hold low for as long as you are transmitting
        digitalWrite(latchPin_LED, LOW);
        //move 'em out
        shiftOut(dataPin_LED, clockPin_LED, MSBFIRST, modus_led_output);
        shiftOut(dataPin_LED, clockPin_LED, LSBFIRST, led_show_array[1][CIRCLE_ALL_LED][counterLED]);
        shiftOut(dataPin_LED, clockPin_LED, LSBFIRST, led_show_array[0][CIRCLE_ALL_LED][counterLED]);
        //return the latch pin high to signal chip that it
        //no longer needs to listen for information
        digitalWrite(latchPin_LED, HIGH);

        break;
      }

    case FILL_UP_ALL_LED:
      led_show_time = 25 + 0.562 * potiValue;
      currentTime = millis();
      if ( currentTime - lastTime_LED > led_show_time )
      {
        lastTime_LED = millis();

        counterLED++;
        if ( counterLED > 15 )
        {
          counterLED = 0;
        }

        //ground latchPin_LED and hold low for as long as you are transmitting
        digitalWrite(latchPin_LED, LOW);
        //move 'em out
        shiftOut(dataPin_LED, clockPin_LED, MSBFIRST, modus_led_output);
        shiftOut(dataPin_LED, clockPin_LED, LSBFIRST, led_show_array[1][FILL_UP_ALL_LED][counterLED]);
        shiftOut(dataPin_LED, clockPin_LED, LSBFIRST, led_show_array[0][FILL_UP_ALL_LED][counterLED]);
        //return the latch pin high to signal chip that it
        //no longer needs to listen for information
        digitalWrite(latchPin_LED, HIGH);

        break;

      }

    case ALTERNATE_BLINKING_ITEMS:
      led_show_time = 25 + 0.562 * potiValue;
      currentTime = millis();
      if ( currentTime - lastTime_LED > led_show_time )
      {
        lastTime_LED = millis();

        counterLED++;
        if ( counterLED > 15 )
        {
          counterLED = 0;
        }

        //ground latchPin_LED and hold low for as long as you are transmitting
        digitalWrite(latchPin_LED, LOW);
        //move 'em out
        shiftOut(dataPin_LED, clockPin_LED, MSBFIRST, modus_led_output);

        // item blinken wenn aktiviert
        if ( item_blocked == 1 && blink_item_flag == 1 )
        {
          blink_item_flag = 0;
          shiftOut(dataPin_LED, clockPin_LED, MSBFIRST, 0);
        }
        else
        {
          blink_item_flag = 1;
          shiftOut(dataPin_LED, clockPin_LED, MSBFIRST, item_output);
        }

        shiftOut(dataPin_LED, clockPin_LED, LSBFIRST, led_show_array[0][led_show][counterLED]);
        //return the latch pin high to signal chip that it
        //no longer needs to listen for information
        digitalWrite(latchPin_LED, HIGH);

        break;

      }
    case ALTERNATE_BLINKING:
      led_show_time = 25 + 0.562 * potiValue;
      currentTime = millis();
      if ( currentTime - lastTime_LED > led_show_time )
      {
        lastTime_LED = millis();

        counterLED++;
        if ( counterLED > 15 )
        {
          counterLED = 0;
        }

        //ground latchPin_LED and hold low for as long as you are transmitting
        digitalWrite(latchPin_LED, LOW);
        //move 'em out
        shiftOut(dataPin_LED, clockPin_LED, MSBFIRST, modus_led_output);
        shiftOut(dataPin_LED, clockPin_LED, LSBFIRST, led_show_array[1][ALTERNATE_BLINKING][counterLED]);
        shiftOut(dataPin_LED, clockPin_LED, LSBFIRST, led_show_array[0][ALTERNATE_BLINKING][counterLED]);
        //return the latch pin high to signal chip that it
        //no longer needs to listen for information
        digitalWrite(latchPin_LED, HIGH);

        break;

      }

  }       // switch case
}           // function: make_led_show


void initialize_led_arrays()
{
  // CIRCLE_ALL
  led_show_array[0][0][0] = 0x28;
  led_show_array[0][0][1] = 0x09;
  led_show_array[0][0][2] = 0x81;
  led_show_array[0][0][3] = 0xC0;
  led_show_array[0][0][4] = 0x50;
  led_show_array[0][0][5] = 0x14;
  led_show_array[0][0][6] = 0x06;
  led_show_array[0][0][7] = 0x22;
  led_show_array[0][0][8] = 0x28;
  led_show_array[0][0][9] = 0x09;
  led_show_array[0][0][10] = 0x81;
  led_show_array[0][0][11] = 0xC0;
  led_show_array[0][0][12] = 0x50;
  led_show_array[0][0][13] = 0x14;
  led_show_array[0][0][14] = 0x06;
  led_show_array[0][0][15] = 0x22;

  led_show_array[1][0][0] = 0x82;
  led_show_array[1][0][1] = 0x06;
  led_show_array[1][0][2] = 0x44;
  led_show_array[1][0][3] = 0x0C;
  led_show_array[1][0][4] = 0x28;
  led_show_array[1][0][5] = 0x09;
  led_show_array[1][0][6] = 0x11;
  led_show_array[1][0][7] = 0x03;
  led_show_array[1][0][8] = 0x82;
  led_show_array[1][0][9] = 0x06;
  led_show_array[1][0][10] = 0x44;
  led_show_array[1][0][11] = 0x0C;
  led_show_array[1][0][12] = 0x28;
  led_show_array[1][0][13] = 0x09;
  led_show_array[1][0][14] = 0x11;
  led_show_array[1][0][15] = 0x03;


  // FILL_UP_ALL
  led_show_array[0][1][0] = 0x00;
  led_show_array[0][1][1] = 0x02;
  led_show_array[0][1][2] = 0x22;
  led_show_array[0][1][3] = 0x2A;
  led_show_array[0][1][4] = 0x2B;
  led_show_array[0][1][5] = 0xAB;
  led_show_array[0][1][6] = 0xEB;
  led_show_array[0][1][7] = 0xFB;
  led_show_array[0][1][8] = 0xFF;
  led_show_array[0][1][9] = 0xFF;
  led_show_array[0][1][10] = 0x00;
  led_show_array[0][1][11] = 0xFF;
  led_show_array[0][1][12] = 0x00;
  led_show_array[0][1][13] = 0xFF;
  led_show_array[0][1][14] = 0x00;
  led_show_array[0][1][15] = 0xFF;


  led_show_array[1][1][0] = 0x00;
  led_show_array[1][1][1] = 0x01;
  led_show_array[1][1][2] = 0x81;
  led_show_array[1][1][3] = 0x83;
  led_show_array[1][1][4] = 0xC3;
  led_show_array[1][1][5] = 0xC7;
  led_show_array[1][1][6] = 0xE7;
  led_show_array[1][1][7] = 0xEF;
  led_show_array[1][1][8] = 0xFF;
  led_show_array[1][1][9] = 0xFF;
  led_show_array[1][1][10] = 0x00;
  led_show_array[1][1][11] = 0xFF;
  led_show_array[1][1][12] = 0x00;
  led_show_array[1][1][13] = 0xFF;
  led_show_array[1][1][14] = 0x00;
  led_show_array[1][1][15] = 0xFF;


  // ALTERNATE_BLINKING_ITEMS
  led_show_array[0][2][0] = 0x65;
  led_show_array[0][2][1] = 0x9A;
  led_show_array[0][2][2] = 0x65;
  led_show_array[0][2][3] = 0x9A;
  led_show_array[0][2][4] = 0x65;
  led_show_array[0][2][5] = 0x9A;
  led_show_array[0][2][6] = 0x65;
  led_show_array[0][2][7] = 0x9A;
  led_show_array[0][2][8] = 0xBA;
  led_show_array[0][2][9] = 0xBE;
  led_show_array[0][2][10] = 0xFE;
  led_show_array[0][2][11] = 0xFF;
  led_show_array[0][2][12] = 0xF7;
  led_show_array[0][2][13] = 0xF5;
  led_show_array[0][2][14] = 0xE5;
  led_show_array[0][2][15] = 0x65;

  // ALTERNATE_BLINKING
  led_show_array[0][3][0] = 0x65;
  led_show_array[0][3][1] = 0x9A;
  led_show_array[0][3][2] = 0x65;
  led_show_array[0][3][3] = 0x9A;
  led_show_array[0][3][4] = 0x65;
  led_show_array[0][3][5] = 0x9A;
  led_show_array[0][3][6] = 0x65;
  led_show_array[0][3][7] = 0x9A;
  led_show_array[0][3][8] = 0x65;
  led_show_array[0][3][9] = 0x9A;
  led_show_array[0][3][10] = 0x65;
  led_show_array[0][3][11] = 0x9A;
  led_show_array[0][3][12] = 0x65;
  led_show_array[0][3][13] = 0x9A;
  led_show_array[0][3][14] = 0x65;
  led_show_array[0][3][15] = 0x9A;

  led_show_array[1][3][0] = 0xF0;
  led_show_array[1][3][1] = 0x0F;
  led_show_array[1][3][2] = 0xF0;
  led_show_array[1][3][3] = 0x0F;
  led_show_array[1][3][4] = 0xF0;
  led_show_array[1][3][5] = 0x0F;
  led_show_array[1][3][6] = 0xF0;
  led_show_array[1][3][7] = 0x0F;
  led_show_array[1][3][8] = 0xF0;
  led_show_array[1][3][9] = 0x0F;
  led_show_array[1][3][10] = 0xF0;
  led_show_array[1][3][11] = 0x0F;
  led_show_array[1][3][12] = 0xF0;
  led_show_array[1][3][13] = 0x0F;
  led_show_array[1][3][14] = 0xF0;
  led_show_array[1][3][15] = 0x0F;

}

