# Attibution for documentation and hardware designs:

## Fritzing
### TapuinoNext layout:

- Original design: Attila Gabriel  
- Fixes and edits by sweetlilmre

### Components:

ESP32-38PinWide-Component.fzpz:  
- https://forum.fritzing.org/t/fritzing-part-of-an-esp32/5355/4
  - https://forum.fritzing.org/uploads/default/original/2X/8/8d99adbb2e8f966178bdb46f5cffa7bd72ce2c2a.fzpz

KY-040_AZ_Rotary_encoder_v0.fzpz:  
- https://forum.fritzing.org/t/ky-040-rotary-encoder-breakout-board-part/11073
  - https://forum.fritzing.org/uploads/short-url/bN5n4wTJwT4cgNHyY3Omdc8aznC.fzpz

SD_Card_Module.fzp:  
- https://github.com/robertoostenveld/fritzing/blob/master/SD%20Card%20Module.fzpz
  - https://github.com/robertoostenveld/fritzing/raw/master/SD%20Card%20Module.fzpz

## Eagle parts
2 Pin Header part  
For some inexplicable reason the built in Eagle libraries do not have a 2 pin male header (starts at 3 pin). This part is a rework of the 3 pin part.  
<br>
ESP32 Devkit part  
Nothing on the internet that I could find had either the correct pin out or size for the dev kit board. I have created a part heavily inspired by the following:
- https://hackaday.io/project/46280-muffsy-stereo-relay-input-selector/log/87314-esp32s-devkit-eagle-library
  - https://cdn.hackaday.io/files/462801980043520/ESP32-DEVKITC.lbr
## Others:
PNG Images, stripboard design, schematics, board and libraries
- sweetlilmre

## Factory bin image generation script:
https://raw.githubusercontent.com/letscontrolit/ESPEasy/3678488f6a67ac9b899fab3d0ccd176c4437b1b4/tools/pio/post_esp32.py
