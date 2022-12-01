# SMBeeFirmware

# Quick start using demo firmware
A pre-built firmware binary [demo.hex] is provided for easy installation. This is built from the core firmware and a demo [program.c] LED sequence.

This section covers:

* Installation of the [AVRDUDE] programming utility
* Downloading the [demo.hex] demo firmware
* Programming an [SMBee] using the [SMBeeHive] programmer

## AVRDUDE programming utility

[AVRDUDE] is a utility for programming AVR microcontrollers using popular programing devices including the USBASP used in the custom SMBeeHive programmer. 

Install the current **version 6.3** or later, this is available for OSX, Linux and Windows:

### OSX install

Install using [Homebrew package manager]:
~~~bash
brew upgrade
brew install avrdude
~~~

### Linux install

Install using `apt` Advanced Package Tool, usually pre-installed on popular linux distributions:

~~~bash
sudo apt update
sudo apt upgrade
sudo apt install avrdude
~~~

To avoid having to superceed `avrdude` commands with `sudo`. You must set up some rules on the USB port regarding the USBASP device.

From your 'SMBeeFirmware' directory:
~~~bash
cd bin/linux-nonroot
. install_rule
~~~

<!---</br>`sudo apt upgrade all`--->

### Windows install

There isn't a stand-alone installer for windows with a suitable version of AVRDUDE.

Currently the best option is to install [WinAVR]. and manually update its version of avrdude:

1. Download and install [WinAVR]. Install in the default location.
1. Download and unpack AVRDUDE 6.3 from the [AVRDUDE download page].
1. Copy the unpacked files (`avrdude.exe` & `avrdude.conf`) into `C:\WinAVR-20100110\bin`, overwriting the existing versions.
1. Install libusbK as a USBASP driver:
   * Download and install the [Zadig usb driver install tool].
   * Plug the USBASP into a USB port and run `Zadig`.
   * Select the `USBasp` from the combo box and `libusbK` as the driver.

Note: `avrdude.exe` depends on external libraries from the WinAVR installation so it can't be installed in isolation.

### Verify the avrdude installation

To verify the installed version enter:
~~~bash
avrdude
~~~

This should report version 6.3 or later.

### Get a copy of demo.hex

First get a copy of [demo.hex], either: 
* Download directly from the GitHub web page [demo.hex.zip] and unzip it. 
* Clone the repository locally. See **Downloading demo.hex from GitHub** below.

### Programme [demo.hex] using AVRDUDE

Connect the SMBee with the SMBeeHive programmer to the AVRDUDE host machine.

The first time the SMBee MCU is programmed you will need to program its `RSTIDSBL` fuse, to disable the ATtiny10 reset pin functionality. Fuses are configuration bits separate from the flash memory, retained through a flash memory clear. Once the Fuse is programmed, this step can be omitted when reprogramming the main flash memory to modify the firmware.

~~~bash
avrdude -v -p attiny10 -c usbasp -U fuse:w:0xfe:m
~~~
In response to the command programming information and progress information will be outputted to the terminal which should finnish with `avrdude: 1 bytes of fuse verified`.

Now programme the main flash memory assuming [demo.hex] is in the current directory:

~~~bash
avrdude -v -p attiny10 -c usbasp -U flash:w:demo.hex:i
~~~
This should complete with a message reporting successful verification of the flashed memory, for example: 

"avrdude: 1006 bytes of flash verified".

It is possible to combine these two steps into:
~~~bash
avrdude -v -p attiny10 -c usbasp -U fuse:w:0xfe:m -U flash:w:demo.hex:i
~~~

# Downloading [demo.hex] from GitHub

If you are on a platform without a web browser or unzip easiest way is to clone the complete repository into your local file-space:

If you don't have it, install the Git software configuration manager:

Target | install
-|-
Linux | sudo apt install git
OSX | brew install git
All | [download git]

Change your default directory to a suitable location.

Get a local copy of the repository:
~~~bash
git clone https://github.com/milelo/SMBeeFirmware.git
~~~

The file location is `SMBeeFirmware/bin/demo.hex`. 

~~~bash
cd SMBeeFirmware
avrdude -v -p attiny10 -c usbasp -U fuse:w:0xfe:m -U flash:w:bin/demo.hex:i
~~~

