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

void sid_init()
{
  dac1.analogReference(INTERNAL);
  AudioMemory(10);
}


void sid_msg()
{
  uint8_t type=msgbuffer[0] & 0xF0;

  if (type == SERMSG_SEND_SHORT){
    if (msgbuffer[1] < 30) {
      playSID.setreg(msgbuffer[1], msgbuffer[2]);
    }
  }
}

void sid_loop()
{
  return;
}
