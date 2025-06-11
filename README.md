# SNES Dumper

<img src="https://github.com/X-death25/SNES_Dumper/blob/main/github_gfx/snes_dumper_title.png" data-canonical-src="https://github.com/X-death25/SNES_Dumper/blob/main/github_gfx/snes_dumper_title.png" width="100%" /> 

Snes_Dumper is an USB-C cartridge reader writer for Super Nes / Super Famicom. 
You can backup / restore your cartridge and save for use it in Flashcart or Emu.  

Supported Features
-----

| Feature | Implemented |
| ------- | ----------- |
| ROM Dump up to 32 MEG  LoRom                | :white_check_mark: |
| ROM Dump up to 32 MEG  HiRom                | :white_check_mark: |
| ROM Dump up to 64 MEG  Ex-HiRom             | :white_check_mark: |
| ROM Dump up to 16 MEG  DSP 1-2-3            | :white_check_mark: |
| ROM Dump up to 16 MEG  Super FX 1 , 2       | :white_check_mark: |
| ROM Dump up to 48 MEG  S-DD1                | :white_check_mark: |
| ROM Dump up to 32 MEG  SA-1                 | :arrows_counterclockwise: |
| Rewrite LoRom cart or compatible            | :white_check_mark: |
| Rewrite HiRom cart or compatible            | :white_check_mark: |


## How to use it

You could find all documentation in the wiki here :

https://github.com/X-death25/SNES_Dumper/wiki

## Preview

<img src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/01.png" data-canonical-src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/01.png" width="49%" /> <img src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/card.jpg" data-canonical-src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/card.jpg" width="49%" />

<img src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/preview01.PNG" data-canonical-src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/preview01.png" width="49%" /> <img src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/preview02.PNG" data-canonical-src="https://github.com/X-death25/MD_Dumper_CLI/blob/main/github_gfx/preview02.png" width="49%" />

## Where i can buy it ?

This product is an unofficial and
unlicensed hardware release for the NINTENDO
Super NES / Super Famicom console, and is not affiliated
with Nintendo Enterprises.

You can buy last version of hardware in my Tindie page <br>

https://www.tindie.com/products/xdeath/usb-megadrive-genesis-cartridge-readerwriter/



## Commands

### CLI Mode

Read mode :
```
-read auto  :  Auto Mode-

```

### GUI Mode
```
-gui|-gui_fr  : Using GUI Window (ENG|FR)
```

## Special thanks

- paulfertser for his code on how to use usb hid on stm32 , it's the first piece of my first version of dumper https://github.com/paulfertser/stm32-tx-hid
- libopencm3 for the very cool STM32 dev lib.
- ichigobankai for some low level arm optimisation.
- Jackobo (Takeshi Imbert) for the beautiful GUI.
- mousebitelabs wonderfull website about Super Nintendo hardware / cartridge info https://mousebitelabs.com/2019/05/18/custom-pcb-explanation/

## Licenses:

- Software : GPL v3
- Hardware (Tindie Version ) : Firmware is closed source
- Documentation / WIKI : CC0 1.0

You can find copy of GPL v3 and CC0 1.0 in the licences folder

