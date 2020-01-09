#include "IRremote.h"

// define the matrix
// andode (+)
#define ROW_1 A1 // Top of the tree, all yellow, only 4 lights
#define ROW_2 A2 // 2nd down
#define ROW_3 A3 // 3rd down
#define ROW_4 A4 // 4th down
const byte rows[] = {
  ROW_1, ROW_2, ROW_3, ROW_4
}; // array used to more easily reference the different rows in the matrix

// cathode (-)
#define COL_1 13   // green
#define COL_2 2   // red w/ yellow
#define COL_3 3   // green 
#define COL_4 4   // green
#define COL_5 5   // red  w/ yellow
#define COL_6 6   // green
#define COL_7 7   // green
#define COL_8 8   // red w/ yellow
#define COL_9 9   // green
#define COL_10 10 // green
#define COL_11 11 // red w/ yellow
#define COL_12 12 // green
const byte col[] = {
  COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8, COL_9, COL_10, COL_11, COL_12
}; // array used to more easily reference the different columns in the matrix


// define additional pins
const int receiver = A0;   // the number of the pushbutton pin


// define additional variables
unsigned long time;         // used for tracking time in loops
unsigned long endtime;      // used for tracking the end time for a loop
int seq = 0;                // used for tracking which sequence is currently active
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin
int speedTime = 500;        // used for setting the speed for changing between things in sequences

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

/*-----( Declare objects )-----*/
IRrecv irrecv(receiver);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'



// The display buffer

byte ALL[] = {B1111, B1111, B1111, B1111, B1111, B1111, B1111, B1111, B1111, B1111, B1111, B1111};
byte NONE[] = {B0000, B0000, B0000, B0000, B0000, B0000, B0000, B0000, B0000, B0000, B0000, B0000};
//byte RED_STAGGER_1[] = {B1111, B1010, B1111, B1111, B1101, B1111, B1111, B1010, B1111, B1111, B1101, B1111};
//byte RED_STAGGER_2[] = {B1111, B1101, B1111, B1111, B1010, B1111, B1111, B1101, B1111, B1111, B1010, B1111};
int RED_STAGGER_1[4][12] = {{1,1,1,1,1,1,1,1,1,1,1,1},{1,1,1,1,0,1,1,1,1,1,0,1},{1,0,1,1,1,1,1,0,1,1,1,1},{1,1,1,1,0,1,1,1,1,1,0,1}};
int RED_STAGGER_2[4][12] = {{1,1,1,1,1,1,1,1,1,1,1,1},{1,0,1,1,1,1,1,0,1,1,1,1},{1,1,1,1,0,1,1,1,1,1,0,1},{1,0,1,1,1,1,1,0,1,1,1,1}};
byte A[] = {B1000, B0000, B0000, B0000, B0000, B0000, B0000, B0000, B0000, B0000, B0000, B0000};
float timeCount = 0;

