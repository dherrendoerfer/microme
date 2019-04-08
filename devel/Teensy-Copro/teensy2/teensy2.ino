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

#include <Audio.h>
#include <reSID.h>

// GUItool: begin automatically generated code
AudioPlaySID       playSID;  
AudioOutputAnalog  dac1; 
AudioConnection    patchCord1(playSID, dac1);
// GUItool: end automatically generated code


int status = 0;
#include <sermsg.h>

/* The local settings page */
uint8_t settings[10];
uint8_t *mode = &settings[0];

void setup() {
  // wait for Arduino Serial Monitor
  int ml = millis();
  while (!Serial && (millis()-ml < 1000)) ; 
  
  Serial.println("Greetings from Teensy2...");
  
  Serial1.begin(500000);
  pinMode(13, OUTPUT);

  sermsg_init();
  sid_init();
  log_init();
}

void msg()
{
  if (msgbuffer[1] == 13)
    digitalWrite(13,msgbuffer[2]);
  else
    settings[msgbuffer[1]]=msgbuffer[2];
}

void loop() {
  sermsg_loop(&Serial1);

  if (msgevent) {
    switch (msgbuffer[0]&0x0F)
    {
      case 0:
        msg();
        break;
      case 1:
        sid_msg();
        break;
      case 2:
        log_msg();
        break;
    }
    msgevent = 0;
    return;
  }

  sid_loop();
  log_loop();
}
