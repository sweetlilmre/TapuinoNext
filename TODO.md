**TODO / Roadmap**

- Implement ini file for LCD selection, write default if it does not exist
  - Defaults will be
    - `#define I2C_DISP_ADDR 0x27`
    - `#define I2C_DISP_TYPE I2C_DISP_TYPE_HD44780`
- Finish TapeCart implementation
  - Run basic test with a buffer and built in PRG to test timing and handshake with C64 loader
    - Implement basic loader pre-code check for $0801 load address PRGs
    - Might need to change the loader to wait for the sense line signal (currently commented out), but would be useful to allow setup time for files / buffers
  - ~~Move buffer handling to a separate class (save on heap space, share buffer and have single allocation)~~
  - Add PRG as allowable extension to LoadSelector
    - Differentiate behaviour depending on TAP / PRG load
- C16 TAP (1/2 wave) playback
  - Fix cycle counting
  - Implement 1/2 wave recording
- Implement other machine type timings (PET etc) and check values against Vice
- Add file / dir filter to remove hidden directories (MacOS / Win / etc?)
