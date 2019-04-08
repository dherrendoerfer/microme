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

uint8_t LEDS[10*3];
uint8_t led_update = 0;
uint8_t led_mode[8];

#define MODE_STD   0
#define MODE_DECAY 1

void led_init()
{
  pinMode(4,OUTPUT);
  ws_2812_select_port(4);
  led_mode[0]=0;
}

void led_msg()
{
  uint8_t type=msgbuffer[0] & 0xF0;

  if (type == SERMSG_PUT_SHORT){
    if (msgbuffer[1] < 30) { 
      LEDS[msgbuffer[1]] = msgbuffer[2];
    }
    else {
      uint8_t led = (msgbuffer[1]-30)*3;
      LEDS[led++] = msgbuffer[2];
      LEDS[led++] = msgbuffer[2];
      LEDS[led++] = msgbuffer[2];
    }
    led_update = 1;
  }
  else if (type == SERMSG_SEND_SHORT) {
    if (msgbuffer[1] < 30) { 
      LEDS[msgbuffer[1]] = msgbuffer[2];
      led_update = 1;
    }
    else if (msgbuffer[1] < 40) { 
      uint8_t led = msgbuffer[1]-30;
      LEDS[led++] = msgbuffer[2];
      LEDS[led++] = msgbuffer[2];
      LEDS[led++] = msgbuffer[2];
      led_update = 1;
    }
    else {
      led_mode[msgbuffer[1]-40] = msgbuffer[2];
    }
  }
}

int laststatus = 0;

int led_loop()
{
  if (laststatus != status && Serial.available() == 0) {
    for (int i=0; i<11; i++) {
      if (status == i) {
        LEDS[i*3]=128;
        LEDS[i*3+1]=128;
        LEDS[i*3+2]=128;
      }
      else {
        LEDS[i*3]=0; 
        LEDS[i*3+1]=0;
        LEDS[i*3+2]=0;
      }
    }
    laststatus = status;
    led_update = 1;

    return 0;
  }

  if ( led_mode[0]==MODE_DECAY && Serial.available() == 0 && millis() % 100 == 0 ) {
    for (int i=0; i<30; i++) {
      if (LEDS[i] > 5)
        LEDS[i]-=5;
      else 
        LEDS[i]=0;
    }
    led_update = 1;

    return 0;
  }
  
  if (led_update && Serial.available() == 0) {
    ws2812_sendarray_mask(LEDS, 10*3, pinMask, ws2812_port, ws2812_port_reg);
    led_update = 0;

    return 0;
  }
}
