/*
 * microME Teensy Co-Processor sample firmware
 * Copyright (C) 2019 D.Herrendoerfer 
 *                    <d.herrendoerfer@herrendoerfer.name>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ************************************************************************/

File root;

File remoteFile[4];
uint8_t file_status[4] = {0,0,0,0};


uint8_t sd_present;

void sdcard_init()
{
  SPI.setMOSI(11);
  SPI.setMISO(12);
  SPI.setSCK(14);

  Serial.println("Initializing SD card...");

  if (!SD.begin(10)) {
    sd_present = 0;
    return;
  }
  else {
    sd_present = 1;  
  }

}

void sdcard_msg()
{
}

void sdcard_file(uint8_t fd)
{
  uint8_t type = SERMSG_MSGTYPE;
  uint8_t filename[MAX_FILENAME_LENGTH];
  uint8_t i;
  
  if (type == SERMSG_SEND_VAR){
    // Open a file (ADDRESS: 1)
    if (SERMSG_ADDRESS == 1) {
      uint8_t length=SERMSG_VAR_LENGTH;
      for (i=0; i<length; i++) {
        filename[i] = SERMSG_VAR_MESSAGE(i);          
      }
      filename[i] = 0;

      remoteFile[fd] = SD.open((char*)filename);
      if (remoteFile[fd]) {
        file_status[fd] = FILE_STATUS_OPEN;
        DEBUG_PRINT("Opened file: ");
        DEBUG_PRINTLN((char*)filename);
      }
    }
    else if (SERMSG_ADDRESS == 2) {
      uint8_t length=SERMSG_VAR_LENGTH;
      for (i=0; i<length; i++) {
        filename[i] = SERMSG_VAR_MESSAGE(i);          
      }
      filename[i] = 0;

      remoteFile[fd] = SD.open((char*)filename,FILE_WRITE);
      if (remoteFile[fd]) {
        file_status[fd] = FILE_STATUS_OPEN;
        file_status[fd] |= FILE_STATUS_WRITE;
        DEBUG_PRINT("Opened file (write): ");
        DEBUG_PRINTLN((char*)filename);
      }
    }
    else if (SERMSG_ADDRESS == 3) {
      uint8_t length=SERMSG_VAR_LENGTH;
      for (i=0; i<length; i++) {
        remoteFile[fd].write(SERMSG_VAR_MESSAGE(i));          
      }
      DEBUG_PRINT("Wrote: ");
      DEBUG_PRINT(length);
      DEBUG_PRINTLN("bytes.");
    }
    sermsg_send_confirm(TARGET_TEENSY, 1); //confirm all packets
  }
  else if (type == SERMSG_GET_SHORT){
    if (SERMSG_ADDRESS == 1) {
      // Get file status

      DEBUG_PRINT("Getting file status: ");
      DEBUG_PRINTLN((int)fd);

      sermsg_send_get_reply(TARGET_TEENSY, 1, file_status[fd]);        
    }
    else if (SERMSG_ADDRESS == 2) {
      // Get available()
      int avail = remoteFile[fd].available();

      DEBUG_PRINT("Getting avail: ");
      DEBUG_PRINTLN((int)avail);
      
      if (avail < FILE_TRANSFER_BUFFER)
        sermsg_send_get_reply(TARGET_TEENSY, 2, avail);
      else
        sermsg_send_get_reply(TARGET_TEENSY, 2, MAX_FILE_CHUNK_SIZE);
    }
    else if (SERMSG_ADDRESS == 3) {
      // Get available()
      if (remoteFile[fd].available())
        sermsg_send_get_reply(TARGET_TEENSY, 3, remoteFile[fd].read());
      else
        sermsg_send_get_reply(TARGET_TEENSY, 3, 0);
    }
    else if (SERMSG_ADDRESS == 4) {
      // Initialise sdcard again()
      root.close();
      sdcard_init();
      sermsg_send_get_reply(TARGET_TEENSY, 4, sd_present);
    }
  }
  else if (type == SERMSG_SEND_SHORT){
    if (SERMSG_ADDRESS == 1) {
      // Close file

      DEBUG_PRINT("Closing file: ");
      DEBUG_PRINTLN((int)fd);

      remoteFile[fd].close();
      file_status[fd] &= ~FILE_STATUS_OPEN;
      file_status[fd] &= ~FILE_STATUS_WRITE;
    }
    sermsg_send_confirm(TARGET_TEENSY, 1); //confirm all packets
  }
  else if (type == SERMSG_GET_VAR){
    if (SERMSG_ADDRESS == 1) {
      // Read up to 64 bytes of a file
      uint8_t i,data[FILE_TRANSFER_BUFFER];
      uint8_t length=SERMSG_VAR_LENGTH;

      DEBUG_PRINT("  requested data size: ");
      DEBUG_PRINTLN((int)length);

      DEBUG_PRINT("Reading file: ");
      DEBUG_PRINTLN((int)fd);

      i=remoteFile[fd].read(&data[1], length-1);

      DEBUG_PRINT("  return size: ");
      DEBUG_PRINTLN((int)i);

      data[0] = i;
      sermsg_send_var_get_reply(TARGET_TEENSY, 1, length, data);
    }
  }
}

