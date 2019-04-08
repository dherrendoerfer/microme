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

uint8_t log_mode[4];
uint8_t log_kbdbuf[256];

uint8_t log_kbd_floor = 0;
uint8_t log_kbd_ceil  = 0;

int8_t log_diff = 0;
int8_t log_lastdiff = 0;

void log_init()
{
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
}

void log_msg()
{
  uint8_t type=msgbuffer[0] & 0xF0;

  if (type == SERMSG_SEND_VAR){
    if (msgbuffer[1] == 1) {
      uint8_t length=msgbuffer[2];
      for (uint8_t i=0; i<length; i++) {
        Serial.print((char)msgbuffer[i+3]);  
      }
    }
  }
  else if (type == SERMSG_GET_SHORT){
    if (msgbuffer[1] == 1) {
      digitalWrite(2,LOW);
      if (sermsg_send_get_reply(&Serial1, 1, log_kbdbuf[log_kbd_floor]) == 0)
        log_kbd_floor++;
    }
    else if (msgbuffer[1] == 2) {
      sermsg_send_get_reply(&Serial1, 2, log_diff);
    }
  }
}

void log_loop()
{
  while (Serial.available() > 0) {
    log_kbdbuf[log_kbd_ceil++] = Serial.read();  
  }

  log_diff = (int8_t)log_kbd_ceil - (int8_t)log_kbd_floor;
  
  if ( log_diff != log_lastdiff ) {
    log_lastdiff=log_diff;
    if ( log_diff )
      digitalWrite(2,HIGH);
    else
      digitalWrite(2,LOW);
//    Serial.print("Diff: ");
//    Serial.println((int)log_diff);
  } 

  
  
  return;
}
