#Xpressnet Manual Control

The XpressNet Manual Control (Xmc) is manual control for the [XPressnet bus](www.lenzusa.com/1newsite1/Manuals/xpressnet.pdf) based on 

 * a [Maple Mini STM32 board](https://www.leaflabs.com/maple/) variant from China
 * RS485 driver IC
 * AT24C256 memeory IC for storage of settings and locdata.
 * Switches for controlling functions. 
 * Pulse switch for setting speed.
 
The user interface is nearly the same as with the WMC application and it's tried to reuse as many as possible library's created for WMC.

## Tested with
 * [MDRRC-II](http://members.home.nl/robert.evers/mdrrc2.htm) OK
 * [Roco MultiMaus as master](https://www.roco.cc/en/product/5215-multimaus-0-0-0-0-0-004001-0/products.html) To be tested
 
## Below the development setup.

![](https://github.com/MDRRC/XMC/blob/master/xmc.jpg)

## Used librarys
 * [XmcApp]() the main state machine of the application.
 * [TinyFsm](https://github.com/digint/tinyfsm) for state machine framework.
 * [XpressNet](https://github.com/MDRRC/XpressNet) for XpressNet protocol.
 * [Bounce2](https://github.com/thomasfredericks/Bounce2) for debouncing switches.
Bounce requires an update for use in the XMC application, add the include and replace the attach function with code below
...
#include "app_cfg.h"
 
 void Bounce::attach(int pin, int mode){
#ifdef MCU_STM32F103RB
  pinMode(pin, (WiringPinMode)(mode));
#else
  pinMode(pin, mode);
#endif
  this->attach(pin);
}
...
 * [Adafruit-ST7735-Library](https://github.com/adafruit/Adafruit-ST7735-Library) to control the display.
 * [WmcTft](https://github.com/MDRRC/WmcTft) for graphics within the Wmc application.
 * [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library) for graphic control of the display.
 * [LocLib](https://github.com/MDRRC/LocLib) for handling locomotive data within the Wmc application.
 * [WmcCv](https://github.com/MDRRC/WmcCv) for changing CV's.
 * [WmcCli](https://github.com/MDRRC/WmcCli) for the command line interface over the USB serial port.
 * [Stm32 Arduino library](https://github.com/rogerclarkmelbourne/Arduino_STM32) 


More info will follow soon!    