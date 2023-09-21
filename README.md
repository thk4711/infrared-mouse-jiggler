# infrared-mouse-jiggler
### Use-Case

1. If you want to prevent your computer from beeing locked automatically and you do not have the nessesary rights to deactivate that on OS level, you can plug in this mouse jiggler and it will show up in the OS as a USB input device. It will regulary move the mouse a little bit back anf fore.
2. You listen to music or watch videos on your PC but you are not sitting next to it. With this device you can use any infrared remote to control things like volume and skip tracks.

### Hardware

The Hardware used in this project is a Digispark. This a a little board with an [ATTINY85](https://www.microchip.com/en-us/product/attiny85) microcontroller with a special bootloader. You can buy that for instance on Amazon eBay or Aliexpress. There are many variants with different USB connectors. The one I am using has a USB connector as part of the circuit board.

The only additional component needed is an infrared receiver. You can buy that as well on Amazon eBay or Aliexpress. Just search for "infrared TSOP 38khz". 

Connect DATA to P2, GND to GND and Vs to 5V of the Digispark.

```
                   ┌───────────────────────┐       ┌───────┐
                   │    Digispark        . │       │  .─.  │
                   │                  P5( )│       │ (   ) │  1: DATA
   ┌───────────────┤                     . │       │  `─'  │  2: GND
   │  ┌───────────┐│                  P4( )│       │  TSOP │  3: Vs
   │  └───────────┘│                     . │       └─┬─┬─┬─┘
   │    ┌─────────┐│                  P3( )│         1 2 3
   │    └─────────┘│                     . │         │ │ │
   │    ┌─────────┐│                  P2( )├─────────┘ │ │
   │    └─────────┘│                     . │           │ │
   │  ┌───────────┐│                  P1( )│           │ │    
   │  └───────────┘│   5V GND VIN        . │           │ │
   └───────────────┤   .   .   .      P0( )│           │ │
                   │  ( ) ( ) ( )        ' │           │ │
                   │   '   '   '           │           │ │
                   └───┬───┬───────────────┘           │ │
                       │   └───────────────────────────┘ │
                       └─────────────────────────────────┘
```

For a more detailed description please have a look at https://www.instructables.com/DIY-USB-IR-receiver/ .

### Arduino IDE configuration

Add the Digisparc board support under Preferences->Additional Boards Manager URLs: https://raw.githubusercontent.com/sparkfun/Arduino_Boards/main/IDE_Board_Manager/package_sparkfun_index.json

Before you upload your sketch, choose the right board under Tools->Board->Digistump AVR Boards->Digispark (Default 16.5 mhz).

More details can be found here: https://www.instructables.com/Digispark-Attiny-85-With-Arduino-IDE/

### Add Adafruit-Trinket library

The sketch is using the the Adafruit TrinketHidCombo library. You can get it from [GitHub](https://github.com/adafruit/Adafruit-Trinket-USB/tree/master). On the page press "Code" button and choose download as zip. 

Now unpack the zip file and copy the TrinketHidCombo directory to your Arduino library directory. 

More details can be found here: https://support.arduino.cc/hc/en-us/articles/5145457742236-Add-libraries-to-Arduino-IDE

### Customize the sketch for your remote

The sketch in this repo is using the infrared codes for an Apple remote. If you want to use a different remote or if you want the remote to do different things you have to change that in the sketch.

To get a list of things the Adafruit-TrinketHidCombo library can do have a look at: https://github.com/adafruit/Adafruit-Trinket-USB/blob/master/TrinketHidCombo/TrinketHidCombo.h Most interresting things are:

- mouseMove
- pressKey
- typeChar
- pressMultimediaKey
- pressSystemCtrlKey
- print
- println

##### To find out the IR codes of your remote please do the following:

1. Comment out the following line in the sketch:
```
TrinketHidCombo.println(Data_back, HEX);
```

3. Flash the sketch onto the Digispark.

4. Open an app which does accept text input like and editor or word processor.

5. Press the buttons on your remote. The HEX codes are written to your app if one button leeds to 2 codes usually you want to use the first code.

6. Edit the following section in the sketch:

```
// IR codes for your Remote control (this example is for an Apple remote)
// change this section to match your remote
#define VOL_UP    0xE0
#define VOL_DOWN  0x10
#define NEXT      0xD0
#define PREV      0xB0
#define MUTE      0xBA
#define MENU      0x40
#define PLAYPAUSE 0x7A
```

   The format is like `#define Button_Name IR_Code` where Button_Name is something you choose and IR_Code is the code for the button you found out in step 3. Reped that for all buttons on your remote you want to use.

6. Mapp actions to you buttons by editing the following part in the sketch.

   ```
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
   ```
7. If everything is working - again comment out this line:
```
// TrinketHidCombo.println(Data_back, HEX);
```

9. Flash the sketch onto the Digispark.

If you need to change the intervall of mouse moovements, change this line in the sketch:
```
#define JIGGLE_INTERVALL 30000
```

The unit of the number is milliseconds.  
