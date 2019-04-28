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
  uint8_t type=SERMSG_MSGTYPE;

  if (type == SERMSG_SEND_SHORT){
    if (SERMSG_ADDRESS < 30) {
      playSID.setreg(SERMSG_ADDRESS, SERMSG_DATA);
      sermsg_send_confirm(TARGET_TEENSY, 1);
    }
  }
  else if (type == SERMSG_SEND_VAR){
    if (SERMSG_ADDRESS == 0 && SERMSG_VAR_LENGTH < 30) {
      for (uint8_t i=0; i<SERMSG_VAR_LENGTH; i++) {
        playSID.setreg(i, SERMSG_VAR_MESSAGE(i));
      }
      sermsg_send_confirm(TARGET_TEENSY, 1);
    }
  }

}

void sid_loop()
{
  return;
}
