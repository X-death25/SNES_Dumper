/*
 * \file main.c
 * \brief Libusb software for communicate with STM32
 * \author X-death for Ultimate-Consoles forum (http://www.ultimate-consoles.fr)
 * \date 2018/06
 *
 * just a simple sample for testing libusb and new code of Sega Dumper
 *
 * --------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include <libusb.h>
#include <assert.h>

#include <time.h>

// Specific Win32 lib ( only used for debug )

#include <conio.h>

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

// Cartridge Type

/*
1 : LoROM <= 4 Mb
2 : LoROM >= 16 Mb
3 : HiROM <= 16 Mb
*/

// Flash Memory Type

/*
1 : Microchip SST
*/

// SNES Dumper Specific Function

void pause(char const *message)
{
    int c;

    printf("%s", message);
    fflush(stdout);

    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

#if defined(_WIN32)
clock_t microsec_start;
clock_t microsec_end;
#else
struct timeval ostime;
long microsec_start = 0;
long microsec_end = 0;
#endif

void timer_start()
{
#if defined(_WIN32)
    microsec_start = clock();
#else
    gettimeofday(&ostime, NULL);
    microsec_start = ((unsigned long long)ostime.tv_sec * 1000000) + ostime.tv_usec;
#endif
}

void timer_end()
{
#if defined(_WIN32)
    microsec_end = clock();
#else
    gettimeofday(&ostime, NULL);
    microsec_end = ((unsigned long long)ostime.tv_sec * 1000000) + ostime.tv_usec;
#endif
}

void timer_show()
{
#if defined(_WIN32)
    printf("~SNES: Elapsed time: %lds", (microsec_end - microsec_start)/1000);
    printf(" (%ldms)\n", (microsec_end - microsec_start));
#else
    printf("~SNES: Elapsed time: %lds", (microsec_end - microsec_start)/1000000);
    printf(" (%ldms)\n", (microsec_end - microsec_start)/1000);
#endif
}

int main()
{

    // LibUSB Specific Var

    int res                      = 0;        /* return codes from libusb functions */
    libusb_device_handle* handle = 0;        /* handle for USB device */
    int kernelDriverDetached     = 0;        /* Set to 1 if kernel driver detached */
    int numBytes                 = 0;        /* Actual bytes transferred. */
    unsigned char usb_buffer_in[64] = {0};   /* 64 byte transfer buffer IN */
    unsigned char usb_buffer_out[64] = {0};  /* 64 byte transfer buffer OUT */
    unsigned long len            = 0;        /* Number of bytes transferred. */

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

    unsigned char Hardwaretype=0;
    unsigned short Hardwaredata=0;
    unsigned long Hardwaresize=0;

//*****************************************************
// Hardware TYPES
// 1 = ExHirom
// 2 = DSP-1
// 3 = DSP-2
// 4 = DSP-3
// 5 = DSP-4
// 6 = Capcom Cx4
// 7 = Ex-HiROM
// 8 = S-DD1
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
        0x8560, 0x0601,  // Rockman X2 Jap
        0x01F7, 0x0804,  // Street Fighter Zero 2 (Japan)
        0xDB9A, 0x0804,  // Street Fighter Zero 2 (USA)
        0x13B8, 0x0806,  // Star Ocean (Japan)
    };


    // Main Program

    printf("\n");
    printf(" ---------------------------------\n");
    printf("    SNES Dumper USB2 Software     \n");
    printf(" ---------------------------------\n");
    printf("\n");

    printf("Init LibUSB... \n");

    /* Initialise libusb. */

    res = libusb_init(0);
    if (res != 0)
    {
        fprintf(stderr, "Error initialising libusb.\n");
        return 1;
    }

    printf("LibUSB Init Sucessfully ! \n");


    printf("Detecting SNES Dumper... \n");

    /* Get the first device with the matching Vendor ID and Product ID. If
     * intending to allow multiple demo boards to be connected at once, you
     * will need to use libusb_get_device_list() instead. Refer to the libusb
     * documentation for details. */

    handle = libusb_open_device_with_vid_pid(0, 0x0483, 0x5740);

    if (!handle)
    {
        fprintf(stderr, "Unable to open device.\n");
        return 1;
    }

    /* Claim interface #0. */

    res = libusb_claim_interface(handle, 0);
    if (res != 0)
    {
        res = libusb_claim_interface(handle, 1);
        if (res != 0)
        {
            printf("Error claiming interface.\n");
            return 1;
        }
    }

// Clean Buffer
    for (i = 0; i < 64; i++)
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


    for (i = 0; i < 64; i++)
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








    }






