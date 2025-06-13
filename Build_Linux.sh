#!/bin/bash

gcc main.c -DSDLGUI -I/usr/include/SDL2 -I/usr/include/libusb-1.0 -L/usr/lib -lSDL2main -lSDL2_image -lSDL2 -lusb-1.0 -o SNES_Dumper_GUI
gcc main.c -I/usr/include/libusb-1.0 -L/usr/lib -lusb-1.0 -o SNES_Dumper
mkdir Linux_Build
cp -dR Informations.txt ./Linux_Build
cp *.csv ./Linux_Build/
cp SNES_Dumper ./Linux_Build/
cp SNES_Dumper_GUI ./Linux_Build/
cp -dR images ./Linux_Build/