If the machine you are downloading to has a web browser it is possible to download the file directly from GitHub however it isn't straightforward. Select the Raw display option for the file than use your browsers save-as function to specify the download location and file-name.

# Building the firmware

To modify flash sequence of the SMBee or other aspects of the firmware you will need to install the avr-gcc toolchain and the VSCode IDE to build and edit the firmware in your local repository clone. 

## OSX toolchain install

Use the [Homebrew package manager] to install the avr-gcc toolchain you will need to build the firmware. Install version 8. I had problems compiling for the ATtiny10 on the current version 9.

From [homebrew-avr]:

~~~bash
xcode-select --install
brew tap osx-cross/avr
brew install avr-gcc@8
~~~

## Linux avr-gcc toolchain install

This is the current status building the firmware under linux with gcc-avr=1:5.4.0+Atmel3.6.1-2:
* **Debian 10 (buster)** - builds **OK**.
* **Raspbian GNU/Linux 10 (buster)** - build **OK**.
* **Ubuntu 18.04 LTS** - build Ok but generates the wrong code. **Do not use**, program size is reported as 1138 bytes (111.1% Full).


### Setup the toolchain:

~~~bash
sudo apt update
sudo apt upgrade
sudo apt install make git wget
sudo apt install gcc-avr avr-libc
~~~

Check the installed version:

~~~bash
avr-gcc --version
~~~

Currently reporting: 

`avr-gcc (GCC) 5.4.0`.

## Windows

I haven't found an avr-gcc toolchain installation for windows that supports the ATtiny10 MCU however the Windows 10 feature [WSL], Windows Subsystem for Linux allows Linux distributions to be run under Windows which support development for the device. There are some limitations with the current version of WSL and USB support is one of them. The firmware can be built under WSL but the resulting binary must be uploaded to the bee from Windows as described above for the demo firmware, this is fairly straight forward once everything is set up because the Windows file system is shared with WSL. WSL-2, a new version of WSL is being developed by Microsoft to overcome the WSL-1 deficiencies, this will probably be formally release around April 2020.

### Windows 10 WSL toolchain setup

To setup a WSL development system under Windows 10 using VSCode:

1. Run windows update to ensure you have an up to date version of Windows 10.
1. Install VSCode for windows: [VSCode installers]. Don't leave VSCode open.
1. Clone the firmware repository in a suitable location in your windows file system. 
<br/> `git clone https://github.com/milelo/SMBeeFirmware.git`
<br/> `cd SMBeeFirmware`
1. Install WSL as described [here][install WSL]. I've tested the build under the following distribtions:
    * **Debian 10 (buster)** - builds **OK**.
    * **Ubuntu 18.04 LTS** - build Ok but generates the wrong code. **Do not use**, program size is reported as 1138 bytes (111.1% Full).
1. Start WSL:
<BR/>`wsl`
<BR/>The command prompt will change to that of the linux distribution.
1. Setup the linux distribution:
<BR/>`sudo apt update`
<BR/>`sudo apt upgrade`
<BR/>`sudo apt install make git wget`
<BR/>`sudo apt install gcc-avr avr-libc`
1. Start VSCode from the same window, you should still be in the Linux 'SMBeeFirmware' directory:
<BR/>`code .`
<BR/>This will install a remote VSCode server and start VSCode running under Windows, the VSCode terminal and build will run in the Linux environment under wsl.
<BR/>For more information see: [Visual Studio Code Remote - WSL].
1. Open a VScode Terminal and build the firmware:
<BR/>`make compile`
<BR/>With the demo firmware the binaries should build and report:
<BR/>'Program:    1016 bytes (99.2% Full)'
<BR/>If the size is > 100% there is a problem with the build. Check the toolchain version and try an earlier version. There is a known problem under Ubuntu, see above.
1. The firmware can't be flashed to the Bee from wsl. Switch back to the command window you used to install wsl and set up the environment:
<BR/>`exit`
<BR/>You should now be back at a **Windows** command prompt in the **SMBeeFrimware** directory.
1. Assuming you insalled *avrdude* under Windows as described above. Connect the bee and dock and invoke the programmer:
<BR/>`avrdude -p attiny10 -c usbasp -U fuse:w:0xfe:m -U flash:w:src/main.hex:i`
<BR/>Windows and linux share the same file system so this will flash the file you built under Linux.

