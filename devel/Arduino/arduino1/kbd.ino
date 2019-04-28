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

const int kbd_DataPin = 5;
const int kbd_IRQpin =  2;

uint8_t kbd_kbdbuf[256];

uint8_t kbd_kbd_floor = 200;
uint8_t kbd_kbd_ceil  = 200;

int8_t kbd_diff = 0;
int8_t kbd_lastdiff = 0;

int kbd_mode = 2; /*Default mode*/
void kbd_init()
{
  uKbd_start(kbd_DataPin, kbd_IRQpin);

  pinMode(6,OUTPUT);
  digitalWrite(6,LOW);
}

void calc_diff()
{
  if (kbd_kbd_ceil < kbd_kbd_floor)
    kbd_diff = 256 - kbd_kbd_floor + kbd_kbd_ceil;
  else
    kbd_diff = kbd_kbd_ceil - kbd_kbd_floor;
}

void kbd_msg()
{
  uint8_t type=SERMSG_MSGTYPE;

  if (type == SERMSG_GET_SHORT){
    if (SERMSG_ADDRESS == 1) {
      digitalWrite(6,LOW);
      if (sermsg_send_get_reply(TARGET_TEENSY, 1, kbd_kbdbuf[kbd_kbd_floor]) == 0) {
        kbd_kbd_floor++;
        calc_diff();
      }
    }
    else if (SERMSG_ADDRESS == 2) {
      sermsg_send_get_reply(TARGET_TEENSY, 2, kbd_diff);
    }
  }
  else if (type == SERMSG_SEND_SHORT) {
    if (SERMSG_ADDRESS == 0)
      kbd_mode=SERMSG_DATA;
    sermsg_send_confirm(TARGET_TEENSY, 1);
  }
}


int kbd_loop()
{
  uint8_t code;
  
  if (kbd_mode == 1){
    if (uKbd_available() > 0) {
      code = uKbd_read();
      if (uKbdC_send_scancode(code))
        Serial.write((char)uKbdC_read());
    }
  }
  else if(kbd_mode == 2){
    while (uKbd_available() > 0) {
      code = uKbd_read();
      if (uKbdC_send_scancode(code)){
        kbd_kbdbuf[kbd_kbd_ceil++] = uKbdC_read();
        calc_diff();
      } 
    }
    
    if ( kbd_diff != kbd_lastdiff ) {
      kbd_lastdiff=kbd_diff;
      if ( kbd_diff && !msgevent)
        digitalWrite(6,HIGH);
      else
        digitalWrite(6,LOW);
    } 
  }
  return(0);
}