uint8_t dir_status = 0;
static uint8_t dir_vfile[2048] = {0,0}; 
uint16_t dir_vfile_size = 0;
uint16_t dir_vfile_ptr = 0;

uint8_t gen_dir_vfile(File dir)
{
  char *spaces = "     ";
  char tmp[32] = {0};
  uint8_t i;
  
  dir_vfile_size = 0; //Start over new
  dir_vfile_ptr = 0;

  dir.rewindDirectory();
  
  while(true) {
   
   File entry =  dir.openNextFile();
   if (! entry) {
     break;
   }

   if (!entry.isDirectory()) {
     if (entry.size() != 0) {
       i = floor(log10(entry.size()));
     } else {
       i = 1; 
     }
    
     sprintf(tmp,"%d %s %s \n",entry.size(),&spaces[i],entry.name());
     i = strlen(tmp);
     
     memcpy(&dir_vfile[dir_vfile_size], tmp, i);
     dir_vfile_size += i;
     dir_vfile[dir_vfile_size+1] = 0;
   }
   entry.close();
  }
}

void sdcard_directory()
{
  uint8_t type = SERMSG_MSGTYPE;
  uint8_t filename[MAX_FILENAME_LENGTH];
  uint8_t i;

  if (type == SERMSG_SEND_VAR){
    // Open a file (ADDRESS: 1)
    if (SERMSG_ADDRESS == 1) {
      uint8_t length=SERMSG_VAR_LENGTH;
      for (i=0; i<length; i++) {
        filename[i] = SERMSG_VAR_MESSAGE(i);          
      }
      filename[i] = 0;

      root = SD.open("/");
      gen_dir_vfile(root);
      root.close();
      
      dir_status = FILE_STATUS_OPEN;

      DEBUG_PRINT("Opened file: ");
      DEBUG_PRINTLN((char*)filename);
    }
    sermsg_send_confirm(TARGET_TEENSY, 1); //confirm all packets
  }  
  else if (type == SERMSG_GET_SHORT){
    if (SERMSG_ADDRESS == 1) {
      // Get file status

      DEBUG_PRINT("Getting directory file status: ");

      sermsg_send_get_reply(TARGET_TEENSY, 1, dir_status);        
    }
    else if (SERMSG_ADDRESS == 2) {
      // Get available()
      int avail = dir_vfile_size - dir_vfile_ptr;

      DEBUG_PRINT("Getting avail: ");
      DEBUG_PRINTLN((int)avail);
      
      if (avail < FILE_TRANSFER_BUFFER)
        sermsg_send_get_reply(TARGET_TEENSY, 2, avail);
      else
        sermsg_send_get_reply(TARGET_TEENSY, 2, MAX_FILE_CHUNK_SIZE);
    }
  }
  else if (type == SERMSG_SEND_SHORT){
    if (SERMSG_ADDRESS == 1) {
      // Close file

      DEBUG_PRINT("Closing directory file: ");

      dir_vfile[0] = 0;
      dir_vfile_size = 0;
      dir_vfile_ptr = 0;
      dir_status = 0;
    }
    sermsg_send_confirm(TARGET_TEENSY, 1); //confirm all packets
  }
  else if (type == SERMSG_GET_VAR){
    if (SERMSG_ADDRESS == 1) {
      // Read up to 64 bytes of a file
      uint8_t data[FILE_TRANSFER_BUFFER];
      uint8_t length=SERMSG_VAR_LENGTH;
      uint8_t len;
      uint16_t avail = dir_vfile_size - dir_vfile_ptr;

      DEBUG_PRINT("  requested data size: ");
      DEBUG_PRINTLN((int)length);

      if (avail > length-1)
        len=length-1;
      else
        len=avail;

      DEBUG_PRINT("Reading directory file: ");

      memcpy(&data[1], &dir_vfile[dir_vfile_ptr], len);
      dir_vfile_ptr += len;

      data[0] = len;
      sermsg_send_var_get_reply(TARGET_TEENSY, 1, length, data);
    }
  }
}

void sdcard_loop()
{
}