## Building the firmware from the command line

Note: on some linux platforms you may need to prefix `make` with `sudo` to flash the firmware.

Compile and upload the firmware:

~~~bash
make
~~~

Other `make` arguments:
* `make demo` (Default): Flash the rstdisbl fuse and the demo firmware.
* `make all`: Compile and upload the firmware
* `make clean`: Remove the object (compiler generated) files.
* `make compile`: Compile the firmware.
* `make rstdisbl`: Flash the rstdisbl fuse. This is required the first-time the MCU is programmed.
* `make upload`: Flash the compiled firmware
* `make upload-demo`: Flash the precompiled demo firmware.

## VSCode IDE install and setup:

I used the MS VSCode IDE to develop and build the firmware. You can just use a text editor to modify the files but the IDE provides many useful features:

Download and install VSCode from [VSCode installers].

## GitHub Desktop App

You can install the [GitHub Desktop App] to clone the firmware repository into a local directory rather than using the command line.

## Demo LED sequence source

This is the source code for [program.c]. It can be easily modified to change the LED sequence:

~~~c
#include "main.h"

void program(void) {
  show(EYES); //Eyes on, full brightness
  wait(10); // 1s
  show1(EYES, FLASH_VSLOW);
  wait(30);
  show(ANTENNA_RL); //Both antennae on, 100% intensity.
  wait(5); // 0.5s
  show1(ANTENNA_RL, FLASH_ANTIPHASE | FLASH_SLOW);// Flash antennae in anti-phase to each other.
  wait(20);
  show(EYES);
  wait(10);
  show1(EYES, FLASH_VSLOW);
  wait(30);
  show(ANTENNA_RL);
  wait(5);
  show1(ANTENNA_RL, FLASH_SLOW);
  wait(30);
  show1(EYES, FLASH_MED);
  wait(10);
  show(ANTENNA_RL);
  wait(5);
  show1(ANTENNA_RL, FLASH_SLOW);
  wait(10);
  show2(ANTENNA_RL, FLASH_SLOW, FLASH_MED);// Flash ANTENNA_R slow & ANTENNA_L medium.
  wait(20);
  show2(ANTENNA_RL, FLASH_MED, FLASH_SLOW);
  wait(20);
  show(ANTENNA_RL);
  wait(5);
  show1(EYES, FLASH_SLOW);
  wait(15);
  show1(EYES, FLASH_MED);
  wait(15);
  show1(EYES, FLASH_FAST);
  wait(10);
  show2(EYES_STING, FLASH_SLOW, FLASH_FAST);// Eyes slow, sting fast
  wait(20);
  show1(EYES, FLASH_MED);
  wait(5);
  show2(EYES_STING, FLASH_SLOW, FLASH_FAST);
  wait(20);
  show1(EYES, FLASH_SLOW);
  wait(30);
}
~~~

---

This is the source code for [main.h], it provides the declarations for use in [program.c], the LED sequence:

~~~c
// PORTB values for output activation

#include <avr/io.h>

//LED state (ledState) values
#define LEDS_OFF 0
#define ANTENNA_R 2
#define ANTENNA_L 1
#define ANTENNA_RL 3 //Individually controllable
#define EYES 5       //Not individually controllable
#define STING 6
#define EYES_STING 4 //Individually controllable

//Flash rate values for dynamic brightness (brightness parameters)
#define FLASH_FAST 0b10000001
#define FLASH_MED 0b10000010
#define FLASH_SLOW 0b10000011
#define FLASH_VSLOW 0b10000100
#define FLASH_VVSLOW 0b10000111
#define FLASH_ANTIPHASE 0b01000000 // OR with flash-rate

//wait for time specified in unit: ~s/10 eg 15 = 1.5s
void wait(uint8_t time);

//Show the specified ledState with 100% static brightness
void show(uint8_t ledState);

//Show (with 1 extra parameter) the specified ledState with specified 
//static-brightness 0-100% or a flashRate value.
//OR in optional FLASH_ANTIPHASE flags eg "FLASH_VSLOW | FLASH_ANTIPHASE"
void show1(uint8_t ledState, uint8_t brightness);

