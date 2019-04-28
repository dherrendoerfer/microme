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

static void io_irq()
{ 
  static uint8_t key = 0;
  static uint8_t io_stats = 0;

  flush_in(TARGET_TEENSY2);

  // IO-irq will only get the state, then the contents one by one
  if (sermsg_get_short(TARGET_TEENSY2, 0, 1, &io_stats)) {
    return;
  }

  if (!io_stats)
    return;
    
  // Remote debug keyboard input
  if (io_stats & 0x01) {
    if (sermsg_get_short(TARGET_TEENSY2, 2, 1, &key)) {
      // ignore
    } else {
      kbd_buffer[kbd_buff_ceil++] = key;
      kbd_buff_ceil &= 0x1F;
    }
  }
return;
  // SD card file input (file-stream)
  if (io_stats & 0x02) {
    if (sermsg_get_short(TARGET_TEENSY2, 3, 1, &key)) {
      // ignore
    }
  } else if (io_stats & 0x04) {
    // END of file stream
    if (sermsg_get_short(TARGET_TEENSY2, 3, 1, &key)) {
      // ignore
    }
  }
}

static void kbd_irq()
{ 
  static uint8_t key = 0;
  
  flush_in(TARGET_ARDUINO);

  if (sermsg_get_short(TARGET_ARDUINO, 1, 1, &key)) {
    return;
  }

  if (key==0x03)
    signal_break++;
    
  kbd_buffer[kbd_buff_ceil++] = key;
  kbd_buff_ceil &= 0x1F;
}

static uint8_t kbd_avail()
{
  if (kbd_buff_ceil < kbd_buff_floor)
    return(32 - kbd_buff_floor + kbd_buff_ceil);
  else
    return(kbd_buff_ceil - kbd_buff_floor);
}

static uint8_t kbd_read(uint8_t *key)
{
  uint8_t ret;

  if (kbd_buff_ceil != kbd_buff_floor) {
    ret = kbd_buffer[kbd_buff_floor++];
    kbd_buff_floor = kbd_buff_floor & 0x1F;
    *key=ret;
    return ret;
  }
  return(0);
}

static void setup_kbd(uint8_t kbd_irqpin)
{
  pinMode(kbd_irqpin, INPUT);

  attachInterrupt(digitalPinToInterrupt(kbd_irqpin), kbd_irq, RISING);
}

static void setup_ioirq(uint8_t io_irqpin)
{
  pinMode(io_irqpin, INPUT);

  attachInterrupt(digitalPinToInterrupt(io_irqpin), io_irq, RISING);
}

