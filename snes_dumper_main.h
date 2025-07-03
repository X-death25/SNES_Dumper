#ifdef SDLGUI	
// SDL Libraries for GUI
#include <SDL.h>												//Main Library file
#include <SDL_image.h>											//For PNG loading files
#define printf SDL_Log
#endif

int use_gui=0;

#include <libusb.h>												//Library for detecting the SNES Dumper device


//File Manager Library
#include "libs/sfd.h"
#include "libs/sfd.c"

// USB Special Command

#define WAKEUP               0x10     // WakeUP for first STM32 Communication
#define READ_SNES            0x11     // Used to read Snes ROM when cartridge info is Known
#define READ_SNES_SAVE       0x12     // Used to read Snes SRAM
#define ERASE_SNES_SAVE      0x13     // Used to clear SNES SRAM
#define WRITE_SNES_SAVE      0x14     // Used to write SNES SRAM
#define ERASE_SNES_FLASH     0x15	  // Used to fully format R/W Snes Cartridge
#define WRITE_SNES_FLASH     0x16	  // Used to Write ROM on R/W Snes Cartridge
#define INFOS_ID             0x18     // Used to detect what type of Flash is on the cartridge
#define READ_SNES_HEADER     0x21     // Used to determine what type of cartridge is inserted

#define READ_SDD1_BANK       0x30     // Used to Read SDD-1 Register ROM BANK
#define WRITE_SDD1_BANK      0x31     // Used to Write SDD-1 Register ROM BANK
#define READ_CX4_BANK        0x32     // Used to Read CX-4 Register ROM BANK
#define WRITE_CX4_BANK       0x33     // Used to Write CX-4 Register ROM BANK


#define CREATE_MX_BUFFER   0x60
#define WRITE_MX_BUFFER    0x61
#define FLASH_MX_BUFFER    0x62

// Version

#define MAX_VERSION 	1
#define MIN_VERSION 	0

char * game_rom						= NULL;
char * game_name					= NULL;


//CSV Flashlist specific Variables

#define CHIPID_TEXT_SIZE 48 // taille de toute la chaine
#define TEXT_SIZE2 48

int current_row2					= 0;
int current_col2					= 0;
int non_empty_cells_in_col_A2				= 0; 						// Compteur de cellules non vides en colonne A
char chipid_text_values[MAX_NON_EMPTY_CELLS][CHIPID_TEXT_SIZE + 1];
int chipid_text_values_count 				= 0;

char txt_csv_deviceID[4];
unsigned short csv_deviceID=0;
unsigned char txt_csv_flash_size[3+1];
unsigned char csv_flash_size=0;
unsigned long flash_size=0;
unsigned char txt_csv_erase_algo[2+1];
unsigned char csv_erase_algo=0;
unsigned char txt_csv_write_algo[2+1];
unsigned char csv_write_algo=0;
unsigned char txt_csv_flash_name[11+1];
unsigned char txt_csv_man_name[18+1];
unsigned char txt_csv_voltage[2+1];
unsigned char csv_voltage=0;
unsigned char flash_algo=0;

FILE *fp2;
struct csv_parser p2;


// LibUSB Specific Var
int res							= 0;						/* return codes from libusb functions */
unsigned char usb_buffer_out[64]			= {0};  					/* 64 byte transfer buffer OUT */
libusb_device_handle* handle 				= 0;	   					/* handle for USB device */
int numBytes						= 0;						/* Actual bytes transferred. */
unsigned char usb_buffer_in[64]				= {0};   					/* 64 byte transfer buffer IN */
int device_found					= -1;
int if_num;
int if_num_max						= 2;
#define VENDOR_ID					0x0483
#define PRODUCT_ID   					0x5740
libusb_context *context					= NULL;
libusb_device **devs;
ssize_t count;													// holding number of devices in list
   

