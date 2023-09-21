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
//   0  1  2
//
//  |-----------|----------------|
//  | TSOP pins | Digispark pins |
//  |-----------|----------------|
//  | 0 : Data  |    PB2         |
//  | 1 : GND   |    GND         |
//  | 2 : VCC   |    5V          |
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

// intervall to move mouse
#define JIGGLE_INTERVALL 30000

// download Adafruit-Trinket library from:
// https://github.com/adafruit/Adafruit-Trinket-USB
#include "TrinketHidCombo.h"

uint8_t situation = 0;
uint8_t START = 0;
uint8_t x = 0;
uint8_t BIT = 0;
uint8_t Id = 0;
uint8_t Id_inv = 0;
uint8_t Data = 0;
uint8_t Data_back = 0;
uint8_t Data_inv = 0;
uint8_t Repeat = 0;
uint8_t sended = 0;
uint16_t Time_old = 0;
uint16_t Time = 0;
uint16_t TimeDelta = 0;
unsigned long Last_jiggle = 0;

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
  // if no IR data - poll USB and
  // check if mouse needs to be moved
  else {
    TrinketHidCombo.poll();
    unsigned long CurrentMillis = millis();
    if (CurrentMillis - Last_jiggle >= JIGGLE_INTERVALL) {
      TrinketHidCombo.mouseMove(2, 0, 0);
      delay(50);
      TrinketHidCombo.mouseMove(-2, 0, 0);
      last_jiggle = CurrentMillis;
    }
  }
}

// Read the IR code
// this code is from laurens.wuyts@gmail.com
// https://www.instructables.com/DIY-USB-IR-receiver/
void IR_Read(void) {
  digitalWrite(1, HIGH);
  Time = micros();
  if (Time_old != 0) {
    TimeDelta = Time - Time_old;
    if ((TimeDelta > 12000) && (TimeDelta < 14000)) {
      START = 1;
      x = 0;
      situation = 1;
      Id = 0;
      Id_inv = 0;
      Data = 0;
      Data_inv = 0;
    } else if ((TimeDelta > 10000) && (TimeDelta < 12000)) {
      situation = 2; // repeat
    } else if ((TimeDelta > 1500) && (TimeDelta < 2500)) {
      situation = 3; //"1"
      BIT = 1;
    } else if ((TimeDelta > 1000) && (TimeDelta < 1500)) {
      situation = 3; //"0"
      BIT = 0;
    } else situation = 5;
    if (situation == 3) {
      if (x < 8) {
        Id |= BIT;
        if (x < 7) Id <<= 1;
        x++;
      } else if (x < 16) {
        Id_inv |= BIT;
        if (x < 15) Id_inv <<= 1;
        x++;
      } else if (x < 24) {
        Data |= BIT;
        if (x < 23) Data <<= 1;
        x++;
      } else if (x < 32) {
        Data_inv |= BIT;
        if (x < 31) {
          Data_inv <<= 1;
        } else {
          // DO SOMETHING HERE
          sended = 1;
          Data_back = Data;
          Repeat = 0;
        }
        x++;
      }
    } else if (situation == 2) {
      if (Repeat == 1) {
        // DO SOMETHING HERE
        sended = 1;
      } else {
        Repeat = 1;
      }
    }
  }
  Time_old = Time;
  digitalWrite(1, LOW);
}
