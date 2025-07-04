int Write_SNES_Save(void)
{
    printf("Writing SNES Save...");
        myfile = fopen("dump_rom.srm","rb");
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
        return 0;
}

int Erase_SNES_Save(void)
{
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
        return 0;
}

int Write_SNES_Flash(int write_mode)
{
        i=0;
        unsigned char flash_MX29F1610=0;

        if ( write_mode == 1 )
        {
            Cartridge_Type = 2;
            printf("\nCartridge type : LoRom  \n");
        }
        else if ( write_mode == 2 )
        {
            Cartridge_Type = 4;
            printf("\nCartridge type : HiRom  \n");
        }
        else if ( write_mode == 3 )
        {
            Cartridge_Type = 7;
            printf("\nCartridge type : ExHiRom  \n");
        }
        else if ( write_mode == 4 )
        {
            Cartridge_Type = 2;
            printf("\nCartridge type : LoRom MX29F1610  \n");
            flash_MX29F1610=1;
        }
        else 
          {
            printf("\nWrong option\n");
            return 0;
          }


        if ( flash_MX29F1610 == 1 )
        {
            myfile = fopen("dump_rom.sfc","rb");
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
            return 0;

           }

            else
            {

                // Sending ROM to Buffer

                myfile = fopen("dump_rom.sfc","rb");
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
            return 0;
}

int Erase_SNES_Flash(int erase_mode)
{
            
            i=0;
            for (i = 0; i < 32; i++)
            {
                printf("\n");
            }

            if ( erase_mode == 2 )
            {
                Cartridge_Type = 2;
            }
            if ( erase_mode == 3 )
            {
                Cartridge_Type = 4;
            }
            if ( erase_mode == 5 )
            {
                Cartridge_Type = 4;
                usb_buffer_out[6] = 2;
            }

           // usb_buffer_out[0] = ERASE_SNES_FLASH;
            // usb_buffer_out[5] = Cartridge_Type;
            printf("Starting Flash Erase ...\n");
            printf("Detect Flash inside the cartridge ...\n");

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

            manufacturer_id = usb_buffer_in[5];
            chip_id = usb_buffer_in[8];
            flash_id = (manufacturer_id<<8) | chip_id;

            printf("Flash ID : %04X \n\n",flash_id);

        for (i = 0; i < chipid_text_values_count; i++)
		{
			strncpy(txt_csv_deviceID,chipid_text_values[i],4);
			//printf(" \n txt chipid value : %s \n",txt_csv_deviceID);
			csv_deviceID = (unsigned short)strtol(txt_csv_deviceID, NULL, 16);
			//printf(" \n DEC Device ID value : %ld \n",csv_deviceID);
			// If found we need to copy all usefull info from CSV to MD dumper Var

			if ( flash_id == csv_deviceID  )
			{
				printf("Found chip in CSV Flashlist ! \n");
				printf("Position in csv table %d \n",i);

				// Flash Size
				strncpy(txt_csv_flash_size,chipid_text_values[i]+5,3);
				txt_csv_flash_size[4] = '\0'; // Null-terminate the output string
				printf("Txt flash size : %s \n",txt_csv_flash_size);
				csv_flash_size = (unsigned char)strtol(txt_csv_flash_size, NULL, 10);
				printf("CSV Flash Size  %d \n",csv_flash_size);
				flash_size=1024*csv_flash_size;
				printf("La valeur de FlashSize est %ld Ko \n",flash_size);

				// Algo Erase
				strncpy(txt_csv_erase_algo,chipid_text_values[i]+9,2);
				txt_csv_erase_algo[2] = '\0'; // Null-terminate the output string
				printf("Txt Erase Algo : %s \n",txt_csv_erase_algo);
				csv_erase_algo = (unsigned char)strtol(txt_csv_erase_algo, NULL, 8);
				printf("CSV Erase Algo  %d \n",csv_erase_algo);

				// Write Algo
				strncpy(txt_csv_write_algo,chipid_text_values[i]+12,2);
				txt_csv_write_algo[2] = '\0'; // Null-terminate the output string
				printf("Txt Write Algo  : %s \n",txt_csv_write_algo);
				csv_write_algo = (unsigned char)strtol(txt_csv_write_algo, NULL, 8);
				printf("CSV Write Algo %d \n",csv_write_algo);

				// Chip Name
				strncpy(txt_csv_flash_name,chipid_text_values[i]+15,11);
				txt_csv_flash_name[12] = '\0'; // Null-terminate the output string
				printf("Flash Device Reference : %s \n",txt_csv_flash_name);

				// Voltage
				strncpy(txt_csv_voltage,chipid_text_values[i]+27,2);
				txt_csv_voltage[2] = '\0'; // Null-terminate the output string
				printf("Txt Chip Voltage : %s \n",txt_csv_voltage);
				csv_voltage = (unsigned char)strtol(txt_csv_voltage, NULL, 8);
				printf("CSV Chip Voltage  %d \n",csv_voltage);

				// Manufacturer
				strncpy(txt_csv_man_name,chipid_text_values[i]+30,18);
				txt_csv_man_name[19] = '\0'; // Null-terminate the output string
				printf("Chip Manufacturer : %s \n",txt_csv_man_name);

				printf("Memory : %s \n",txt_csv_flash_name);
				printf("Capacity %ld Ko \n",flash_size);
				printf("Chip Manufacturer : %s \n",txt_csv_man_name);
				printf("Chip Voltage %ld V \n",csv_voltage);
				printf("CSV Erase Algo  %d \n",csv_erase_algo);
				printf("CSV Write Algo %d \n",csv_write_algo);
			}
		}





            /*timer_start();
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

            return 0;
            */
}

int Detect_SNES_Flash(void)
{
// Vendor / ID Info
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


            return 0;
}

int Debug_Mode(void)
{
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
}