// SNES Dumper Var

    // Dumper Specific Var

    unsigned long i=0;
    unsigned long j=0;
    unsigned long k=0;
    int choixMenu=0;
    unsigned long address=0;
    int game_size=0;
    unsigned char *BufferROM;
    unsigned char *Buffer1;
    unsigned char *Buffer2;
    unsigned long save_size = 0;
    unsigned char *buffer_save = NULL;
    unsigned char buffer_temp1[32];
    unsigned char buffer_temp2[32];
    unsigned char Retry=0;
    unsigned char dump_name[64];
    unsigned char Flash_type=0;

    // Rom Header info


    unsigned char Cartridge_Detected=0;
    unsigned char Cartridge_Type=0;
    unsigned char Game_Name[21];
    unsigned char Rom_Type=0;
    unsigned long Rom_Size=0;
    unsigned long Ram_Size=0;
    unsigned char Rom_Version=0;
    unsigned char Rom_Region=0;
    int checksum_header = 0;


    // Custom Cartridges specific var

    unsigned char cx4Type=0;
    unsigned char SDD1_ROM_Register=0;

    // Flash specific var

    unsigned char manufacturer_id=0;
    unsigned char chip_id=0;
    unsigned short flash_id=0;
    unsigned short rom_id=0;
    const char * wheel[] = { "-","\\","|","/"}; //erase wheel

    // File manipulation Specific Var

    FILE *myfile;

//*********************************************************
// SNES SPECIAL Cartridge LOOKUP TABLE
// Format = {chksum, Hardwaretype | CartrdigeSize}
// chksum is located in ROM Header $FFDE
// Cartidge Type and Size are combined to conserve memory
//*********************************************************
// Based on
// https://en.wikipedia.org/wiki/List_of_Super_NES_enhancement_chips
//

    unsigned char Hardwaretype=0;
    unsigned short Hardwaredata=0;
    unsigned long Hardwaresize=0;

//*****************************************************
// Hardware TYPES
// 1 = not used / Reserved
// 2 = DSP-1
// 3 = DSP-2
// 4 = DSP-3
// 5 = DSP-4
// 6 = Capcom Cx4
// 7 = Ex-Hirom
// 8 = S-DD1
// 9 = Super FX 1
// A = Super FX 2
// B = SA-1
//*****************************************************

    static const unsigned short HardwareID [] =
    {
        0x8528, 0x0705,  // Daikaijuu Monogatari II
        0x7C57, 0x0706,  // Tales Of Phantasia JAP
        0xFE6E, 0x0706,  // Tales Of Phantasia Fr by DeJap
        0x432E, 0x0706,  // Trials Of Mana rev1
        0x8A23, 0x0200,  // Super Mario Kart (J)
        0x52B8, 0x0200,  // Super Mario Kart (E)
        0xEB44, 0x0200,  // Super Mario Kart (U)
        0x5672, 0x0201,  // Ace wo Nerae!
        0x3858, 0x0201,  // Soukou Kihei Votoms
        0xC0BB, 0x0201,  // Ballz 3D (J)
        0x155E, 0x0201,  // Ballz 3D (U)
        0xFEB5, 0x0201,  // Battle Racers (J)
        0x82D5, 0x0203,  // Drift King - Shutokou Battle 2 (J)
        0x646E, 0x0203,  // Final Stretch (J)
        0x6070, 0x0200,  // Hashiriya Tamashii - Rider's Spirits (J)
        0x2485, 0x0203,  // Korean League (K)
        0x5971, 0x0203,  // Super Bases Loaded 2 (U)
        0x8BD6, 0x0203,  // Super 3D Baseball (J)
        0xA185, 0x0200,  // Lock On (U)
        0x21ED, 0x0201,  // Michael Andretti's Indy Car Challenge (J)
        0x9CF9, 0x0201,  // Michael Andretti's Indy Car Challenge (U)
        0xE30C, 0x0200,  // Pilotwings (E)
        0x3163, 0x0200,  // Pilotwings (U)
        0xCCF7, 0x0200,  // Pilotwings (J)
        0x8640, 0x0200,  // Super Air Diver (E)
        0x137A, 0x0200,  // Super Air Diver (J)
        0xBEC2, 0x0202,  // Super Air Diver 2 (J)
        0x2A6B, 0x0203,  // Drift King Shutokou Battle '94
        0x587F, 0x0202,  // Super F1 Circus Gaiden (J)
        0x3706, 0x0201,  // Suzuka 8 Hours (J)
        0xF5DF, 0x0201,  // Suzuka 8 Hours (U)
        0x8560, 0x0601,  // Rockman X2 Jap  || CAPCOM CX2
        0x01F7, 0x0804,  // Street Fighter Zero 2 (Japan) || SDD-1
        0xDB9A, 0x0804,  // Street Fighter Zero 2 (USA) || SDD-1
        0x13B8, 0x0806,  // Star Ocean (Japan) || SDD-1
		0x730E, 0x0A02,  // Super Mario World 2 (Japan) || Super FX2
        0x4575, 0x0B04,  // Super Mario RPG (Japan) || SA-1
        0x64F0, 0x0B04,  // Dragon Ball Z Hyper Dimension (Japan) || SA-1
    };

