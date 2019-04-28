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

static uint8_t microme_input_available()
{
  uint8_t avail;

  /* Keyboard wins over serial debug console*/

  uvga.waitSync();

  if (digitalRead(12) == HIGH) {
    if (!sermsg_get_short(TARGET_TEENSY2, 2, 2, &avail)) {
      return avail;
    }
  }

/*
  if (digitalRead(11) == HIGH) {
    if (!sermsg_get_short(TARGET_ARDUINO, 1, 2, &avail)) {
      return avail;
    }
  }
*/
  return kbd_avail();
//  return 0;
}

static uint8_t microme_read_input(uint8_t *data)
{
  if (digitalRead(12) == HIGH) {
    if (!sermsg_get_short(TARGET_TEENSY2, 2, 1, data)) {
      return 0;
    }
  }

/*
  if (digitalRead(11) == HIGH) {
    if (!sermsg_get_short(TARGET_ARDUINO, 1, 1, data)) {
      return 0;
    }
  }
*/

  if (!kbd_read(data))
    return 0;

  return 1;
}

static uint8_t microme_write_ioconsole(char* data, uint8_t length)
{
  int i;
  sermsg_send_var(TARGET_TEENSY2, 2, 1, length, (uint8_t*)data);

  for (i=0; i<length; i++) {
    uvga.print((char)data[i]);
  }
  return 0;
}

static uint8_t microme_write_ioconsole(char* data)
{
  uvga.println((char*)data);
  uvga.waitSync();
  
  sermsg_send_var(TARGET_TEENSY2, 2, 1, strlen(data), (uint8_t*)data);
  sermsg_send_var(TARGET_TEENSY2, 2, 1, 1, (uint8_t*)"\n");
  return 0;
}
