/*
 * microME Arduino sample firmware
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

#include "uWS2812.h"
#include "uKbd.h"
#include "uKbdCharmap.h"

uint8_t output_buffer[128];
uint8_t buff_floor=0;
uint8_t buff_ceil=0;

int status = 0;
#include <sermsg.h>

/* The local settings page */
uint8_t settings[10];

uint8_t *mode = &settings[0];

void setup() 
{
  Serial.begin(115200);

  sermsg_init();
  kbd_init();
  led_init();
  lpt_init();
}

void msg()
{
  if (msgbuffer[1] == 13)
    digitalWrite(13,msgbuffer[2]);
  else
    settings[msgbuffer[1]]=msgbuffer[2];
}

void loop() 
{
  sermsg_loop(&Serial);

  if (msgevent) {
    switch (msgbuffer[0]&0x0F)
    {
      case 0:
        msg();
        break;
      case 1:
        kbd_msg();
        break;
      case 2:
        led_msg();
        break;
      case 3:
        lpt_msg();
        break;
    }
    msgevent = 0;
    return;
  }
  
  kbd_loop();
  led_loop();
  lpt_loop();
}
