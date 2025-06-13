/*

SNES Dumper CLI Version
X-death - 06/2025

SNES Dumper SDL GUI Version
Jackobo Le Chocobo (Akina Usagi) - 

*/

//Operating System Libraries and variables declarations
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//SNES Dumper Functions
#include "snes_dumper_main.h"


int main(int argc, char *argv[])
{
printf("\n");
printf("------------------------------------------------------------------\n");
printf("  ____  _   _ _____ ____    ____                                  \n");
printf(" / ___|| \ | | ____/ ___|  |  _ \ _   _ _ __ ___  _ __   ___ _ __ \n");
printf(" \___ \|  \| |  _| \___ \  | | | | | | | '_ ` _ \| '_ \ / _ \ '__|\n");
printf("  ___) | |\  | |___ ___) | | |_| | |_| | | | | | | |_) |  __/ |   \n");
printf(" |____/|_| \_|_____|____/  |____/ \__,_|_| |_| |_| .__/ \___|_|   \n");
printf("                                                 |_|              \n");
printf("------------------------------------------------------------------\n");
printf("\n");
printf("Release : Aug. 2025 \n");
printf("\n");

//LibUsb : Init & Detect
if(Detect_Device()!=0)		return 1;
	
return 0;
}


