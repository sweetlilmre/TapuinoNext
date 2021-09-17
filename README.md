# TapuinoNext

## Intro

Welcome to the TapuinoNext alpha.  

Most of the code has been written and tested to a fair extent, but there are bound to be loads of bugs.
Please report bugs as issues against the repo. If you have fixes, please submit these as a pull request.
I could really use some help with documentation in markdown format if you would like to contribute in that way.

## Hardware
I haven't had time to document the hardware to the level I would like.
At this point in time, you are pretty much on your own, apart from the schematic in the docs folder.

UPDATE:
- "instructions" to build a stripboard prototype are available at [Prototype Build](docs/PrototypeBuild.md)
- schematics and board designs for reference implementations are available in the [Eagle](docs/Eagle) folder.
- the mini board has been produced and tested successfully!
- **NB: the micro board has not**

The BOM for the TapuinoNext is:
- ESP32 Devkit equivalent: e.g. https://www.banggood.com/ESP32-WiFi-+-bluetooth-Development-Board-Ultra-Low-Power-Consumption-Dual-Core-ESP-32-ESP-32S-Similar-ESP8266-Geekcreit-for-Arduino-products-that-work-with-official-Arduino-boards-p-1175488.html

![ESP32 Dev Module](docs/img/ESP32_Dev_Module.png?raw=true "SD Card Adapter")

- resistors:
  - (voltage divider)
    - 1k5
    - 3k3
  - 100 Ohm (Recording LED current limiter)
- Rotary encoder KY-040: e.g. https://www.banggood.com/5Pcs-5V-KY-040-Rotary-Encoder-Module-AVR-PIC-p-951151.html

![KY-040 Rotary Encoder](docs/img/Rotary_Encoder.png?raw=true "KY-040 Rotary Encoder")

- LCD screen: SSD1306 OLED or HD44780 via I2C backpack
- SD Card breakout board / socket, no level shifter! Ones without resistors seem to work better. I used something like this: https://www.mischianti.org/2019/12/15/how-to-use-sd-card-with-esp8266-and-arduino/
This is the one I built:

![SD Card Adapter](docs/img/SD_Adapter.png?raw=true "SD Card Adapter")

- Bi-directional level shifter: e.g. https://www.banggood.com/10Pcs-Logic-Level-Converter-Bi-Directional-IIC-4-Way-Level-Conversion-Module-p-1033750.html
  
![4 Channel Level Shifter](docs/img/4CH_Level_Shifter.png?raw=true "4 Channel Level Shifter")

- C64 Datasette connector (use one off of a dead datasette or find one of these: https://www.thefuturewas8bit.com/cassette-port-connector-vic20-64-64c-128-128d-pet.html)

Wire this all up according to the schematic and flash the firmware.

## Firmware

Probably the easiest way to handle this at the moment is to:
- install VS Code
- install Platformio on top of that
- install the Espressif 32 platform or start a new project and choose "Espressif ESP32 Dev Module" as the board, with "Arduino" as the framework
- clone the TapuinoNext repository
- you will need to create a ***config-user.h*** file in the include directory. This is currently used to specify which of the 2 supported displays you have and what the correct I2C address for the display is. A sample ***config-user.h.example*** has beem provided
- point VS code to the directory you cloned the repo into and upload to the board (I'm assuming that you know how to do all of this)

## Attribution

See [Attribution](docs/Attribution.md)