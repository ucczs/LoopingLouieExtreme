

//////////// LED SCHIEBEREGISTER /////////////////
//Pin connected to ST_CP of 74HC595 (LATCH)
const int latchPin_LED = 6;
//Pin connected to SH_CP of 74HC595 (CLOCK)
const int clockPin_LED = 7;
////Pin connected to DS of 74HC595 (DATA)
const int dataPin_LED = 8;

/////////// TASTER SCHIEBEREGISTER //////////////////
const int dataPin_Taster = 9;
const int latchPin_Taster = 10;
const int clockPin_Taster = 11;

const int itemTaster_pin = 12;
int item_output = B11111111;
int item_taster_value = 0;
int shift_value = 1;

int item_blocked = 0;
int debounce_item = 3000;
int blink_item_flag = 0;
unsigned long int current_time_item = 0;


unsigned long int currentTime = 0;
unsigned long int lastTime_LED = 0;
int led_show_time = 200;

int led_answer = 255;

void setup()
{

  pinMode(latchPin_LED, OUTPUT);
  pinMode(clockPin_LED, OUTPUT);
  pinMode(dataPin_LED, OUTPUT);

  pinMode(latchPin_Taster, OUTPUT);
  pinMode(clockPin_Taster, OUTPUT);
  pinMode(dataPin_Taster, OUTPUT);

  pinMode(itemTaster_pin, INPUT);

}

void loop() {


  currentTime = millis();

  // Taster alle auf High setzen
  digitalWrite(latchPin_Taster, LOW);
  shiftOut(dataPin_Taster, clockPin_Taster, MSBFIRST, 255);
  digitalWrite(latchPin_Taster, HIGH);

  item_taster_value = digitalRead(itemTaster_pin);


  ///////////////// TASTER CHECK ////////////////////////////
  if ( item_taster_value == HIGH && item_blocked == 0)
  {
    shift_value = 1;

    for (int i = 0; i < 8; i++)
    {
      // led_answer = led_answer ^ B11111111;

      digitalWrite(latchPin_Taster, LOW);
      shiftOut(dataPin_Taster, clockPin_Taster, MSBFIRST, shift_value);
      digitalWrite(latchPin_Taster, HIGH);

      delayMicroseconds(500);

      item_taster_value = digitalRead(itemTaster_pin);

      if ( item_taster_value == HIGH )
      {
        // prüfen ob Item schon verbraucht
        // bsp. item_output: B10110001
        // j = 4
        // 1 << 4 =         B00010000
        // ver-&          = B00010000
        // > 0
        if ( (item_output ^ (1 << i) ) > 0 )
        {
          // turn light off
          switch (i)
          {
            case 0:         // red button
              item_output = item_output & B11101111;
              break;
            case 1:         // green button
              item_output = item_output & B11111011;
              break;
            case 2:         // green button
              item_output = item_output & B11110111;
              break;
            case 3:         // red button
              item_output = item_output & B01111111;
              break;
            case 4:         // red button
              item_output = item_output & B11011111;
              break;
            case 5:         // green button
              item_output = item_output & B11111101;
              break;
            case 6:         // green button
              item_output = item_output & B11111110;
              break;
            case 7:         // red button
              item_output = item_output & B10111111;
              break;
          }
          item_blocked = 1;
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
  }


  //////////// LED SHOW ///////////////////////
  currentTime = millis();

  if ( currentTime - lastTime_LED > led_show_time )
  {
    lastTime_LED = millis();

    digitalWrite(latchPin_LED, LOW);

    shiftOut(dataPin_LED, clockPin_LED, MSBFIRST, led_answer);

    // item blinken wenn aktiviert
    if( item_blocked == 1 && blink_item_flag == 1 )
    {
      blink_item_flag = 0;
      shiftOut(dataPin_LED, clockPin_LED, MSBFIRST, 0);
    }
    else 
    {
      blink_item_flag = 1;
      shiftOut(dataPin_LED, clockPin_LED, MSBFIRST, item_output);
    }
    
    shiftOut(dataPin_LED, clockPin_LED, LSBFIRST, 255);

    digitalWrite(latchPin_LED, HIGH);

  }


}
