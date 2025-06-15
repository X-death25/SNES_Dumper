int Dump_SNES_ROM(int mode)
{
    if(mode==2)
  	{
  		Cartridge_Type = 2;
  		game_size = game_size * 1024;
  	}
  	else if(mode==3)
  	{
  		Cartridge_Type = 4;
  		game_size = game_size * 1024;
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
	}
}

int Dump_SNES_Save(int mode)
{
       if(mode==1)	save_size = Ram_Size;
        else		save_size *= 1024;

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
        return 0;
}