//Show (with 2 extra parameters). Provides individal LED control for duel-LED states.
//Show the specified ledState with specified individual static-brightness 0-100% or flashRate value.
//OR in optional FLASH_ANTIPHASE flags eg "FLASH_VSLOW | FLASH_ANTIPHASE"
void show2(uint8_t ledState, uint8_t brightness1, uint8_t brightness2);
~~~

The main implementation can be found in [main.c].

# Firmware operation

These are the main information sources to support the firmware:

* [ATtiny10 data sheet]
* [SMBee KiCAD on GitHub]

## Functionality Overview
* The momentary action button SW1 toggles the MCU into and out of a low power sleep mode, this has negligible battery drain.
* `main.c` provides the main functionality and provides an API in in main.h to control the LED's.
* A separate file, `program.c` programs the steps that define the LED illumination sequence.
* For each programme step, the step-duration, fixed LED-brightness or Flash rate (modulated brightness) can be programmed.
* At each step the LED's to be illuminated are selected, either a single LED or a pair of LED's according to table 1. Both eyes behave as a single LED. Only the table 1 combinations are currently supported.
* Normally the `program.c` sequence is only executed once then sleep mode is entered again.
* If SW1 is given an extended press from sleep mode until the sting-led illuminates, the LED sequence is continuously looped until SW1 is pressed to enter sleep mode again.
* The code size of `main.c` must be minimized for size to leave flash memory space for `program.c`.
* The LED's are driven by two pulse-width-modulators used to vary their brightness.
* The frequency of the pulse width modulators is fixed and chosen to be high enough so the LED's don't appear to flicker as they are modulated on and off to control their brightness.
* The Brightness of the LED's is determined by the selected pulse-width on-off ratio of the connected PWM which in turn is controlled by the values of OCR0AL and OCR0BL.
* To minimize the MCU power, the MCU clock frequency is chosen to be as low as possible but high enough to support the required PWM frequency. This ensures maximum LED brightness.
* To give a pulsating effect rather simple on-off flashing, a waveform, `WAVEFORM[]` is used to modulate the the pulse-widths of the PWM's and therefor modulate the LED brightness.
* TC0 is utilized as an 8 bit continuously running timer counter (except in sleep). From its minimum count 0 to maximum count 0xFF is the PWM period after which the counter overflows to 0. The *on* time is from minimum count 0 to when the count matches the value in the register OCR0AL or OCR0BL for PWMA and PWMB respectively. The LED brightness is therefor controlled by setting the values of registers OCR0AL and OCR0BL.
* The TC0 overflow interrupt TOV0 is enabled to generate an interrupt on every TC0 period, 4.096ms. The integer `ct0_ticks` is defined to serve as the application timer, in the Interrupt Service Routine `ISR(TIM0_OVF_vect)`, this is incremented every time the ISR is called. `ct0_ticks` is a 16bit unsigned integer so it will overflow approximately every 4.5 minutes defining the longest timer period. `ct0_ticks` is always counting except in sleep mode.
* `void wait(uint8_t time)` is the API call to pad the sequence step period and therefore define the LED illumination time. It takes the current value of `ct0_ticks` counter-timer, adds the wait time to it and sets the value to the integer `waitTimerEnd`, it then polls for the `WAIT_TIMEOUT` bit of `status` to be set. `waitTimerEnd` is checked for equality with `ct0_ticks` on every interrupt incrementing `ct0_ticks`. When the integers match the `WAIT_TIMEOUT` bit of `status` is set. This timing method is designed to be unaffected by `ct0_ticks` overflows.
* `ct0_ticks` is also used to index the array `WAVEFORM[]` of LED brightness values, at the selected flash rate. `ct0_ticks` increments at a fixed rate however by selecting which 4 bits from `ct0_ticks` are used to index the waveform, the rate of change of the index values can be selected. The lower bits will be fast, moving up a bit, the rate will halve. Bits in the variable `ledModulator` are used to define the flash rate for both PWMs.
* An anti-phase flag `FLASH_ANTIPHASE` is available in the API to specify that one LED should flash in anti-phase to another. This is achieved by adding an offset to one of `WAVEFORM[]` index values therefore shifting its phase by 180°.
*  Each PWM can be configured to be active high to source current or active low to sink current depending on the LED it is required to drive.
* SW1 has no hardware debounce circuitry so it must be debounced in software. The characteristics of the switch are that it may fast cycle on and off after it is pressed or released for its settling time before stabilizing. The debounce functionality is to delay for the maximum settling time `SW_SETTLE_MS` after a level or edge change from SW1 is detected before trying to read its final state. As well as toggling sleep state, the switch press duration is measured at startup to potentially enable the continuous programme loop state.
* In sleep state the processor clock is stopped however peripheral circuitry including output drivers can selectively remain active. To minimize power in sleep, the code deactivates everything when entering sleep except the switch pull-up and PB3 change detect monitoring circuit required to trigger a wake-up interrupt on a SW1 state change. Wake-up interrupt handler `ISR(PCINT0_vect)` isn't implemented, this invokes the required default programme restart behavior.


