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

#define ARDUINO_IO 1
#include <microME.h>
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
  uint8_t type=SERMSG_MSGTYPE;
  
  if (type == SERMSG_SEND_SHORT) {
    if (SERMSG_ADDRESS == 13)
      digitalWrite(13,SERMSG_DATA);
    else
      settings[SERMSG_ADDRESS]=SERMSG_DATA;
    sermsg_send_confirm(TARGET_TEENSY, 1);
  }
}

void loop() 
{
  sermsg_loop(TARGET_TEENSY);

  if (msgevent) {
    switch (SERMSG_TARGET)
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
