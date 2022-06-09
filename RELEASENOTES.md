*****************************************
** TapuinoNext Release Notes           **
** sweetlilmre <sweetlilmre@gmail.com> **
*****************************************

## v0.0.3-alpha+? - ? June 2022

  - Changed changelog format for automatice release note generation
  - Added 'v' to version, should have been there from the begining
    - fixed auto_version.py to support this
  - Added TODO.md to keep some kind of roadmap going

## v0.0.2-alpha+237 - 04 June 2022
  - Fixed various compatibility issues with framework-arduinoespressif32 > 2.0.1
    - the platform is pinned to version 4.0.1 (and hence framework 2.0.1) waiting for timer fixes to get into the next framework release and platform update (4.0.5 and 2.0.4?)
    - timer functionality was changed from a re-triggered one-shot to a repeat timer (retriggered one-shot does not work in the context of the move to IDF4.4 from framework 2.0.2)
    - input initialisation in the constructor of ESP32InputHandler.cpp also broke (shouldn't have done this in the first place)

  - Other minor improvments
    - changed the text for the firmware update menu to make it less confusing
    - added the spinner UI into the update progress display
    - changed the name of the firmware release to "update.bin" for ease of use
