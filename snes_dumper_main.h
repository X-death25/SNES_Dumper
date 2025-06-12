#ifdef SDLGUI	
// SDL Libraries for GUI
#include <SDL.h>												//Main Library file
#include <SDL_image.h>											//For PNG loading files
#define printf SDL_Log
#endif

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
	for (i = 0; i < 64; i++)
	{
		usb_buffer_in[i]=0x00;
		usb_buffer_out[i]=0x00;
	}
	i=0;
	
	return 0;
}
