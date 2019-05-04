/*
 * microME Teensy Main-Processor sample firmware
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


uint8_t stat(uint8_t fd)
{
  uint8_t fstatus;
  sermsg_get_short(TARGET_TEENSY2, 4+fd, 1, &fstatus);
  return(fstatus);
}

uint8_t open(uint8_t fd, char *filename)
{
  uint8_t fstatus;
  sermsg_send_var(TARGET_TEENSY2, 4+fd, 1, strlen(filename), (uint8_t*)filename);
  return (stat(fd) & FILE_STATUS_OPEN);
}

uint8_t open_w(uint8_t fd, char *filename)
{
  uint8_t fstatus;
  sermsg_send_var(TARGET_TEENSY2, 4+fd, 2, strlen(filename), (uint8_t*)filename);
  return (stat(fd) & FILE_STATUS_OPEN);
}

uint8_t available(uint8_t fd)
{
  uint8_t avail;
  sermsg_get_short(TARGET_TEENSY2, 4+fd, 2, &avail);
  return(avail);
}

uint8_t close(uint8_t fd)
{
  return (sermsg_send_short(TARGET_TEENSY2, 4+fd, 1, 1));
}


uint8_t read(uint8_t fd)
{
  uint8_t ret;
  sermsg_get_short(TARGET_TEENSY2, 4+fd, 3, &ret);
  return(ret);
}

int read(int fd, uint8_t *buffer, uint16_t pos, uint8_t length)
{
  uint8_t tmpbuffer[FILE_TRANSFER_BUFFER];
  
  if (sermsg_read_var(TARGET_TEENSY2, 4+fd, 1, length, tmpbuffer)) {
    return 0;  
  }

  memcpy((void*)&buffer[pos],(void*)&tmpbuffer[1],tmpbuffer[0]);

  return(tmpbuffer[0]);
}

int write(int fd, uint8_t *buffer, uint16_t pos, uint8_t length)
{
  if (sermsg_send_var(TARGET_TEENSY2, 4+fd, 3, length, (uint8_t*)&buffer[pos])) {
    return 0;  
  }

  return(length);
}

int reinit()
{
  uint8_t ret;
  if (sermsg_get_short(TARGET_TEENSY2, 4, 4, &ret)) {
    return 0;  
  }

  return(ret);
}