void setup() {
  // Open serial port
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  // Set all used pins to OUTPUT
  // This is very important! If the pins are set to input
  // the display will be very dim.
  for (byte i = 1; i <= 12; i++)
    pinMode(i, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
}

void loop() {

  {
    if (irrecv.decode(&results)) // have we received an IR signal?

    {
      interpretIR();
      Serial.println(results.value, HEX);
      irrecv.resume(); // receive the next value
    }
  }
  setSeq();
}

void interpretIR() {
  {

    switch (results.value)

    {
      case 0xFFA25D:
        if (seq == 0) {
          seq = 1;
        } else {
          seq = 0;
        }; break; // power
      case 0xFF6897: seq = 1;    break; // 0
      case 0xFF30CF: seq = 2;    break; // 1
      case 0xFF18E7: seq = 3;    break; // 2
      case 0xFF7A85: seq = 4;    break; // 3
      case 0xFF10EF: seq = 5;    break; // 4
      case 0xFF38C7: seq = 6;    break; // 5
      case 0xFF5AA5: seq = 7;    break; // 6
      case 0xFF42BD: seq = 8;    break; // 7
      case 0xFF4AB5: seq = 9;    break; // 8
      case 0xFF52AD: seq = 10;    break; // 9
      case 0xFF906F:
        if (speedTime  == 0) {
        } else if (speedTime - 100 <= 0) {
          speedTime = 4;
        } else if (speedTime <= 300) {
          speedTime = speedTime - 50;
        } else {
          speedTime = speedTime - 100;
        }
        break; // speed up the time between changes in sequences
      case 0xFFE01F:
        if (speedTime  == 0) {
        } else if (speedTime + 100 >= 1500) {
          speedTime = 1500;
        } else if (speedTime <= 300) {
          speedTime = speedTime + 50;
        } else {
          speedTime = speedTime + 100;
        }
        break; // slow down the time between changes in sequences
      case 0xFF9867:
        speedTime = 500;
        break;
    }// End Case

  }
}

void setSeq() {
  {
    switch (seq)
    {
      case 0: clearLights(); break;
      case 1: solidOn(); break;
      case 2: redBlink(); break;
      case 3: redStaggerBlink(); break;
      case 4: redGreenBlink(); break;
      case 5: redGreenYellowBlink(); break;
      case 6: chaseColumns(); break;
      case 7: doubleColumns(); break;
      case 8: chaseSingle(); break;
      case 9: chaseDouble(); break;
      case 10: crazyTwinkle(); break;
    }
  }
}

void solidOn() {
  green();
  delay(5);
  clearLights();
  yellow();
  delay(5);
  clearLights();
  red();
  delay(5);
  clearLights();
}

void redBlink() {
  time = millis();
  endtime = time;
  while ((endtime - time) <= speedTime) // do this loop for the first half of the sequence
  {
    green();
    delay(5);
    clearLights();
    yellow();
    delay(5);
    clearLights();
    red();
    delay(5);
    clearLights();
    endtime = millis();
  }
  while ((endtime - time) <= speedTime * 2) // do this loop for the second half of the sequence
  {
    green();
    delay(5);
    clearLights();
    yellow();
    delay(5);
    clearLights();
    endtime = millis();
  }
  endtime = time;
}

void redStaggerBlink() {
  time = millis();
  endtime = time;
  while ((endtime - time) <= speedTime) // do this loop for the first half of the sequence
  {
    //    green();
    //    delay(5);
    //    clearLights();
    //    yellow();
    //    delay(5);
    //    clearLights();
    lightTree(RED_STAGGER_1);
    clearLights();
    endtime = millis();
  }
  while ((endtime - time) <= speedTime * 2) // do this loop for the second half of the sequence
  {
    //    green();
    //    delay(5);
    //    clearLights();
    //    yellow();
    //    delay(5);
    //    clearLights();
    lightTree(RED_STAGGER_2);
    clearLights();
    endtime = millis();
  }
  endtime = time;
}

void redGreenBlink() {
  time = millis();
  endtime = time;
  while ((endtime - time) <= speedTime) // do this loop for the first half of the sequence
  {
    green();
    delay(5);
    clearLights();
    yellow();
    delay(5);
    clearLights();
    endtime = millis();
  }
  while ((endtime - time) <= speedTime * 2) // do this loop for the second half of the sequence
  {
    red();
    delay(5);
    clearLights();
    yellow();
    delay(5);
    clearLights();
    endtime = millis();
  }
  endtime = time;
}

void redGreenYellowBlink() {
  time = millis();
  endtime = time;
  while ((endtime - time) <= speedTime) // do this loop for the first half of the sequence
  {
    yellow();
    delay(5);
    clearLights();
    endtime = millis();
  }

  while ((endtime - time) <= speedTime * 2) // do this loop for the second half of the sequence
  {
    green();
    delay(5);
    clearLights();
    endtime = millis();
  }

  while ((endtime - time) <= speedTime * 3) // do this loop for the second half of the sequence
  {
    red();
    delay(5);
    clearLights();
    endtime = millis();
  }
  endtime = time;
}

void chaseSingle() {
  for (byte i = 0; i < 4; i++)
  {
    digitalWrite(rows[i], HIGH);
    for (byte q = 0; q < 12; q++)
    {
      digitalWrite(col[q], 0 & 0x01);
      delay(25);
      digitalWrite(col[q], 1 & 0x01);
    }
    delay(50);
    digitalWrite(rows[i], LOW);
  }
}

void chaseDouble() {
  for (byte i = 0; i < 4; i++)
  {
    digitalWrite(rows[i], HIGH);
    for (byte q = 0; q < 12; q++)
    {
      int  x = q + 6;
      if (q > 6) {
        x = q - 6;
      }
      digitalWrite(col[q], 0 & 0x01);
      digitalWrite(col[x], 0 & 0x01);
      delay(25);
      digitalWrite(col[q], 1 & 0x01);
      digitalWrite(col[x], 1 & 0x01);
    }
    delay(50);
    digitalWrite(rows[i], LOW);
  }
}

void crazyTwinkle() {
  for (byte i = 0; i < 4; i++)
  {
    digitalWrite(rows[i], HIGH);
    if (i + 2 > 3) {
      digitalWrite(rows[i - 2], HIGH);
    } else {
      digitalWrite(rows[i + 2], HIGH);
    }

    for (byte q = 0; q < 12; q++)
    {
      int  x = q + 6;
      if (q > 6) {
        x = q - 6;
      }
      digitalWrite(col[q], 0 & 0x01);
      digitalWrite(col[x], 0 & 0x01);
      delay(25);
      digitalWrite(col[q], 1 & 0x01);
      digitalWrite(col[x], 1 & 0x01);
    }
    delay(50);
    digitalWrite(rows[i], LOW);
  }
}

void chaseColumns() {
  for (byte i = 0; i < 4; i++)
  {
    digitalWrite(rows[i], HIGH);
  }
  for (byte q = 0; q < 12; q++)
  {
    digitalWrite(col[q], 0 & 0x01);
    delay(speedTime);
    digitalWrite(col[q], 1 & 0x01);
  }
}

void doubleColumns() {
  for (byte i = 0; i < 4; i++)
  {
    digitalWrite(rows[i], HIGH);
  }
  for (byte q = 0; q < 12; q++)
  {
    int  x = q + 6;
    if (q > 5) {
      x = q - 6;
    }
    digitalWrite(col[q], 0 & 0x01);
    digitalWrite(col[x], 0 & 0x01);
    delay(speedTime);
    digitalWrite(col[q], 1 & 0x01);
    digitalWrite(col[x], 1 & 0x01);
  }
}

// not currently used
void chaseRows() {
  for (byte i = 0; i < 4; i++)
  {
    digitalWrite(rows[i], HIGH);
    delay(25);
    digitalWrite(rows[i], LOW);
  }
  for (byte q = 0; q < 12; q++)
  {
    digitalWrite(col[q], 0 & 0x01);
    //  delay(25);
    //  digitalWrite(row[i], 1 & 0x01);
  }
}

void clearLights() {
  for (byte i = 0; i < 4; i++)
  {
    digitalWrite(rows[i], LOW);
  }
  for (byte q = 0; q < 12; q++)
  {
    digitalWrite(col[q], 1 & 0x01);
  }
}

void green() {
  digitalWrite(rows[0], LOW);
  digitalWrite(rows[1], HIGH);
  digitalWrite(rows[2], HIGH);
  digitalWrite(rows[3], HIGH);
  digitalWrite(col[0], 0 & 0x01);
  digitalWrite(col[1], 1 & 0x01);
  digitalWrite(col[2], 0 & 0x01);
  digitalWrite(col[3], 0 & 0x01);
  digitalWrite(col[4], 1 & 0x01);
  digitalWrite(col[5], 0 & 0x01);
  digitalWrite(col[6], 0 & 0x01);
  digitalWrite(col[7], 1 & 0x01);
  digitalWrite(col[8], 0 & 0x01);
  digitalWrite(col[9], 0 & 0x01);
  digitalWrite(col[10], 1 & 0x01);
  digitalWrite(col[11], 0 & 0x01);
}

void yellow() {
  digitalWrite(rows[0], HIGH);
  //digitalWrite(rows[1], LOW);
  //digitalWrite(rows[2], LOW);
  //digitalWrite(rows[3], LOW);
  //digitalWrite(col[0], 1 & 0x01);
  digitalWrite(col[1], 0 & 0x01);
  //digitalWrite(col[2], 1 & 0x01);
  //digitalWrite(col[3], 1 & 0x01);
  digitalWrite(col[4], 0 & 0x01);
  //digitalWrite(col[5], 1 & 0x01);
  //digitalWrite(col[6], 1 & 0x01);
  digitalWrite(col[7], 0 & 0x01);
  //digitalWrite(col[8], 1 & 0x01);
  //digitalWrite(col[9], 1 & 0x01);
  digitalWrite(col[10], 0 & 0x01);
  //digitalWrite(col[11], 1 & 0x01);
}

void red() {
  digitalWrite(rows[0], LOW);
  digitalWrite(rows[1], HIGH);
  digitalWrite(rows[2], HIGH);
  digitalWrite(rows[3], HIGH);
  digitalWrite(col[0], 1 & 0x01); // green
  digitalWrite(col[1], 0 & 0x01); // red
  digitalWrite(col[2], 1 & 0x01); // green
  digitalWrite(col[3], 1 & 0x01); // green
  digitalWrite(col[4], 0 & 0x01); // red
  digitalWrite(col[5], 1 & 0x01); // green
  digitalWrite(col[6], 1 & 0x01); // green
  digitalWrite(col[7], 0 & 0x01); // red
  digitalWrite(col[8], 1 & 0x01); // green
  digitalWrite(col[9], 1 & 0x01); // green
  digitalWrite(col[10], 0 & 0x01); // red
  digitalWrite(col[11], 1 & 0x01); // green

}



//byte RED_STAGGER_1[] = {B1111, B1010, B1111, B1111, B1101, B1111, B1111, B1010, B1111, B1111, B1101, B1111};
//byte RED_STAGGER_2[] = {B1111, B1101, B1111, B1111, B1010, B1111, B1111, B1101, B1111, B1111, B1010, B1111};
//byte RED[] = {B111111111111,
//              B101101101101,
//              B101101101101,
//              B101101101101};
//byte RED_STAGGER_1[] = {B111111111111,
//                        B111101111101,
//                        B101111101111,
//                        B111101111101};
//
//byte RED_STAGGER_2[] = {B111111111111,
//                        B101111101111,
//                        B111101111101,
//                        B101111101111};

// not currently used
void  lightTree(int buffer2[][])
{
  // Turn on each row in series
  for (int i = 0; i < 4; i++)        // count next row
  {
    digitalWrite(rows[i], HIGH);    //initiate whole row
    for (int a = 0; a < 12; a++)    // count next column
    {
      // if You set (~buffer2[i] >> a) then You will have positive
      digitalWrite(rows[a], (buffer2[i] >> a) & 0x01); // initiate whole column

      delayMicroseconds(100);       // uncoment delay for diferent speed of display
      //delayMicroseconds(1000);
      //delay(10);
      //delay(100);

      digitalWrite(rows[a], 1 & 0x01);      // reset whole column
    }
    digitalWrite(col[i], LOW);     // reset whole row
    // otherwise last row will intersect with next row
  }
}


//// not currently used
//void  lightTree(byte buffer2[])
//{
//  // Turn on each column in series
//  for (byte i = 0; i < 12; i++)        // count next column
//  {
//    digitalWrite(col[i], HIGH);    //initiate whole row
//    for (byte a = 0; a < 4; a++)    // count next row
//    {
//      // if You set (~buffer2[i] >> a) then You will have positive
//      digitalWrite(rows[a], (buffer2[i] >> a) & 0x01); // initiate whole column
//
//      delayMicroseconds(100);       // uncoment delay for diferent speed of display
//      //delayMicroseconds(1000);
//      //delay(10);
//      //delay(100);
//
//      digitalWrite(rows[a], 1 & 0x01);      // reset whole column
//    }
//    digitalWrite(col[i], LOW);     // reset whole row
//    // otherwise last row will intersect with next row
//  }
//}
