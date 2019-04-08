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

static void kbd_irq()
{ 
  static uint8_t key = 0;

  flush_in(&Serial1);
  
  if (sermsg_get_short(&Serial1, 1, 1, &key)) {
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


