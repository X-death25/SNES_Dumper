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

	if (use_gui==1)
	{  
#ifdef SDLGUI
		int gui_scale=2;
		int quit = 0;
		SDL_Event event;
		int mouse_x, mouse_y;

		//Init Window
		SDL_Init(SDL_INIT_VIDEO);
		SDL_Window * window = SDL_CreateWindow("SNES Dumper version 2025", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 424*gui_scale, 240*gui_scale, 0);
		SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
		
		SDL_Surface * image1;
		SDL_Surface * image2;
		SDL_Surface * image3;
		SDL_Surface * image4;
		SDL_Surface * image5;
		
		//Create Background Texture
		if(lang==1)
		{
			#if defined(_WIN32)
				image1 = IMG_Load(".\\images\\opts_background_read_default_FR.png");
				image2 = IMG_Load(".\\images\\opts_background_read_ram_manual_FR.png");
				image3 = IMG_Load(".\\images\\opts_background_read_rom_manual_FR.png");
				image4 = IMG_Load(".\\images\\opts_background_write_default_FR.png");
				image5 = IMG_Load(".\\images\\opts_background_write_ram_FR.png");
			#else
				image1 = IMG_Load("./images/opts_background_read_default_FR.png");
				image2 = IMG_Load("./images/opts_background_read_ram_manual_FR.png");
				image3 = IMG_Load("./images/opts_background_read_rom_manual_FR.png");
				image4 = IMG_Load("./images/opts_background_write_default_FR.png");
				image5 = IMG_Load("./images/opts_background_write_ram_FR.png");
			#endif
		}
		else
		{
			#if defined(_WIN32)
				image1 = IMG_Load(".\\images\\opts_background_read_default.png");
				image2 = IMG_Load(".\\images\\opts_background_read_ram_manual.png");
				image3 = IMG_Load(".\\images\\opts_background_read_rom_manual.png");
				image4 = IMG_Load(".\\images\\opts_background_write_default.png");
				image5 = IMG_Load(".\\images\\opts_background_write_ram.png");
			#else
				image1 = IMG_Load("./images/opts_background_read_default.png");
				image2 = IMG_Load("./images/opts_background_read_ram_manual.png");
				image3 = IMG_Load("./images/opts_background_read_rom_manual.png");
				image4 = IMG_Load("./images/opts_background_write_default.png");
				image5 = IMG_Load("./images/opts_background_write_ram.png");
			#endif			
		}

		while (quit==0)
		{
			SDL_Texture * texture;
			if(gui_tab_mode==0 && (dump_mode==0 || dump_mode==2))		texture = SDL_CreateTextureFromSurface(renderer, image1);
			else if(gui_tab_mode==0 && data_type==1 && dump_mode==1)	texture = SDL_CreateTextureFromSurface(renderer, image2);
			else if(gui_tab_mode==0 && data_type==0 && dump_mode==1)	texture = SDL_CreateTextureFromSurface(renderer, image3);
			else if(gui_tab_mode==1 && data_type==0)			texture = SDL_CreateTextureFromSurface(renderer, image4);
			else if(gui_tab_mode==1 && data_type==1)			texture = SDL_CreateTextureFromSurface(renderer, image5);
			
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_SetRenderDrawColor(renderer, 250, 173, 5, 255);

			if(gui_tab_mode==0)
			{
				switch(data_type)
				{
					case 0: //ROM								
						for (int x = 224*gui_scale; x <=230*gui_scale; x++)
							for (int y = 67*gui_scale; y <=73*gui_scale; y++)
								SDL_RenderDrawPoint(renderer, x, y);
						break;
					case 1: //RAM
						for (int x = 270*gui_scale; x <=276*gui_scale; x++)
							for (int y = 67*gui_scale; y <=73*gui_scale; y++)
								SDL_RenderDrawPoint(renderer, x, y);
						break;
				}
				
				switch(dump_mode)
				{
					case 0: //Auto
						for (int x = 224*gui_scale; x <=230*gui_scale; x++)
							for (int y = 84*gui_scale; y <=90*gui_scale; y++)
								SDL_RenderDrawPoint(renderer, x, y);
						break;
					case 1: //Manual
						for (int x = 270*gui_scale; x <=276*gui_scale; x++)
							for (int y = 84*gui_scale; y <=90*gui_scale; y++)
								SDL_RenderDrawPoint(renderer, x, y);
								
						switch(data_type)
						{
							case 0: //ROM								
								switch(dump_rom_size_opts)
								{
									case 0: //32
										for (int x = 20*gui_scale; x <=26*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 1: //64
										for (int x = 66*gui_scale; x <=72*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 2: //128
										for (int x = 112*gui_scale; x <=118*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 3: //256
										for (int x = 158*gui_scale; x <=164*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 4: //512
										for (int x = 204*gui_scale; x <=210*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 5: //1024
										for (int x = 20*gui_scale; x <=26*gui_scale; x++)
											for (int y = 157*gui_scale; y <=163*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 6: //2048
										for (int x = 66*gui_scale; x <=72*gui_scale; x++)
											for (int y = 157*gui_scale; y <=163*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 7: //4096
										for (int x = 112*gui_scale; x <=118*gui_scale; x++)
											for (int y = 157*gui_scale; y <=163*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 8: //8192
										for (int x = 158*gui_scale; x <=164*gui_scale; x++)
											for (int y = 157*gui_scale; y <=163*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
								}
								
								switch(dump_cart_mode_opts)
								{
									case 0: //GG
										for (int x = 250*gui_scale; x <=256*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 1: //MD
										for (int x = 296*gui_scale; x <=302*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 2: //SMS
										for (int x = 342*gui_scale; x <=348*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
								}
								break;
							case 1: //RAM
								for (int x = 270*gui_scale; x <=276*gui_scale; x++)
									for (int y = 67*gui_scale; y <=73*gui_scale; y++)
										SDL_RenderDrawPoint(renderer, x, y);
								
								switch(dump_sram_size_opts)
								{
									case 0: //8192
										for (int x = 20*gui_scale; x <=26*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 1: //32768
										for (int x = 66*gui_scale; x <=72*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
								}
								
								switch(dump_sram_type_opts)
								{
									case 0: //Parallel SRAM
										for (int x = 250*gui_scale; x <=256*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 1: //Serial I2C
										for (int x = 342*gui_scale; x <=348*gui_scale; x++)
											for (int y = 140*gui_scale; y <=146*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
									case 2: //Serial SPI
										for (int x = 250*gui_scale; x <=256*gui_scale; x++)
											for (int y = 157*gui_scale; y <=163*gui_scale; y++)
												SDL_RenderDrawPoint(renderer, x, y);
										break;
								}
								
								break;
						}		
						break;		
					case 2: //Bankswitch
						for (int x = 329*gui_scale; x <=335*gui_scale; x++)
							for (int y = 84*gui_scale; y <=90*gui_scale; y++)
								SDL_RenderDrawPoint(renderer, x, y);
						break;
				}		
			}
			else if(gui_tab_mode==1)
			{
				if(data_type==1)
				{
					switch(dump_sram_type_opts)
					{
						case 0: //Parallel SRAM
							for (int x = 250*gui_scale; x <=256*gui_scale; x++)
								for (int y = 140*gui_scale; y <=146*gui_scale; y++)
									SDL_RenderDrawPoint(renderer, x, y);
							break;
						case 1: //Serial I2C
							for (int x = 342*gui_scale; x <=348*gui_scale; x++)
								for (int y = 140*gui_scale; y <=146*gui_scale; y++)
									SDL_RenderDrawPoint(renderer, x, y);
							break;
						case 2: //Serial SPI
							for (int x = 250*gui_scale; x <=256*gui_scale; x++)
								for (int y = 157*gui_scale; y <=163*gui_scale; y++)
									SDL_RenderDrawPoint(renderer, x, y);
							break;
					}
				}
				
				switch(erase_or_write)
				{
					case 0: //Erase								
						for (int x = 224*gui_scale; x <=230*gui_scale; x++)
							for (int y = 67*gui_scale; y <=73*gui_scale; y++)
								SDL_RenderDrawPoint(renderer, x, y);
						break;
					case 1: //Write
						for (int x = 282*gui_scale; x <=288*gui_scale; x++)
							for (int y = 67*gui_scale; y <=73*gui_scale; y++)
								SDL_RenderDrawPoint(renderer, x, y);
						break;
				}	
				
				switch(data_type)
				{
					case 0: //ROM								
						for (int x = 224*gui_scale; x <=230*gui_scale; x++)
							for (int y = 84*gui_scale; y <=90*gui_scale; y++)
								SDL_RenderDrawPoint(renderer, x, y);
						break;
					case 1: //RAM
						for (int x = 282*gui_scale; x <=288*gui_scale; x++)
							for (int y = 84*gui_scale; y <=90*gui_scale; y++)
								SDL_RenderDrawPoint(renderer, x, y);
						break;
				}	
			}
					
			//Display Texture	
			SDL_RenderPresent(renderer);
		
			SDL_GetMouseState(&mouse_x, &mouse_y);
			SDL_WaitEvent(&event);
				 
			switch (event.type)		//Window Events according to mouse positions and left click on this Window
			{
				case SDL_QUIT:
					quit = 1;
					SDL_DestroyTexture(texture);
					SDL_FreeSurface(image1);
					SDL_FreeSurface(image2);
					SDL_FreeSurface(image3);
					SDL_FreeSurface(image4);
					SDL_FreeSurface(image5);
					SDL_DestroyRenderer(renderer);
					SDL_DestroyWindow(window);
					SDL_Quit();
					return 1;
				case SDL_MOUSEBUTTONDOWN:
					if(mouse_x>=18*gui_scale  && mouse_x<=108*gui_scale) 
					{
						if (mouse_y>=46*gui_scale  && mouse_y<=58*gui_scale)
							gui_tab_mode = 0;
					}
					else if(mouse_x>=117*gui_scale  && mouse_x<=207*gui_scale) 
					{
						if (mouse_y>=46*gui_scale && mouse_y<=58*gui_scale)
							gui_tab_mode = 1;
					}
					else if(mouse_x>=337*gui_scale && mouse_x<=419*gui_scale)
					{
						if(mouse_y>=214*gui_scale && mouse_y<=235*gui_scale)	//Launch
						{
							quit = 1;
							SDL_DestroyTexture(texture);
							SDL_FreeSurface(image1);
							SDL_FreeSurface(image2);
							SDL_FreeSurface(image3);
							SDL_FreeSurface(image4);
							SDL_FreeSurface(image5);
							SDL_DestroyRenderer(renderer);
							SDL_DestroyWindow(window);
							SDL_Quit();
							break;
						}
					}
					
					if(gui_tab_mode==0 && (dump_mode==0 || dump_mode==2))
					{
						if(mouse_x>=222*gui_scale  && mouse_x<=232*gui_scale) 
						{
							if (mouse_y>=65*gui_scale && mouse_y<=75*gui_scale)
								data_type = 0;
							else if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								dump_mode = 0;
						}
						else if(mouse_x>=268*gui_scale && mouse_x<=278*gui_scale) 
						{
							if (mouse_y>=65*gui_scale && mouse_y<=75*gui_scale)
								data_type = 1;
							else if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								dump_mode = 1;
						}
						else if(mouse_x>=327*gui_scale && mouse_x<=337*gui_scale) 
						{
							if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								dump_mode = 2;
						}
					}
					else if(gui_tab_mode==0 && data_type==1 && dump_mode==1)
					{
						if(mouse_x>=222*gui_scale && mouse_x<=232*gui_scale) 
						{
							if (mouse_y>=65*gui_scale && mouse_y<=75*gui_scale)
								data_type = 0;
							else if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								dump_mode = 0;
						}
						else if(mouse_x>=268*gui_scale && mouse_x<=278*gui_scale) 
						{
							if (mouse_y>=65*gui_scale && mouse_y<=75*gui_scale)
								data_type = 1;
							else if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								dump_mode = 1;
						}
						else if(mouse_x>=327*gui_scale && mouse_x<=337*gui_scale) 
						{
							if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								dump_mode = 2;
						}
						else if(mouse_x>=18*gui_scale && mouse_x<=28*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_sram_size_opts = 0;
						}
						else if(mouse_x>=64*gui_scale && mouse_x<=74*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_sram_size_opts = 1;
						}
						else if(mouse_x>=248*gui_scale && mouse_x<=258*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_sram_type_opts = 0;
							else if (mouse_y>=155*gui_scale && mouse_y<=165*gui_scale)
								dump_sram_type_opts = 2;
						}
						else if(mouse_x>=340*gui_scale && mouse_x<=350*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_sram_type_opts = 1;
						}						
					}
					else if(gui_tab_mode==0 && data_type==0 && dump_mode==1)
					{
						if(mouse_x>=222*gui_scale && mouse_x<=232*gui_scale) 
						{
							if (mouse_y>=65*gui_scale && mouse_y<=75*gui_scale)
								data_type = 0;
							else if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								dump_mode = 0;
						}
						else if(mouse_x>=268*gui_scale && mouse_x<=278*gui_scale) 
						{
							if (mouse_y>=65*gui_scale && mouse_y<=75*gui_scale)
								data_type = 1;
							else if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								dump_mode = 1;
						}
						else if(mouse_x>=327*gui_scale && mouse_x<=337*gui_scale) 
						{
							if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								dump_mode = 2;
						}
						else if(mouse_x>=18*gui_scale && mouse_x<=28*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_rom_size_opts = 0;
							else if (mouse_y>=155*gui_scale && mouse_y<=165*gui_scale)
								dump_rom_size_opts = 5;
						}
						else if(mouse_x>=64*gui_scale && mouse_x<=74*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_rom_size_opts = 1;
							else if (mouse_y>=155*gui_scale && mouse_y<=165*gui_scale)
								dump_rom_size_opts = 6;
						}
						else if(mouse_x>=110*gui_scale && mouse_x<=120*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_rom_size_opts = 2;
							else if (mouse_y>=155*gui_scale && mouse_y<=165*gui_scale)
								dump_rom_size_opts = 7;
						}
						else if(mouse_x>=156*gui_scale && mouse_x<=166*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_rom_size_opts = 3;
							else if (mouse_y>=155*gui_scale && mouse_y<=165*gui_scale)
								dump_rom_size_opts = 8;
						}
						else if(mouse_x>=202*gui_scale && mouse_x<=212*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_rom_size_opts = 4;
						}
						else if(mouse_x>=248*gui_scale && mouse_x<=258*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_cart_mode_opts = 0;
						}
						else if(mouse_x>=294*gui_scale && mouse_x<=304*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_cart_mode_opts = 1;
						}
						else if(mouse_x>=340*gui_scale && mouse_x<=350*gui_scale) 
						{
							if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
								dump_cart_mode_opts = 2;
						}
					}
					else if(gui_tab_mode==1)
					{
						if(data_type==1)
						{
							if(mouse_x>=248*gui_scale && mouse_x<=258*gui_scale) 
							{
								if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
									dump_sram_type_opts = 0;
								else if (mouse_y>=155*gui_scale && mouse_y<=165*gui_scale)
									dump_sram_type_opts = 2;
							}
							else if(mouse_x>=340*gui_scale && mouse_x<=350*gui_scale) 
							{
								if (mouse_y>=138*gui_scale && mouse_y<=148*gui_scale)
									dump_sram_type_opts = 1;
							}		
						}
						if(mouse_x>=222*gui_scale && mouse_x<=232*gui_scale) 
						{
							if (mouse_y>=65*gui_scale && mouse_y<=75*gui_scale)
								erase_or_write = 0;
							else if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								data_type = 0;
						}
						else if(mouse_x>=280*gui_scale && mouse_x<=290*gui_scale) 
						{
							if (mouse_y>=65*gui_scale && mouse_y<=75*gui_scale)
								erase_or_write = 1;
							else if (mouse_y>=82*gui_scale && mouse_y<=92*gui_scale)
								data_type = 1;
						}
					}
					break;
			}
		}
#endif
	}
	else
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
	}

	//LibUsb : Init & Detect
	if(Detect_Device()!=0)		return 1;

// At this step we can try to read the buffer wake up Snes Dumper

    usb_buffer_out[0] = WAKEUP;// Affect request to  WakeUP Command

    libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 0); // Send Packets to SNES Dumper

    libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 0); // Receive packets from SNES Dumper


    printf("SNES Dumper %.*s",6, (char *)usb_buffer_in);
    printf("\n");
    printf("Hardware Firmware version : %d", usb_buffer_in[20]);
    printf(".%d\n", usb_buffer_in[21]);
    printf("Software Firmware version : %d",MAX_VERSION);
    printf(".%d\n",MIN_VERSION);

    printf("\nDisplaying USB IN buffer\n\n");

    intj=0;	
    for (int i = 0; i < 64; i++)
    {
        printf("%02X ",usb_buffer_in[i]);
        j++;
        if (j==16)
        {
            printf("\n");
            j=0;
        }
    }


    for (int i = 0; i < 64; i++)
    {
        usb_buffer_in[i]=0x00;
        usb_buffer_out[i]=0x00;
    }
// Now try to detect cartridge type
    printf("\nDetecting Cartridge type... ");
    /*
    while ( Cartridge_Detected == 0  )
    {

    // Now try to read ROM SNES Header

    if ( Cartridge_Type == 2  ) { address=32688;}
    if ( Cartridge_Type == 3  ) { address=32688;}
    if ( Cartridge_Type == 4  ) { address=65456;}

     //printf("Cartridge Type : %d",Cartridge_Type);
    usb_buffer_out[0] = READ_SNES;
    usb_buffer_out[1] = address & 0xFF ;
    usb_buffer_out[2] = (address & 0xFF00)>>8;
    usb_buffer_out[3]=(address & 0xFF0000)>>16;
    usb_buffer_out[4] = 0; // Slow Mode
    usb_buffer_out[5] = Cartridge_Type;
    libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
    libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

    printf("\nDisplaying USB IN buffer\n\n");

       for (i = 0; i < 64; i++)
        {
            printf("%02X ",usb_buffer_in[i]);
    		j++;
    		if (j==16){printf("\n");j=0;}
        }

    //printf("\nDetecting Cartridge type... ");
    // Try to detect LoRom Cartridge

    if ( usb_buffer_in[37] == 0x20 && Cartridge_Type == 2 ) { printf("\nCartridge Detected : LoROM <= 8Mb ");Cartridge_Detected = 2;}
    if ( usb_buffer_in[37] == 0x32 && Cartridge_Type == 2 ) { printf("\nCartridge Detected : LoROM <= 8Mb ");Cartridge_Detected = 2;}
    if ( usb_buffer_in[37] == 0x20 && Cartridge_Type == 3 ) { printf("\nCartridge Detected : LoROM >= 16Mb ");Cartridge_Detected = 3;}
    if ( usb_buffer_in[37] == 0x30 && Cartridge_Type == 3 ) { printf("\nCartridge Detected : LoROM >= 16Mb ");Cartridge_Detected = 3;}
    if ( (usb_buffer_in[37] == 0x31 || 0x21 ) && Cartridge_Type == 4 ) { printf("\nCartridge Detected : HiROM/ExHirom");Cartridge_Detected = 4;}
    if ( Cartridge_Detected == 0 ){ Cartridge_Type++;}


    if ( Cartridge_Type == 5) {Cartridge_Detected = 10;Cartridge_Type =0; printf("\nUnknown Cartridge type warning header will be display incorrect value");}
    // Try to detect HiRom Cartridge

    //printf("\nCartridge Type :  %d ",Cartridge_Type);
    //scanf("%ld");

    }
    */



    while ( Cartridge_Detected == 0  )
    {

        Cartridge_Type++;
        printf("Try to detect with cartridge type %d \n",Cartridge_Type);


        if ( Cartridge_Type == 2 ) // LoROM Cartridge < 16 Mb
        {

            Cartridge_Type = 2;

            address=32688;
            //address=0;

            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1] = address & 0xFF ;
            usb_buffer_out[2] = (address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4] = 0; // Slow Mode
            usb_buffer_out[5] = Cartridge_Type;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

            printf("\nDisplaying USB IN buffer\n\n");

            for (i = 0; i < 64; i++)
            {
                printf("%02X ",usb_buffer_in[i]);
                j++;
                if (j==16)
                {
                    printf("\n");
                    j=0;
                }
            }

            Rom_Type=usb_buffer_in[37]; // Cartridge Format
            if ( Rom_Type == 0x20)
            {
                Cartridge_Detected = 1;
            }
            if ( Rom_Type == 0x30)
            {
                Cartridge_Detected = 1;
            }

            Rom_Type=usb_buffer_in[38]; // Cartridge Format
            cx4Type = usb_buffer_in[25];
            if ( usb_buffer_in[25] == 0x32 )
            {
                cx4Type=2;   // X2
            }
            if ( usb_buffer_in[25] == 0x33 )
            {
                cx4Type=3;   // X3
            }
            //printf("\nLa valeur de Cx4 Type est %d",cx4Type);
            if ( Rom_Type == 0xF3)
            {
                Cartridge_Detected = 1;
                Cartridge_Type = 6;
            }

        }



        if ( Cartridge_Type == 4  ) // HiROM Cartridge
        {

            Cartridge_Type = 4;
            address=65456;
            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1] = address & 0xFF ;
            usb_buffer_out[2] = (address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4] = 0; // Slow Mode
            usb_buffer_out[5] = Cartridge_Type;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);


            printf("\nDisplaying USB IN buffer\n\n");

            for (i = 0; i < 64; i++)
            {
                printf("%02X ",usb_buffer_in[i]);
                j++;
                if (j==16)
                {
                    printf("\n");
                    j=0;
                }
            }



            Rom_Type=usb_buffer_in[37]; // Cartridge Format

            if ( Rom_Type == 0x21)
            {
                Cartridge_Detected = 1;
            }
            if ( Rom_Type == 0x30)
            {
                Cartridge_Detected = 1;
            }
            if ( Rom_Type == 0x31)
            {
                Cartridge_Detected = 1;
            }

        }

        if ( Cartridge_Type == 5  ) // DSP1 Cartridge
        {

            Cartridge_Type = 5;
            address=65456;
            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1] = address & 0xFF ;
            usb_buffer_out[2] = (address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4] = 0; // Slow Mode
            usb_buffer_out[5] = Cartridge_Type;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);


            printf("\nDisplaying USB IN buffer\n\n");

            for (i = 0; i < 64; i++)
            {
                printf("%02X ",usb_buffer_in[i]);
                j++;
                if (j==16)
                {
                    printf("\n");
                    j=0;
                }
            }



            Rom_Type=usb_buffer_in[37]; // Cartridge Format
            if ( Rom_Type == 0x21)
            {
                Cartridge_Detected = 1;
            }
            if ( Rom_Type == 0x31)
            {
                Cartridge_Detected = 1;
            }

        }


        if ( Cartridge_Type ==  6 ) // Capcom CX4 Cartridge
        {

            Cartridge_Type = 6;
            address=65456;
            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1] = address & 0xFF ;
            usb_buffer_out[2] = (address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4] = 0; // Slow Mode
            usb_buffer_out[5] = 3;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

            printf("\nDisplaying USB IN buffer\n\n");

            for (i = 0; i < 64; i++)
            {
                printf("%02X ",usb_buffer_in[i]);
                j++;
                if (j==16)
                {
                    printf("\n");
                    j=0;
                }
            }

            /*for (i=0; i<22; i++) {Game_Name[i]=usb_buffer_in[i+16];} // ROM Name
            printf("\n %s",Game_Name);*/
            /*if(memcmp((unsigned char *)Game_Name," ROCKMAN X2",11) == 0)
            {Cartridge_Detected = 1;Cartridge_Type = 6;printf("\nOk !");}*/

            Rom_Type=usb_buffer_in[38]; // Cartridge Format
            cx4Type = usb_buffer_in[25];
            if ( usb_buffer_in[25] == 0x32 )
            {
                cx4Type=2;   // X2
            }
            if ( usb_buffer_in[25] == 0x33 )
            {
                cx4Type=3;   // X3
            }
            //printf("\nLa valeur de Cx4 Type est %d",cx4Type);
            if ( Rom_Type == 0xF3)
            {
                Cartridge_Detected = 1;
                Cartridge_Type = 6;
            }

        }

        if ( Cartridge_Type == 7  ) // ExHiROM Cartridge
        {

            Cartridge_Type = 7;
            address=65456;
            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1] = address & 0xFF ;
            usb_buffer_out[2] = (address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4] = 0; // Slow Mode
            usb_buffer_out[5] = Cartridge_Type;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);


            printf("\nDisplaying USB IN buffer\n\n");

            for (i = 0; i < 64; i++)
            {
                printf("%02X ",usb_buffer_in[i]);
                j++;
                if (j==16)
                {
                    printf("\n");
                    j=0;
                }
            }
        }


// Escape loop

        else if ( Cartridge_Type > 15)
        {
            Cartridge_Detected = 1;
            printf("\nCan't detect Cartridge type...");
        }


    }

    Cartridge_Type = Cartridge_Type;


    printf("\nCartridge Type :  %d ",Cartridge_Type);
    /*
    usb_buffer_out[0] = READ_SNES;
    usb_buffer_out[1] = address & 0xFF ;
    usb_buffer_out[2] = (address & 0xFF00)>>8;
    usb_buffer_out[3]=(address & 0xFF0000)>>16;
    usb_buffer_out[4] = 0; // Slow Mode
    usb_buffer_out[5] = Cartridge_Type;
    libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
    libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);
    */

    printf("\n\n --- Cartridge INFO ---");

    for (i=0; i<22; i++)
    {
        Game_Name[i]=usb_buffer_in[i+16];   // ROM Name
    }
    printf("\nGame name :  %.*s",21,(char *) Game_Name);
    Rom_Type=usb_buffer_in[37]; // Cartridge Format
    if ( (Rom_Type & 0x01) == 1)
    {
        printf("\nCartridge format : HiROM");
    }
    else
    {
        printf("\nROM Map : LoROM");
    }
//if ( Rom_type == 0x20 ) {printf("\nROM Speed : SlowROM");}
    game_size= (0x400 << usb_buffer_in[39]); // Rom Size
    printf("\nGame Size :  %ld Ko",game_size/1024);
    printf(" / %ld Mbits",((game_size/1024)*8)/1024);
    Ram_Size= (0x400 << usb_buffer_in[40]); // Ram Size
    if ( usb_buffer_in[40] == 0x00)
    {
        printf("\nRAM Size : None");
        Ram_Size=0;
    }
    else
    {
        printf("\nRAM Size :  %ld Ko",Ram_Size/1024);
        printf(" / %ld Kbits",(Ram_Size/1024)*8);
    }
    if ( (Rom_Type >> 4 & 0x03) == 3)
    {
        printf("\nROM Speed : FastROM");
    }
    else
    {
        printf("\nROM Speed : SlowROM");
    }
    Rom_Region=usb_buffer_in[41];
    if (Rom_Region == 0)
    {
        printf("\nGame Region : Japan");
    }
    else if (Rom_Region == 1)
    {
        printf("\nGame Region : USA");
    }
    else if (Rom_Region == 2)
    {
        printf("\nGame Region : Europe");
    }
    else
    {
        printf("\nGame Region is : Unknown");
    }
    Rom_Region=usb_buffer_in[43];
    printf("\nGame Version : 1.%d ",Rom_Version);
    checksum_header = (usb_buffer_in[47]<<8) | usb_buffer_in[46];
    printf("\nHeader Checksum : %X", checksum_header);
//printf("\nHeader Checksum : %02X%02X ",usb_buffer_in[47],usb_buffer_in[46]);
    printf("\nComplement Checksum : %02X%02X ",usb_buffer_in[45],usb_buffer_in[44]);

// Special Cartridges detection code

// Extra Hardware detection Specific code

    // Search checksum cartridge in Custom Hardware game table

    for (i = 0; i < sizeof(HardwareID)/sizeof(short); i++)
    {
        if ( checksum_header == HardwareID[i] )
        {
            Hardwaredata=HardwareID[i+1];
            Hardwaretype = Hardwaredata >> 8;
            Hardwaresize = Hardwaredata & 0x0F;

            if ( Hardwaredata == 0x0705) // Daikaijuu Monogatari II
            {
                printf("\nSpecial Cartridge detected : ExHirom 40Mbits \n");
                game_size=1024*Hardwaresize*1024;
                Cartridge_Type = 7;
            }

            if ( Hardwaredata == 0x0706) // Tales Of Phantasia
            {
                printf("\nSpecial Cartridge detected : ExHirom 48Mbits \n");
                game_size=1024*Hardwaresize*1024;
                Cartridge_Type = 7;
            }

            if ( Hardwaredata == 0x0200) // 512k (Nintendo DSP1)
            {
                printf("\nSpecial Cartridge detected : Nintendo DSP-1 \n");
                Cartridge_Type = 5;
                game_size=1024*512;
            }

            if ( Hardwaredata == 0x0201) // 1024k (Nintendo DSP1)
            {
                printf("\nSpecial Cartridge detected : Nintendo DSP-1 \n");
                Cartridge_Type = 5;
                game_size=1024*1024;
            }

            if ( Hardwaredata == 0x0202) // 1280k (Nintendo DSP1)
            {
                printf("\nSpecial Cartridge detected : Nintendo DSP-1 \n");
                Cartridge_Type = 5;
                game_size=1024*1280;
            }

            if ( Hardwaredata == 0x0203) // 1536k (Nintendo DSP1)
            {
                printf("\nSpecial Cartridge detected : Nintendo DSP-1 \n");
                Cartridge_Type = 5;
                game_size=1024*1536;
            }

            if ( Hardwaredata == 0x0601) // 1536k (Capcom CX4)
            {
                Cartridge_Type = 6;
                printf("\nSpecial Cartridge detected : Capcom CX4 in X%d mode \n",cx4Type);
                game_size=1024*1536;
            }

            if ( Hardwaredata == 0x0804) // S-DD1 32Mb
            {
                printf("\nSpecial Cartridge detected : Nintendo S-DD1 \n");
                game_size=1024*Hardwaresize*1024;
                Cartridge_Type = 8;
            }

            if ( Hardwaredata == 0x0806) // S-DD1 48Mb
            {
                printf("\nSpecial Cartridge detected : Nintendo S-DD1 \n");
                game_size=1024*Hardwaresize*1024;
                Cartridge_Type = 8;
            }

        }

    }

    //  Cartridge_Type = 2;


    printf("\nCartridge type : %ld \n",Cartridge_Type);

    printf("\n\n --- MENU ---\n");
    printf(" 1) Dump SNES ROM\n");
    printf(" 2) Dump SNES Save\n");
    printf(" 3) Write SNES Save\n");
    printf(" 4) Erase SNES Save\n");
    printf(" 5) Write SNES Flash\n");
    printf(" 6) Erase SNES Flash\n");
    printf(" 8) Flash Memory Detection \n");
    printf(" 0) Debug\n");

    printf("\nYour choice: \n");
    scanf("%d", &choixMenu);



    switch(choixMenu)
    {
    case 1: // READ SNES ROM

        choixMenu=0;
        /* printf(" 1) Auto (from header)\n");
         printf(" 2) Manual\n");
         printf(" 3) Slow Mode\n");
         printf(" Your choice: ");
         scanf("%d", &choixMenu);
         if(choixMenu==3){usb_buffer_out[4]=1; } // in slow mode first
         else {game_size = Rom_Size;usb_buffer_out[4]=0; }*/

        printf("\n --- DUMP Mode : ---\n");
        printf(" 11) Automatic mode ( from header ) \n");
        printf(" 12) Manual mode LoRom\n");
        printf(" 13) Manual mode HiRom\n");
        printf(" 14) Manual mode ExHiRom\n");
        printf("\nYour choice: \n");
        scanf("%d", &choixMenu);

        if ( choixMenu == 12 )
        {
            printf("Mode set to Lorom.. \n");
            Cartridge_Type = 2;
            printf("Enter game size in Ko ...\n");
            scanf("%ld", &game_size);
            game_size = game_size * 1024;
        }

        if ( choixMenu == 13 )
        {
            printf("Mode set to HiRom.. \n");
            Cartridge_Type = 4;
            printf("Enter game size in Ko ...\n");
            scanf("%ld", &game_size);
            game_size = game_size * 1024;
        }
        else
        {
            printf("Mode set to Automatic.. \n");
        }



        printf("Sending command Dump ROM \n");
        printf("Dumping please wait ...\n");
        address=0;
        printf("\nRom Size : %ld Ko \n",game_size/1024);
        BufferROM = (unsigned char*)malloc(game_size);
        // Cleaning Buffer

        address=0;
        i=0;
        for (i=0; i<game_size; i++)
        {
            BufferROM[i]=0x00;
        }
        i=0;


        switch(Cartridge_Type)
        {
        case 2: // LoRom < 16 Mb

            // Start Dump in high speed mode


            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1; // high speed mode
            usb_buffer_out[5] = Cartridge_Type;

            timer_start();
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            printf("ROM dump in progress...\n");

            res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes, 60000);
            timer_end();
            timer_show();
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }
            myfile = fopen("dump_rom.sfc","wb");
            fwrite(BufferROM,1,game_size, myfile);
            fclose(myfile);
            timer_end();
            timer_show();
            return 0;

        case 4: // HiRom

            // Start Dump in high speed mode


            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1; // high speed mod
            usb_buffer_out[5] = Cartridge_Type;

            timer_start();

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes,0);
            printf("ROM dump in progress...\n");

            res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes,0);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }
            myfile = fopen("dump_rom.sfc","wb");
            fwrite(BufferROM,1,game_size, myfile);
            fclose(myfile);
            timer_end();
            timer_show();
            return 0;


        case 5: // DSP-1

            // Start Dump in high speed mode


            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1; // high speed mod
            usb_buffer_out[5] = Cartridge_Type;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes,0);
            printf("ROM dump in progress...\n");

            res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes,0);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }
            myfile = fopen("dump_rom.sfc","wb");
            fwrite(BufferROM,1,game_size, myfile);
            fclose(myfile);
            return 0;

        case 6: // Capcom CX-4

            // Start Dump in high speed mode

            printf("Dump ROM in mode Capcom CX-4...\n");
            //timer_start();
            game_size = 1024*2048;
            printf("Dump first part of the ROM...\n");


            address=0;
            BufferROM = (unsigned char*)malloc(game_size);
            // Cleaning Buffer

            address=0;
            i=0;
            for (i=0; i<game_size; i++)
            {
                BufferROM[i]=0x00;
            }
            i=0;

            printf("Write CX-4 ROM Register...\n");
            usb_buffer_out[0] = WRITE_CX4_BANK;
            usb_buffer_out[8] = 0x00; // Bank Number
            usb_buffer_out[9] = 1; // Value

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1] = address & 0xFF ;
            usb_buffer_out[2] = (address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4] = 0; // Slow Mode
            usb_buffer_out[5] = 0x02;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

            address=0;

            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1; // high speed mod
            usb_buffer_out[5] = 2;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            printf("ROM dump in progress...\n");

            res = libusb_bulk_transfer(handle, 0x82,BufferROM,1024*1024, &numBytes,60000);

            if (res != 0)
            {
                printf("Error 1 \n");
                return 1;
            }

            address=0x400000;

            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1; // high speed mod
            usb_buffer_out[5] = 2;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            printf("ROM dump in progress...\n");

            res = libusb_bulk_transfer(handle, 0x82,BufferROM+(1024*1024),1024*1024, &numBytes,60000);

            if (res != 0)
            {
                printf("Error 2 \n");
                return 1;
            }





            myfile = fopen("dump_rom.sfc","wb");
            fwrite(BufferROM,1,game_size, myfile);
            fclose(myfile);
            return 0;

        case 7: // Ex-HiRom

            // Start Dump in high speed mode


            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1; // high speed mod
            usb_buffer_out[5] = Cartridge_Type;
            timer_start();

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes,0);
            printf("ROM dump in progress...\n");

            res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes,0);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }
            myfile = fopen("dump_rom.sfc","wb");
            fwrite(BufferROM,1,game_size, myfile);
            fclose(myfile);
            timer_end();
            timer_show();

            return 0;

        case 8: // S-DD1

            printf("Dump ROM in mode S-DD1...\n");
            timer_start();
            printf("Dump first part of the ROM...\n");

            address=0;
            BufferROM = (unsigned char*)malloc(game_size);
            // Cleaning Buffer

            address=0;
            i=0;
            for (i=0; i<game_size; i++)
            {
                BufferROM[i]=0x00;
            }
            i=0;

            unsigned long offset=0;

            // Start Dump in high speed mode
            i=0;

            for (i = 0; i < 64; i++)
            {
                usb_buffer_in[i]=0x00;

            }
            i=0;
            printf("Dumping Bank 0 & Bank 1..\n");
            printf("Write S-DD1 ROM Register...\n");
            usb_buffer_out[0] = WRITE_SDD1_BANK;
            usb_buffer_out[8] = 0x00; // Bank Number
            usb_buffer_out[9] = j; // Value

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

            printf("Read S-DD1 ROM Register...\n");

            usb_buffer_out[0] = READ_SDD1_BANK;
            usb_buffer_out[7] = 0x00; // Bank Number
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);
            SDD1_ROM_Register = usb_buffer_in[63];
            printf("Value of S-DD1 ROM Register is %ld \n",SDD1_ROM_Register);

            usb_buffer_out[0] = READ_SNES;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=1; // high speed mod
            usb_buffer_out[5] = 0x08;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes,0);

            res = libusb_bulk_transfer(handle, 0x82,BufferROM,2048*1024, &numBytes,0);
            if (res != 0)
            {
                printf("Error \n");
                return 1;
            }
            k=k+(2048*1024);
            j++;


            while ( k < game_size)

            {
                j=j+1;
                printf("Dump upper Bank %ld...\n",j);

                printf("Write S-DD1 ROM Register...\n");
                usb_buffer_out[0] = WRITE_SDD1_BANK;
                usb_buffer_out[8] = 0x00; // Bank Number
                usb_buffer_out[9] = j; // Value

                libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
                libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

                //printf("Read S-DD1 ROM Register...\n");

                usb_buffer_out[0] = READ_SDD1_BANK;
                usb_buffer_out[7] = 0x00; // Bank Number
                libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
                libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);
                SDD1_ROM_Register = usb_buffer_in[63];
                //printf("Value of S-DD1 ROM Register is %ld \n",SDD1_ROM_Register);


                usb_buffer_out[0] = READ_SNES;
                usb_buffer_out[1] = address & 0xFF ;
                usb_buffer_out[2] = (address & 0xFF00)>>8;
                usb_buffer_out[3]=(address & 0xFF0000)>>16;
                usb_buffer_out[4] = 0; // Slow Mode
                usb_buffer_out[5] = 0x04;
                libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
                libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);


                printf("Restarting Dump please wait ...\n");
                address=0xF00000;

                usb_buffer_out[0] = READ_SNES;
                usb_buffer_out[1]=address & 0xFF;
                usb_buffer_out[2]=(address & 0xFF00)>>8;
                usb_buffer_out[3]=(address & 0xFF0000)>>16;
                usb_buffer_out[4]=1; // high speed mod
                usb_buffer_out[5] = 0x04;

                libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes,0);
                printf("ROM dump in progress...\n");

                res = libusb_bulk_transfer(handle, 0x82,BufferROM+k,1024*1024, &numBytes,0);
                if (res != 0)
                {
                    printf("Error \n");
                    return 1;
                }
                k=k+(1024*1024);

            }



            printf("Dump ROM Completed ! \n");
            timer_end();
            timer_show();



            myfile = fopen("dump_rom.sfc","wb");
            fwrite(BufferROM,1,game_size, myfile);
            fclose(myfile);
            return 0;



            /* usb_buffer_out[0] = READ_SNES;
             usb_buffer_out[1]=address & 0xFF;
             usb_buffer_out[2]=(address & 0xFF00)>>8;
             usb_buffer_out[3]=(address & 0xFF0000)>>16;
             usb_buffer_out[4]=1; // high speed mod
             usb_buffer_out[5] = Cartridge_Type;

             libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes,0);
             printf("ROM dump in progress...\n");

             res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes,0);
             if (res != 0)
             {
                 printf("Error \n");
                 return 1;
             }
            myfile = fopen("dump_rom.sfc","wb");
            fwrite(BufferROM,1,game_size, myfile);
            fclose(myfile);
            return 0;*/


        }








    // Start Dump in slow mode

    /*	while ( i< game_size)
        {
    		usb_buffer_out[0] = READ_SNES;
    usb_buffer_out[1] = address & 0xFF ;
    usb_buffer_out[2] = (address & 0xFF00)>>8;
    usb_buffer_out[3]=(address & 0xFF0000)>>16;
    usb_buffer_out[4] = 0; // Slow Mode
    usb_buffer_out[5] = Cartridge_Type;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,(BufferROM+i),64, &numBytes, 60000);
            address +=64; //next adr
            i+=64;

            printf("\r ROM dump in progress: %ld%%", ((100 * i)/game_size));
            fflush(stdout);
        }*/


    /*if ( Cartridge_Type == 4 || 5 )
    {


        usb_buffer_out[0] = READ_SNES;
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[4]=1; // high speed mod
        usb_buffer_out[5] = Cartridge_Type;

        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes,0);
        printf("ROM dump in progress...\n");

        res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes,0);
        if (res != 0)
        {
            printf("Error \n");
            return 1;
        }
    }*/

    /* if ( Cartridge_Type == 6 )
     {


    	address=0;

         usb_buffer_out[0] = READ_SNES;
         usb_buffer_out[1]=address & 0xFF;
         usb_buffer_out[2]=(address & 0xFF00)>>8;
         usb_buffer_out[3]=(address & 0xFF0000)>>16;
         usb_buffer_out[4]=1; // high speed mode
         usb_buffer_out[5] = 6;

         libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
         printf("ROM dump in progress...\n");

         res = libusb_bulk_transfer(handle, 0x82,BufferROM,game_size, &numBytes, 60000);
         if (res != 0)
         {
             printf("Error \n");
             return 1;
         }







     }


     /*if ( Hardwaretype == 1 && Hardwaredata == 0x0106 ) // 48 Mb ExHirom Cartridge Tricks

      {
     	 // First Clean 48Mb Buffer
     	 Buffer1 = (unsigned char*)malloc(1024*2048);
     	 Buffer2 = (unsigned char*)malloc(1024*4096);
     	 i=0;
     		for (i=0; i<1024*4096; i++)
     			{
     				Buffer2[i]=0x00;
     			}
     	 i=0;
     		for (i=0; i<1024*2048; i++)
     			{
     				Buffer1[i]=0x00;
     			}
     memcpy( Buffer1,BufferROM,1024*2048);
     memcpy( Buffer2,BufferROM+1024*2048,1024*4096);
     //	printf("\nFirst Mux Ok !");
     memcpy( BufferROM,Buffer2,1024*4096);
     memcpy( BufferROM+1024*4096,Buffer1,1024*2048);
     //printf("\nMux Ok !");
     }*/

    /* myfile = fopen("dump_rom.sfc","wb");
     fwrite(BufferROM,1,game_size, myfile);
     fclose(myfile);
    break;*/

    case 2: // DUMP SNES Save
        choixMenu=0;
        printf(" 1) Auto (from header)\n");
        printf(" 2) Manual \n");
        printf(" Your choice: ");
        scanf("%d", &choixMenu);

        if(choixMenu==1)
        {
            save_size = Ram_Size;
        }

        if(choixMenu==2)
        {
            printf(" Enter number of KB to dump: ");
            scanf("%d", &save_size);
            save_size *= 1024;
        }

        if(choixMenu>3)
        {
            printf(" Wrong number!\n\n");
            return 0;
        }

        buffer_save = (unsigned char*)malloc(save_size); // raw in 8bit format

        for (i=0; i<(save_size); i++)
        {
            buffer_save[i]=0x00;
        }

        usb_buffer_out[0] = READ_SNES_SAVE;
        address=0;
        i=0;

        while ( i< save_size)
        {

            usb_buffer_out[0] = READ_SNES_SAVE;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=0; // in slow mode first
            usb_buffer_out[5] = Cartridge_Type;

            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82,(buffer_save+i),64, &numBytes, 60000);
            address +=64; //next adr
            i+=64;

            printf("\r SAVE dump in progress: %ld%%", ((100 * i)/save_size));
            fflush(stdout);
        }

        i=0;
        j=0;
        myfile = fopen("dump_rom.srm","wb");
        fwrite(buffer_save,1,save_size, myfile);
        fclose(myfile);
        break;

    case 3: // Write SNES SAVE

        printf(" Save file: ");
        scanf("%60s", dump_name);
        myfile = fopen(dump_name,"rb");
        fseek(myfile,0,SEEK_END);
        save_size = ftell(myfile);
        buffer_save = (unsigned char*)malloc(save_size);
        rewind(myfile);
        fread(buffer_save, 1, save_size, myfile);
        fclose(myfile);

        // Write SRAM

        i=0;
        j=0;
        k=0;
        address=0;
        while ( j < save_size)
        {

            // 1 : Send buffer to SRAM


            // Fill usb out buffer with save data
            for (i=32; i<64; i++)
            {
                usb_buffer_out[i] = buffer_save[k+j];
                k++;
            }
            i=0;
            k=0;
            // Fill temp1 buffer 1 with save data value
            for (i=32; i<64; i++)
            {
                buffer_temp1[k] = usb_buffer_out[i];
                k++;
            }
            i=0;
            k=0;

            usb_buffer_out[0] = WRITE_SNES_SAVE; // Select write in 8bit Mode
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[5] = Cartridge_Type;
            usb_buffer_out[7] = 0x00;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            while ( usb_buffer_in[7] != 0xAA)
            {
                libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);
            }

            // Custom Test : check value read SRAM into buffer temp 2

            usb_buffer_out[0] = READ_SNES_SAVE;
            usb_buffer_out[1]=address & 0xFF;
            usb_buffer_out[2]=(address & 0xFF00)>>8;
            usb_buffer_out[3]=(address & 0xFF0000)>>16;
            usb_buffer_out[4]=0; // in slow mode first
            usb_buffer_out[5] = Cartridge_Type;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);

            i=0;
            k=0;
            // Fill temp1 buffer 2 with readed save data value
            for (i=0; i<32; i++)
            {
                buffer_temp2[i] = usb_buffer_in[i];
            }

            // Compare the two buffer

            if(memcmp((unsigned char *)buffer_temp1,(unsigned char *)buffer_temp2,32) == 0)
            {
                address+=32;
                j+=32;
            }
            else
            {
                printf("CRC Failed at address : %ld \n",address);
                Retry++;
            }
            if ( Retry == 50 )
            {
                Retry=0;
                address+=32;
                j+=32;
            }

            /*printf("\nDisplaying buffer_temp2 buffer\n\n");

             for (i = 0; i < 64; i++)
            {
            printf("%02X ",buffer_temp2[i]);
            j++;
            if (j==16){printf("\n");j=0;}
            }
            while(1);*/

            // compare temp1 with temp2


            //  address+=32;
            //j+=32;
        }

        printf("SRAM Sucessfully Writted ...\n");
        break;


    case 4: // Erase SNES SAVE

        choixMenu=0;
        i=0;
        printf("Erasing SNES Save...");
        usb_buffer_out[0] = ERASE_SNES_SAVE; // Select write in 8bit Mode
        usb_buffer_out[1]=address & 0xFF;
        usb_buffer_out[2]=(address & 0xFF00)>>8;
        usb_buffer_out[3]=(address & 0xFF0000)>>16;
        usb_buffer_out[5] = Cartridge_Type;
        usb_buffer_out[7] = 0x00;
        libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
        while ( usb_buffer_in[7] != 0xAA)
        {
            libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);
        }
        printf("\nSRAM Sucessfully Erased ...\n");
        break;

    case 5: // Write SNES Flash

        choixMenu=0;
        i=0;
        unsigned char flash_MX29F1610=0;

        // Menu for Cartridge type

        printf("\n --- REPRO ---\n");
        printf(" 11) LoRom Repro \n");
        printf(" 12) HiRom Repro\n");
        printf(" 13) ExHiRom Repro\n");
        printf(" 14) LoRom Repro (MX29F1610) \n");
        printf("\nYour choice: \n");
        scanf("%d", &Flash_type);
        if ( Flash_type == 11 )
        {
            Cartridge_Type = 2;
            printf("\nCartridge type : LoRom  \n");
        }
        if ( Flash_type == 12 )
        {
            Cartridge_Type = 4;
            printf("\nCartridge type : HiRom  \n");
        }
        if ( Flash_type == 13 )
        {
            Cartridge_Type = 7;
            printf("\nCartridge type : ExHiRom  \n");
        }
        if ( Flash_type == 14 )
        {
            Cartridge_Type = 2;
            printf("\nCartridge type : LoRom MX29F1610  \n");
            flash_MX29F1610=1;
        }
        // else {Cartridge_Type = 4;}

        if ( flash_MX29F1610 == 1 )
        {

            printf(" Enter ROM file: ");
            scanf("%60s", dump_name);
            myfile = fopen(dump_name,"rb");
            fseek(myfile,0,SEEK_END);
            game_size = ftell(myfile);
            BufferROM = (unsigned char*)malloc(game_size);
            rewind(myfile);
            fread(BufferROM, 1, game_size, myfile);
            fclose(myfile);

            i=0;
            address = 0;
            j=0;
            k=0;
			unsigned long offset_mx=0;



            timer_start();


            printf("Create MX Buffer...\n");
            usb_buffer_out[0] = CREATE_MX_BUFFER;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            i=0;
            while(usb_buffer_in[6]!=0xAA)
            {
                libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);   //wait status
            }
            printf("Buffer MX Created sucessfully ! \n");

 address = 0;

		while ( k < game_size)

		{

                // Send ROM To MX Buffer
                while (j!=4)
                {


                    // Fill usb out buffer with save data in 8bit

                    for (i=0; i<32; i++)
                    {
                        usb_buffer_out[32+i] = BufferROM[i+k];
                    }
                    k=k+32;
                    i=0;

 
                  //    printf("Send ROM to MX Buffer...\n");
                    usb_buffer_out[0] = WRITE_MX_BUFFER; // Select write in 16bit Mode
                    usb_buffer_out[1]=address & 0xFF;
                    usb_buffer_out[2]=(address & 0xFF00)>>8;
                    usb_buffer_out[3]=(address & 0xFF0000)>>16;
                    usb_buffer_out[5] = Cartridge_Type;
                    usb_buffer_out[6] = j;
                    j=j+1;

                    libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 6000);
                    while(usb_buffer_in[6]!=0xBB)
                    {
                        libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);   //wait status
                    }

                }
                j=0;


             //   printf("Flash Buffer in memory...\n");
                usb_buffer_out[0] = FLASH_MX_BUFFER; // Select write in 16bit Mode
                usb_buffer_out[1] = address & 0xFF;
                usb_buffer_out[2] = (address & 0xFF00)>>8;
                usb_buffer_out[3] = (address & 0xFF0000)>>16;

                libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 6000);
                while(usb_buffer_in[6]!=0xCC)
                {
                    libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);   //wait status
                }

                // printf("Buffer Flashed \n");
				address=address+64;
				//k=k+64;

        
			}

             
            timer_end();
            timer_show();
            printf("Flash Sucessfully Writted ...\n");
            break;

           }

            else
            {

                // Sending ROM to Buffer

                printf(" Enter ROM file: ");
                scanf("%60s", dump_name);
                myfile = fopen(dump_name,"rb");
                fseek(myfile,0,SEEK_END);
                game_size = ftell(myfile);
                BufferROM = (unsigned char*)malloc(game_size);
                rewind(myfile);
                fread(BufferROM, 1, game_size, myfile);
                fclose(myfile);

                i=0;
                address = 0;
                j=0;
                k=0;

                printf("Writing Flash please wait ...\n");
                timer_start();
                while ( j < game_size)
                {

                    for (i=32; i<64; i++)
                    {
                        usb_buffer_out[i] = BufferROM[k];
                        k++;
                    }
                    i=0;
                    j+=32;

                    usb_buffer_out[0] = WRITE_SNES_FLASH; // Select write in 8bit Mode
                    usb_buffer_out[1]=address & 0xFF;
                    usb_buffer_out[2]=(address & 0xFF00)>>8;
                    usb_buffer_out[3]=(address & 0xFF0000)>>16;
                    usb_buffer_out[5] = Cartridge_Type;
                    libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
                    while ( usb_buffer_in[7] != 0xAA)
                    {
                        libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);
                    }
                    //printf("Buffer writted !\n");
                    address+=32;
                    printf("\r WRITE SNES flash in progress: %ld%%", ((100 * address)/game_size));
                    fflush(stdout);
                }
                printf("s\n");
            }
            timer_end();
            timer_show();
            printf("Flash Sucessfully Writted ...\n");
            break;


        case 6: //ERASE SNES FLASH

            printf("\n --- ERASE Mode : ---\n");
            printf(" 11) Automatic mode ( from header ) \n");
            printf(" 12) ERASE LoRom REPRO\n");
            printf(" 13) ERASE HiRom REPRO\n");
            printf(" 14) ERASE ExHirom REPRO\n");
            printf(" 15) ERASE LoRom REPRO (MX29F1610) \n");
            printf("\nYour choice: \n");
            scanf("%d", &choixMenu);

            if ( choixMenu == 12 )
            {
                Cartridge_Type = 2;
            }
            if ( choixMenu == 13 )
            {
                Cartridge_Type = 4;
            }
            if ( choixMenu == 15 )
            {
                Cartridge_Type = 4;
                usb_buffer_out[6] = 2;
            }

            usb_buffer_out[0] = ERASE_SNES_FLASH;
            usb_buffer_out[5] = Cartridge_Type;
            printf("Starting Flash Erase ...\n");
            timer_start();
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
            i=0;
            while(usb_buffer_in[0]!=0x01)
            {
                libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);   //wait status
                printf("\r ERASE flash in progress: %s ", wheel[i]);
                fflush(stdout);
                i++;
                if(i==4)
                {
                    i=0;
                }
            }
            timer_end();
            timer_show();
            printf("\r ERASE SNES flash in progress: 100%%");
            fflush(stdout);

            break;


        case 8: // Vendor / ID Info

            for (i = 0; i < 64; i++)
            {
                usb_buffer_in[i]=0x00;
            }

            //Cartridge_Type = 2;


            //printf("Detecting Flash...\n");
            printf("-=Flash Memory module detection-=\n\n");
            usb_buffer_out[0] = INFOS_ID;
            usb_buffer_out[5] = Cartridge_Type;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 6000);
            libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);

            printf("\nDisplaying USB IN buffer\n\n");

            for (i = 0; i < 64; i++)
            {
                printf("%02X ",usb_buffer_in[i]);
                j++;
                if (j==16)
                {
                    printf("\n");
                    j=0;
                }
            }

            manufacturer_id = usb_buffer_in[1];
            chip_id = usb_buffer_in[3];
            flash_id = (manufacturer_id<<8) | chip_id;

            printf("\n1) Try to detect Microchip / SST flash memory :");
            printf(" Flash ID : %04X \n\n",flash_id);
            if ( flash_id != rom_id )
            {
                switch(flash_id)
                {
                case 0xBFB6 :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : 2x SST39SF020 \n");
                    printf("Capacity : 4Mb \n");
                    printf("Voltage : 5V \n");
                    break;
                case 0xBFB7 :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : 2x SST39SF040 \n");
                    printf("Capacity : 8Mb \n");
                    printf("Voltage : 5V \n");
                    break;
                case 0xBF53 :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : SST36VF3204 \n");
                    printf("Capacity : 32Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                case 0xBF5B :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : SST39VF3201 \n");
                    printf("Capacity : 32Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                case 0xBF5A :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : SST39VF3202 \n");
                    printf("Capacity : 32Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                case 0xBF6B :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : SST38VF6401 \n");
                    printf("Capacity : 64Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                default :
                    printf("No compatible Flash detected \n");
                }
            }
            else
            {
                printf("No Microchip / SST or compatible Flash detected \n");
            }

            printf("\n2) Try to detect STMicroelectronics flash memory :");
            printf(" Flash ID : %04X \n\n",flash_id);

            if ( flash_id != rom_id )
            {
                switch(flash_id)
                {
                case 0x2057 :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : M29W320EB \n");
                    printf("Capacity : 32Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                case 0x20ED :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : M29W640D \n");
                    printf("Capacity : 64Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                default :
                    printf("No compatible Flash detected \n");
                }
            }
            else
            {
                printf("No STMicroelectronics flash detected \n");
            }

            printf("\n3) Try to detect Macronix flash memory :");
            printf(" Flash ID : %04X \n\n",flash_id);

            if ( flash_id != rom_id )
            {
                switch(flash_id)
                {
                case 0xC2CB :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : MX29LV640 \n");
                    printf("Capacity : 64Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                case 0xC2C9 :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : MX29LV640 \n");
                    printf("Capacity : 64Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                case 0xC2A7 :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : MX29LV320 \n");
                    printf("Capacity : 32Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                case 0xC2A8 :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : MX29LV320 \n");
                    printf("Capacity : 32Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                default :
                    printf("No compatible Flash detected \n");
                }
            }
            else
            {
                printf("No Macronix Flash detected \n");
            }

            printf("\n4) Try to detect Spansion flash memory :");
            printf(" Flash ID : %04X \n\n",flash_id);

            if ( flash_id != rom_id )
            {
                switch(flash_id)
                {
                case 0x017E :
                    printf("Flash Memory detected ! \n");
                    printf("Memory : S29GL064 \n");
                    printf("Capacity : 64Mb \n");
                    printf("Voltage : 3.3V \n");
                    break;
                default :
                    printf("No compatible Flash detected \n");
                }
            }
            else
            {
                printf("No Spansion Flash detected \n");
            }

            // Try to detect MX29F1610 special detection

            printf("\n5) Try to detect Macronix MX29F1610 flash memory :");
            usb_buffer_out[0] = INFOS_ID;
            usb_buffer_out[5] = Cartridge_Type;
            libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 6000);
            libusb_bulk_transfer(handle, 0x82, usb_buffer_in, sizeof(usb_buffer_in), &numBytes, 6000);

            /* printf("\nDisplaying USB IN buffer\n\n");

                for (i = 0; i < 64; i++)
                 {
                     printf("%02X ",usb_buffer_in[i]);
             		j++;
             		if (j==16){printf("\n");j=0;}
                 }*/

            manufacturer_id = usb_buffer_in[5];
            chip_id = usb_buffer_in[8];
            flash_id = (manufacturer_id<<8) | chip_id;
            printf(" Flash ID : %04X \n\n",flash_id);

            switch(flash_id)
            {
            case 0xC2F1 :
                printf("Flash Memory detected ! \n");
                printf("Memory : MX29F1610 \n");
                printf("Capacity : 16Mb \n");
                printf("Voltage : 5V \n");
                break;
            default :
                printf("No compatible Flash detected \n");
            }


            break;

        case 0 : // Exit

            printf("Debug for ARM2SNES Cartridge... \n \n");
            Cartridge_Type = 2;
            address=32688;
            i=0;
            j=0;
            //address=0;

            printf("Sending command Dump ROM \n");
            printf("Dumping please wait ...\n");
            address=0;
            game_size = 1024*512;
            printf("\nRom Size : %ld Ko \n",game_size/1024);
            BufferROM = (unsigned char*)malloc(game_size);
            // Cleaning Buffer

            address=0;
            i=0;
            for (i=0; i<game_size; i++)
            {
                BufferROM[i]=0x00;
            }
            i=0;

            // Start Dump in slow mode

            while ( i< game_size)
            {
                usb_buffer_out[0] = READ_SNES;
                usb_buffer_out[1] = address & 0xFF ;
                usb_buffer_out[2] = (address & 0xFF00)>>8;
                usb_buffer_out[3]=(address & 0xFF0000)>>16;
                usb_buffer_out[4] = 0; // Slow Mode
                usb_buffer_out[5] = Cartridge_Type;
                libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
                libusb_bulk_transfer(handle, 0x82,(BufferROM+i),64, &numBytes, 60000);
                address +=64; //next adr
                i+=64;

                printf("\r ROM dump in progress: %ld%%", ((100 * i)/game_size));
                fflush(stdout);
            }

            printf("\nDump ROM completed !");
            myfile = fopen("dump_rom.sfc","wb");
            fwrite(BufferROM,1,game_size, myfile);
            fclose(myfile);





            return 0;
            break;

        case 20 : // Exit
            return 0;
            break;


        }
	
	return 0;
}