//Timer functions according to Operating Systems

#if defined(_WIN32)		//Windows
	clock_t microsec_start;
	clock_t microsec_end;

	void timer_start()
	{
		microsec_start = clock();
	}
	
	void timer_end()
	{
		microsec_end = clock();
	}

	void timer_show()
	{
		printf("~ Elapsed time: %lds", (microsec_end - microsec_start)/1000);
		printf(" (%ldms)\n", (microsec_end - microsec_start));
	}
#else 				//Others
	struct timeval ostime;
	long microsec_start = 0;
	long microsec_end = 0;

	void timer_start()
	{
		gettimeofday(&ostime, NULL);
		microsec_start = ((unsigned long long)ostime.tv_sec * 1000000) + ostime.tv_usec;
	}

	void timer_end()
	{
		gettimeofday(&ostime, NULL);
		microsec_end = ((unsigned long long)ostime.tv_sec * 1000000) + ostime.tv_usec;
	}

	void timer_show()
	{
		printf("~ Elapsed time: %lds", (microsec_end - microsec_start)/1000000);
		printf(" (%ldms)\n", (microsec_end - microsec_start)/1000);
	}
#endif

void Display_Help(char *prog_name)
{
	printf("\n");
	printf("How to use the program:\n");
	printf("\n");
	printf("  %s -dumprom (auto|lorom|hirom|exhirom) (auto|32|64|128|256|512|1024|2048|4096|8192|16384|32768) - Dump ROM\n", prog_name);
	printf("  %s -dumpsave (auto|lorom|hirom|exhirom) (auto|8|16|32|64|128|256|512) - Dump Save\n", prog_name);
	printf("  %s -writesave - Write Save\n", prog_name);
	printf("  %s -eraseesave  - Erase Save\n", prog_name);
	printf("  %s -writeflash - Write Flash\n", prog_name);
	printf("  %s -eraseflash (auto|lorom|hirom|exhirom|lorommx29f1610) - Erase Flash\n", prog_name);
	printf("  %s -detectflash - Detect Flash\n", prog_name);
	printf("  %s -debug - Debug Mode\n", prog_name);
	printf("\n");
}


int Detect_Device(void)
{		
	printf("Init LibUSB... \n");
	res = libusb_init(&context);
	if (res != 0)
	{
		printf("Error initialising libusb.\n");
		return 1;
	}
	printf("LibUSB Init Sucessfully ! \n");

	printf("Detecting SNES Dumper... \n");
	count = libusb_get_device_list(context, &devs);
	if (count <= 0)
	{
		printf("Error getting device list\n");
		return 1;
	}

	for (size_t idx = 0; idx < count; ++idx)
	{
		libusb_device *device = devs[idx];
		struct libusb_device_descriptor desc = {0};

		res = libusb_get_device_descriptor(device, &desc);
		assert(res == 0);

		if(desc.idVendor=VENDOR_ID && desc.idProduct==PRODUCT_ID) device_found=idx;
	}
	
	if(device_found!=-1)
	{
		printf("SNES Dumper Device Found !\n");
		libusb_device *device = devs[device_found];
		struct libusb_device_descriptor desc = {0};
		res = libusb_get_device_descriptor(device, &desc);

		printf("LibUSB Device ID = %d\n",device_found);
		printf("LibUSB Device Vendor = %04x:%04x\n",desc.idVendor,desc.idProduct);
		
		libusb_open(device, &handle);
		printf("Handle result : %s\n", libusb_error_name(res));
		
		if (!handle)
		{
			printf("Unable to open SNES Dumper Device.\n");
			return 1;
		}
		else
		{
			printf("SNES Dumper Device opened !\n");

			for (if_num = 0; if_num < if_num_max; if_num++) 
			{
				res = libusb_kernel_driver_active(handle, if_num);
				if (res)
				{
					libusb_detach_kernel_driver(handle, if_num);
				}
				res = libusb_claim_interface(handle, if_num);
				if (res < 0) 
				{
					printf("Error claiming interface %d: %s\n", if_num, libusb_error_name(res));
					if(if_num==if_num_max-1)
					{
						printf("Exiting...");
						return 1;
					}
				}
				else
				{
					printf("Interface %d claimed\n", if_num);
					if_num=2;
				}
			}
		}
	}
	else
	{
		printf("SNES Dumper Device Not Found !\n");
		return 1;
	}

	// Clean Buffer
	for (int i = 0; i < 64; i++)
	{
		usb_buffer_in[i]=0x00;
		usb_buffer_out[i]=0x00;
	}

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

	/*printf("\nDisplaying USB IN buffer\n\n");
	for (i = 0; i < 64; i++)
	{
		printf("%02X ",usb_buffer_in[i]);
		j++;
        	if (j==16)
		{
			printf("\n");
			j=0;
		}
    	}*/

	for (i = 0; i < 64; i++)
		{
        	usb_buffer_in[i]=0x00;
        	usb_buffer_out[i]=0x00;
    		}
	
	return 0;
}

