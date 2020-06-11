# Xpressnet Manual Control

The XpressNet Manual Control (Xmc) is manual control for the [XPressnet bus](www.lenzusa.com/1newsite1/Manuals/xpressnet.pdf) based on 

 * An [Maple Mini STM32 board](https://www.leaflabs.com/maple/) variant from China
 * RS485 driver IC
 * AT24C256 memory IC for storage of settings and locomotive data.
 * 128*128 pixel ST7735 LCD. 
 * Push buttons for controlling functions / accesory decoders / menu related actions. 
 * Pulse switch for setting speed / menu related actions.
 
The user interface is nearly the same as with the WMC application and it's tried to reuse / update library's created for WMC.

## Tested with
 * [MDRRC-II](http://members.home.nl/robert.evers/mdrrc2.htm) Response ok. 
 * [Roco MultiMaus as master](https://www.roco.cc/en/product/5215-multimaus-0-0-0-0-0-004001-0/products.html) OK for locs, turnouts not tested yet.

![](https://github.com/MDRRC/XMC/blob/master/Doc/xmc_roco.jpg) 
 
## Schematic
The [schematic](https://github.com/MDRRC/XMC/blob/master/Doc/xmc_schematic.pdf) of the XMC.

## PCB front and back side.
The PCB from the front and back. 
![](https://github.com/MDRRC/XMC/blob/master/Doc/xmc_pcb_front_1.jpg)

![](https://github.com/MDRRC/XMC/blob/master/Doc/xmc_pcb_back.jpg)

## Programming the STM32 
The STM32 can be programmed without the need to build the software. By using a programming tool from the STM32 Arduino library the STM32 can be programmed with the buildin bootloader of the STM32 on the USB connection.
 * The [Stm32 Arduino library](https://github.com/rogerclarkmelbourne/Arduino_STM32) must be present on your PC.
 * Open the DOS box and enter command below (Location of files on your own PC might differ and the COM port will probably also be different, so change it accordingly!!)
 > C:/Arduino_STM32-master/tools/win/maple_upload.bat COM9 1 1EAF:0003 C:/LocationOfBinFileOnYourPc/Xmc.bin
 * The bin file is flashed into the STM32.
 * When ready, the screen stays gray for ~3 seconds and subsequent the start screen appears.
 
## Programming the STM32 with FLASHER-STM32
The STM32 can also be programmed with [Flasher-STM32](https://www.st.com/en/development-tools/flasher-stm32.html)
 * Connect an USB serial to TTL level (!) adapter to RX1 and TX1 (pin 25 and 26). The USB connector can NOT be used!!! 
 * Connect an USB cable to the USB connector of the STM32 module so it has supply voltage when programming the STM32 out out the XMC PCB. 
 * Connect the BOOT1 pin to ground when programming the STM32 out out the XMC PCB. 
 * Follow instructions of chapter 5. Software update of [MDRRC-II Lite](https://github.com/MDRRC/Doc/blob/master/mdrrc_ii_lite.zip)
 * Select the xmc.bin and flash the file.
 * When ready press RESet button, the screen stays gray for ~3 seconds and subsequent the start screen appears.   
 
## Used library's
If you want to build the code yourself or update / change the code following library's are required.
 * [XmcApp](https://github.com/MDRRC/XmcApp) the main state machine of the application.
 * [TinyFsm](https://github.com/digint/tinyfsm) for state machine framework.
 * [XpressNet](https://github.com/MDRRC/XpressNet) for XpressNet protocol.
 * [Bounce2](https://github.com/thomasfredericks/Bounce2) for debouncing switches.
Bounce requires an update for use in the XMC application, add the include and replace the attach function with code below
```c
#include "app_cfg.h"
 
void Bounce::attach(int pin, int mode)
{
  #ifdef MCU_STM32F103RB
  pinMode(pin, (WiringPinMode)(mode));
  #else
  pinMode(pin, mode);
  #endif
  this->attach(pin);
}
```
 * [Adafruit-ST7735-Library](https://github.com/adafruit/Adafruit-ST7735-Library) to control the display.
 * [WmcTft](https://github.com/MDRRC/WmcTft) for graphics within the XMC application.
 * [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library) for graphic control of the display.
 * [LocLib](https://github.com/MDRRC/LocLib) for handling locomotive data within the XMC application.
 * [WmcCv](https://github.com/MDRRC/WmcCv) for changing CV's.
 * [WmcCli](https://github.com/MDRRC/WmcCli) for the command line interface over the USB serial port.
 * [Stm32 Arduino library](https://github.com/rogerclarkmelbourne/Arduino_STM32) 
 
 In the Stm32 Arduino library
 * Remove or rename (.x for example) ..\Arduino_STM32-master\STM32F1\cores\maple\libmaple\usart.c so it's NOT build 
 * Remove or rename (.x for example) ..\Arduino_STM32-master\STM32F1\system\libmaple\usart_private.h so it's NOT build 
 
 Both files are in [XpressNet](https://github.com/MDRRC/XpressNet)