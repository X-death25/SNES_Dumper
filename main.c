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
#include "snes_dumper_read.h"
#include "snes_dumper_write.h"

int main(int argc, char *argv[])
{
#ifdef SDLGUI
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO); //Display informations on console
#endif
	
	if(argc == 1)
	{
		Display_Help(argv[0]);
		return 1;
	}
	else if (strcmp(argv[1], "-help") == 0)
	{
		Display_Help(argv[0]);
		return 1;
	}


#ifdef SDLGUI
	//Using GUI Mode ?
	int lang=0;
	if(strcmp(argv[1], "-gui") == 0)
	{
		use_gui=1;
		lang=0;
	}
	else if(strcmp(argv[1], "-gui_fr") == 0)
	{
		use_gui=1;
		lang=1;
	}
#endif

	int dump_snes_rom_mode=0;		// 1=Auto; 2=Manual Mode LoRom; 3=Manual Mode HiRom; 4=Manual mode ExHiRom
	int dump_snes_save_mode=0;		// 1=Auto; 2=Manual
	int write_snes_save=0;
	int erase_snes_save=0;
	int write_snes_flash=0;			// 1=LoRom; 2=HiRom; 3=ExHiRom; 4=LoRom MX29F1610
	int erase_snes_flash=0;			// 1=Auto, 2=LoRom; 3=HiRom; 4=ExHiRom; 5=LoRom MX29F1610
	int detect_snes_flash=0;
	int debug_mode=0;
	
	if (use_gui==1)
	{  
#ifdef SDLGUI

#endif
	}
	else
	{
		printf("\n");
		printf("-----------------------------------------------------------------------------------\n");
		printf("  #####                             ######                                         \n");
		printf(" #     # #    # ######  ####        #     # #    # #    # #####  ###### #####      \n");
		printf(" #       ##   # #      #            #     # #    # ##  ## #    # #      #    #     \n");
		printf("  #####  # #  # #####   ####  ##### #     # #    # # ## # #    # #####  #    #     \n");
		printf("       # #  # # #           #       #     # #    # #    # #####  #      #####      \n");
		printf(" #     # #   ## #      #    #       #     # #    # #    # #      #      #   #      \n");
		printf("  #####  #    # ######  ####        ######   ####  #    # #      ###### #    #     \n");
		printf("-----------------------------------------------------------------------------------\n");
		printf("\n");
		printf("Release : Aug. 2025 \n");
		printf("\n");
	}

	//LibUsb : Init & Detect
	if(Detect_Device()!=0)		return 1;
	
	// Now try to detect cartridge type
	Game_Infos();

	if(use_gui==0)
	{
		if (strcmp(argv[1], "-dumprom") == 0)
		{
			if (strcmp(argv[2], "auto")==0)				dump_snes_rom_mode=1;	
			else
				{
				if (strcmp(argv[2], "lorom")==0)		dump_snes_rom_mode=2;
				else if (strcmp(argv[2], "hirom")==0)		dump_snes_rom_mode=3;
				else if (strcmp(argv[2], "exhirom")==0)		dump_snes_rom_mode=4;
				
				if (strcmp(argv[3], "32") == 0)			game_size=32;	
				else if (strcmp(argv[3], "64") == 0)		game_size=64;
				else if (strcmp(argv[3], "128") == 0)		game_size=128;	
				else if (strcmp(argv[3], "256") == 0)		game_size=256;	
				else if (strcmp(argv[3], "512") == 0)		game_size=512;	
				else if (strcmp(argv[3], "1024") == 0)		game_size=1024;	
				else if (strcmp(argv[3], "2048") == 0)		game_size=2048;	
				else if (strcmp(argv[3], "4096") == 0)		game_size=4096;	
				else if (strcmp(argv[3], "8192") == 0)		game_size=8192;	
				else if (strcmp(argv[3], "16384") == 0)		game_size=16384;	
				else if (strcmp(argv[3], "32768") == 0)		game_size=32768;	
				else if (strcmp(argv[3], "65536") == 0)		game_size=65536;	
				}
		}
		else if (strcmp(argv[1], "-dumpsave") == 0)
		{
			if (strcmp(argv[2], "auto")==0)				dump_snes_save_mode=1;	
			else
				{
				dump_snes_save_mode=2;
				
				if (strcmp(argv[3], "32") == 0)			save_size=32;	
				else if (strcmp(argv[3], "64") == 0)		save_size=64;
				else if (strcmp(argv[3], "128") == 0)		save_size=128;	
				else if (strcmp(argv[3], "256") == 0)		save_size=256;	
				else if (strcmp(argv[3], "512") == 0)		save_size=512;	
				else if (strcmp(argv[3], "1024") == 0)		save_size=1024;	
				else if (strcmp(argv[3], "2048") == 0)		save_size=2048;	
				else if (strcmp(argv[3], "4096") == 0)		save_size=4096;	
				else if (strcmp(argv[3], "8192") == 0)		save_size=8192;	
				else if (strcmp(argv[3], "16384") == 0)		save_size=16384;	
				else if (strcmp(argv[3], "32768") == 0)		save_size=32768;	
				else if (strcmp(argv[3], "65536") == 0)		save_size=65536;	
				}
		}
		else if (strcmp(argv[1], "-writesave") == 0)
		{
			write_snes_save=1;
		}
		else if (strcmp(argv[1], "-erasesave") == 0)
		{
			erase_snes_save=1;
		}
		else if (strcmp(argv[1], "-writeflash") == 0)
		{
			if (strcmp(argv[2], "lorom")==0)		write_snes_flash=1;
			if (strcmp(argv[2], "hirom")==0)		write_snes_flash=2;
			if (strcmp(argv[2], "exhirom")==0)		write_snes_flash=3;
			if (strcmp(argv[2], "lorommx29f1610")==0)	write_snes_flash=4;

		}
		else if (strcmp(argv[1], "-eraseflash") == 0)
		{
			if (strcmp(argv[2], "auto")==0)			erase_snes_flash=1;
			if (strcmp(argv[2], "lorom")==0)		erase_snes_flash=2;
			if (strcmp(argv[2], "hirom")==0)		erase_snes_flash=3;
			if (strcmp(argv[2], "exhirom")==0)		erase_snes_flash=4;
			if (strcmp(argv[2], "lorommx29f1610")==0)	erase_snes_flash=5;
		}
		else if (strcmp(argv[1], "-detectflash") == 0)
		{
			detect_snes_flash=1;
		}
		else if (strcmp(argv[1], "-debug") == 0)
		{
			debug_mode=1;
		}
	}
	
	if(dump_snes_rom_mode>0) 		Dump_SNES_ROM(dump_snes_rom_mode);
	else if(dump_snes_save_mode>0) 		Dump_SNES_Save(dump_snes_save_mode);
	else if(write_snes_save>0) 		Write_SNES_Save();
	else if(erase_snes_save>0) 		Erase_SNES_Save();
	else if(write_snes_flash>0) 		Write_SNES_Flash(write_snes_flash);
	else if(erase_snes_flash>0) 		Erase_SNES_Flash();
	else if(detect_snes_flash>0) 		Detect_SNES_Flash();
	else if(debug_mode>0) 			Debug_Mode();
	
	return 0;
}