* The PWM's are programmed to operate in the Fast PWM Mode see data-sheet 12.9.3. This is the simplest mode of operation that fits our purpose.


ATTiny	| PB2 | PB1 </BR> PWM-B | PB0 </BR> PWM-A | PB </BR> (hex)
-|:-:|:-:|:-:|:-:
antenna R	 | 0 | 1 | 0 | 2
antenna L	 | 0 | 0 | 1 | 1
antenna R&L  | 0 | 1 | 1 | 3
eyes	     | 1 | 0 | 1 | 5
sting	     | 1 | 1 | 0 | 6
eyes & sting | 1 | 0 | 0 | 4

**Table 1**, **PB**: Port B

<iframe width="400" height="225" src="https://www.youtube.com/embed/2UukflvSl64" frameborder="0" allowfullscreen></iframe>

[ATtiny10 data sheet]: https://ww1.microchip.com/downloads/en/DeviceDoc/atmel-8127-avr-8-bit-microcontroller-attiny4-attiny5-attiny9-attiny10_datasheet.pdf
[AVRDUDE]: https://www.nongnu.org/avrdude/
[Homebrew package manager]: https://brew.sh/
[Zadig usb driver install tool]: https://zadig.akeo.ie/
[AVRDUDE release notes]: https://savannah.nongnu.org/forum/forum.php?forum_id=8461
[AVRDUDE download page]: http://download.savannah.gnu.org/releases/avrdude/avrdude-6.3-mingw32.zip
[VSCode installers]: https://code.visualstudio.com/download
[GitHub Desktop App]: https://desktop.github.com/
[AVRDUDE path setup]: http://fab.cba.mit.edu/classes/863.16/doc/projects/ftsmin/windows_avr.html#avrdude
[SMBee]: https://github.com/milelo/SMBeeKiCad
[SMBee KiCAD on GitHub]: https://github.com/milelo/SMBeeKiCad
[SMBeeHive]: https://github.com/milelo/SMBeeHiveKiCad
[SMBeeHive KiCAD on GitHub]: https://github.com/milelo/SMBeeHiveKiCad
[demo.hex]: bin/demo.hex
[demo.hex.zip]: https://github.com/milelo/SMBeeFirmware/raw/master/bin/demo.hex.zip
[main.h]: src/main.h
[main.c]: src/main.c
[program.h]: src/program.h
[program.c]: src/program.c
[download git]: https://git-scm.com/downloads
[The AVR GCC Toolchain]: http://avr-eclipse.sourceforge.net/wiki/index.php/The_AVR_GCC_Toolchain
[install the AVR toolchain on windows]: http://fab.cba.mit.edu/classes/863.16/doc/projects/ftsmin/windows_avr.html
[WSL]: https://docs.microsoft.com/en-us/windows/wsl/about
[homebrew-avr]: https://github.com/osx-cross/homebrew-avr
[WinAVR]: https://sourceforge.net/projects/winavr/
[install WSL]: https://docs.microsoft.com/en-us/windows/wsl/install-win10
[wsl configuration]: https://docs.microsoft.com/en-us/windows/wsl/wsl-config
[LxRunOffline wiki]: https://github.com/DDoSolitary/LxRunOffline/wiki
[Visual Studio Code Remote - WSL]: https://code.visualstudio.com/docs/remote/wsl