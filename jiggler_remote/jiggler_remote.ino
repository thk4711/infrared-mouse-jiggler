// This code will do 2 things:
// 1. read an infrared remote control and performe USB keyboard
//    or mouse actions
// 2. move the mouse regulary just a little to prevent your computer
//    from beeing locked
//
// Add this board definition to Aruino IDE:
// http://digistump.com/package_digistump_index.json
// Infrared receiver soldered to Digispark
//
//   |-----|
//   | ( ) |
//   |-----|
//   |  |  |
//   |  |  |
//   1  2  3
//
//  |-----------|----------------|
//  | TSOP pins | Digispark pins |
//  |-----------|----------------|
//  | 1 : Data  |    PB2         |
//  | 2 : GND   |    GND         |
//  | 3 : VCC   |    5V          |
//  |-----------|----------------|

// IR codes for your Remote control (this example is for an Apple remote)
// change this section to match your remote
#define VOL_UP    0xE0
#define VOL_DOWN  0x10
#define NEXT      0xD0
#define PREV      0xB0
#define MUTE      0xBA
#define MENU      0x40
#define PLAYPAUSE 0x7A

// intervall to move mouse in ms
#define JIGGLE_INTERVAL 30000

// download Adafruit-Trinket library from:
// https://github.com/adafruit/Adafruit-Trinket-USB
#include "TrinketHidCombo.h"

unsigned long Last_jiggle = 0;

uint8_t STEP = 0;
uint8_t BIT = 0;
uint8_t Id = 0;
uint8_t Id_inv = 0;
uint8_t Data = 0;
uint8_t Data_inv = 0;
uint8_t Data_back = 0;
uint8_t sended = 0;
uint32_t Time_old = 0;
uint32_t Time = 0;
uint32_t TimeDelta = 0;

void setup(void) {
  // Use INT0(P2) on the Digispark
  attachInterrupt(0, IR_Read, FALLING);
  // optional LED which will blink if IR signal is detected
  pinMode(1, OUTPUT);
  digitalWrite(1, LOW);
  TrinketHidCombo.begin();
}

void loop(void) {
  if (sended == 1) {
    // uncomment the line below to find the IR codes for a remote
    // go to a text editor end whenever a remote signal is detected it will be
    // send with the keyboard emulation to your computer
    // for production use you have to remove this

    // TrinketHidCombo.println(Data_back, HEX);

    // what to do when a certain IR code is detected
    // for a list of thing which can be done please look at:
    // https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidCombo.h

    if(Data_back == VOL_UP) {
      TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_UP); }
    else if(Data_back == VOL_DOWN) {
      TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_DOWN); }
    else if(Data_back == NEXT) {
      TrinketHidCombo.pressMultimediaKey(MMKEY_SCAN_NEXT_TRACK); }
    else if(Data_back == PREV) {
      TrinketHidCombo.pressMultimediaKey(MMKEY_SCAN_PREV_TRACK); }
    else if(Data_back == MUTE) {
      TrinketHidCombo.pressMultimediaKey(MMKEY_MUTE); }
    else if(Data_back == PLAYPAUSE) {
      TrinketHidCombo.pressMultimediaKey(MMKEY_PLAYPAUSE); }
    else if(Data_back == MENU) {
      TrinketHidCombo.write(char(32)); }
    sended = 0;
    }
    else if (sended == 2) { // Repetition
      // Control keys that you want to have repetitions
      if(Data_back == VOL_UP) {
        TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_UP); }
      else if(Data_back == VOL_DOWN) {
        TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_DOWN); }
      sended = 0;
      }
  // if no IR data - poll USB and
  // check if mouse needs to be moved
  else {
    TrinketHidCombo.poll();
    unsigned long CurrentMillis = millis();
    if (CurrentMillis - Last_jiggle >= JIGGLE_INTERVAL) {
      TrinketHidCombo.mouseMove(2, 0, 0);
      delay(50);
      TrinketHidCombo.mouseMove(-2, 0, 0);
      Last_jiggle = CurrentMillis;
    }
  }
}

// Read the IR code
// this code is from laurens.wuyts@gmail.com
// https://www.instructables.com/DIY-USB-IR-receiver/
void IR_Read(void) {
  Time = micros();
  TimeDelta = Time - Time_old;

  if (TimeDelta > 100000 || (TimeDelta > 12500)&&(TimeDelta < 14500)) {
    STEP = 0;
    }
  STEP++;
  if (STEP == 1) {
    if ((TimeDelta > 12500)&&(TimeDelta < 14500)) {
      Id = 0;
      Id_inv = 0;
      Data = 0;
      Data_inv = 0;
      }
    else {
      STEP = 0;
      }
    }
  else if((STEP>=2) && (STEP<34)) { //Accumulate the bit values between 0-31.
    if ((TimeDelta > 1750)&&(TimeDelta < 2750)) BIT = 1;
    else if ((TimeDelta > 625)&&(TimeDelta < 1625)) BIT = 0;
    else STEP = 0;
  if (STEP < 10) {
    Id |= BIT;
    if (STEP < 9) Id <<= 1;
    }
  else if (STEP < 18) {
    Id_inv |= BIT;
    if (STEP < 17) Id_inv <<= 1;
    }
  else if (STEP < 26) {
    Data |= BIT;
    if (STEP < 25) Data <<= 1;
    }
  else {
    Data_inv |= BIT;
    if (STEP < 33) Data_inv <<= 1;
    else {
      if (Data^Data_inv == 0xFF && Id^Id_inv == 0xFF) {
        Data_back = Data;
        sended = 1;
        }
      else {
        STEP = 0;
        }
      }
    }
  }
  else if ((TimeDelta > 40100)&&(TimeDelta < 42100)) {} // Ignore the First repetition block
  else if ((TimeDelta > 10250)&&(TimeDelta < 12250)) {} // Ignore Begin of new repetition block
  else if ((TimeDelta > 95750)&&(TimeDelta < 110750)) // End of a repetition block
    {
    // Repeat
    sended = 2;
    }
  else {
    STEP = 0;
    }
  Time_old = Time;
  }