void Game_Infos(void)
{
	printf("\nDetecting Cartridge type...\n");
	while ( Cartridge_Detected == 0  )
	{
        	Cartridge_Type++;
		printf("Try to detect with cartridge type %d \n",Cartridge_Type);
	        if ( Cartridge_Type == 2 ) // LoROM Cartridge < 16 Mb
        	{
            		address=32688;
			usb_buffer_out[0] = READ_SNES;
			usb_buffer_out[1] = address & 0xFF ;
			usb_buffer_out[2] = (address & 0xFF00)>>8;
			usb_buffer_out[3]=(address & 0xFF0000)>>16;
			usb_buffer_out[4] = 0; // Slow Mode
			usb_buffer_out[5] = Cartridge_Type;
			libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
			libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

			/*printf("\nDisplaying USB IN buffer\n\n");
			for (i = 0; i < 64; i++)
			{
				printf("%02X ",usb_buffer_in[i]);
				j++;
				if (j==16)
				{
					printf("\n");
					j=0;
				}
			}*/
			
			Rom_Type=usb_buffer_in[37]; // Cartridge Format
            		if ( Rom_Type == 0x20 || Rom_Type == 0x30) Cartridge_Detected = 1;
          
			Rom_Type=usb_buffer_in[38]; // Cartridge Format
            		cx4Type = usb_buffer_in[25];
			if ( usb_buffer_in[25] == 0x32 )	cx4Type=2;   // X2
		        else if ( usb_buffer_in[25] == 0x33 )	cx4Type=3;   // X3
            
            		//printf("\nLa valeur de Cx4 Type est %d",cx4Type);
            		if ( Rom_Type == 0xF3)
            		{
                		Cartridge_Detected = 1;
                		Cartridge_Type = 6;
            		}
		}
        	if ( Cartridge_Type == 4  ) // HiROM Cartridge
        	{
            		address=65456;
			usb_buffer_out[0] = READ_SNES;
			usb_buffer_out[1] = address & 0xFF ;
			usb_buffer_out[2] = (address & 0xFF00)>>8;
			usb_buffer_out[3]=(address & 0xFF0000)>>16;
			usb_buffer_out[4] = 0; // Slow Mode
			usb_buffer_out[5] = Cartridge_Type;
			libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
			libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

			/*printf("\nDisplaying USB IN buffer\n\n");
			for (i = 0; i < 64; i++)
		        {
		        	printf("%02X ",usb_buffer_in[i]);
		                j++;
		                if (j==16)
		                {
		                	printf("\n");
					j=0;
		                }
		        }*/

			Rom_Type=usb_buffer_in[37]; // Cartridge Format
			if ( Rom_Type == 0x21 || Rom_Type == 0x30 || Rom_Type == 0x31) Cartridge_Detected = 1;
		}
	        if ( Cartridge_Type == 5  ) // DSP1 Cartridge
        	{
			address=65456;
			usb_buffer_out[0] = READ_SNES;
			usb_buffer_out[1] = address & 0xFF ;
			usb_buffer_out[2] = (address & 0xFF00)>>8;
			usb_buffer_out[3]=(address & 0xFF0000)>>16;
			usb_buffer_out[4] = 0; // Slow Mode
			usb_buffer_out[5] = Cartridge_Type;
			libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
			libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

			/*printf("\nDisplaying USB IN buffer\n\n");
			for (i = 0; i < 64; i++)
		        {
		        	printf("%02X ",usb_buffer_in[i]);
		                j++;
		                if (j==16)
		                {
		                	printf("\n");
					j=0;
		                }
		        }*/
			
			Rom_Type=usb_buffer_in[37]; // Cartridge Format
            if ( Rom_Type == 0x21 || Rom_Type == 0x31 ) Cartridge_Detected = 1;
		}
		if ( Cartridge_Type ==  6 ) // Capcom CX4 Cartridge
		{
			address=65456;
			usb_buffer_out[0] = READ_SNES;
			usb_buffer_out[1] = address & 0xFF ;
			usb_buffer_out[2] = (address & 0xFF00)>>8;
			usb_buffer_out[3]=(address & 0xFF0000)>>16;
			usb_buffer_out[4] = 0; // Slow Mode
			usb_buffer_out[5] = 3;
			libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
			libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

			/*printf("\nDisplaying USB IN buffer\n\n");
			for (i = 0; i < 64; i++)
		        {
		        	printf("%02X ",usb_buffer_in[i]);
		                j++;
		                if (j==16)
		                {
		                	printf("\n");
					j=0;
		                }
		        }*/

            		Rom_Type=usb_buffer_in[38]; // Cartridge Format
			cx4Type = usb_buffer_in[25];
            		if ( usb_buffer_in[25] == 0x32 )	cx4Type=2;   // X2
            		else if ( usb_buffer_in[25] == 0x33 )	cx4Type=3;   // X3
            
            		if ( Rom_Type == 0xF3)
            		{
                		Cartridge_Detected = 1;
                		Cartridge_Type = 6;
            		}
		}
		if ( Cartridge_Type == 7  ) // ExHiROM Cartridge
		{
			address=65456;
			usb_buffer_out[0] = READ_SNES;
			usb_buffer_out[1] = address & 0xFF ;
			usb_buffer_out[2] = (address & 0xFF00)>>8;
			usb_buffer_out[3]=(address & 0xFF0000)>>16;
			usb_buffer_out[4] = 0; // Slow Mode
			usb_buffer_out[5] = Cartridge_Type;
			libusb_bulk_transfer(handle, 0x01,usb_buffer_out, sizeof(usb_buffer_out), &numBytes, 60000);
			libusb_bulk_transfer(handle, 0x82,usb_buffer_in,64, &numBytes, 60000);

			/*printf("\nDisplaying USB IN buffer\n\n");
			for (i = 0; i < 64; i++)
		        {
		        	printf("%02X ",usb_buffer_in[i]);
		                j++;
		                if (j==16)
		                {
		                	printf("\n");
					j=0;
		                }
		        }*/
        	}
			
		if ( Cartridge_Type > 15)
        	{
            		Cartridge_Detected = 1;
            		printf("\nCan't detect Cartridge type...");
        	}
	}
	
	printf("\nCartridge Type :  %d ",Cartridge_Type);
	
	printf("\n\n --- Cartridge INFO ---");
	for (i=0; i<22; i++)		Game_Name[i]=usb_buffer_in[i+16];   // ROM Name
    	printf("\nGame name :  %.*s",21,(char *) Game_Name);
	
	Rom_Type=usb_buffer_in[37]; // Cartridge Format
    	if ( (Rom_Type & 0x01) == 1) 	printf("\nCartridge format : HiROM");
    	else				printf("\nROM Map : LoROM");
	
	game_size= (0x400 << usb_buffer_in[39]); // Rom Size
	printf("\nGame Size :  %ld Ko / %ld Mbits",game_size/1024, ((game_size/1024)*8)/1024);
	
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
	
    	if ( (Rom_Type >> 4 & 0x03) == 3)	printf("\nROM Speed : FastROM");
	else					printf("\nROM Speed : SlowROM");
	
	Rom_Region=usb_buffer_in[41];
    	if (Rom_Region == 0)			printf("\nGame Region : Japan");
    	else if (Rom_Region == 1)		printf("\nGame Region : USA");
    	else if (Rom_Region == 2)		printf("\nGame Region : Europe");
	else					printf("\nGame Region is : Unknown");
	
	Rom_Region=usb_buffer_in[43];
	printf("\nGame Version : 1.%d ",Rom_Version);
	checksum_header = (usb_buffer_in[47]<<8) | usb_buffer_in[46];
	printf("\nHeader Checksum : %X", checksum_header);
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

	//Temp
	if (checksum_header == 0xE30C || checksum_header == 0x3163 || checksum_header == 0xCCF7) // Pilotwings (E), Pilotwings (U),  Pilotwings (J)
		Cartridge_Type = 2;
	
	printf("\nCartridge type : %ld \n",Cartridge_Type);
}
