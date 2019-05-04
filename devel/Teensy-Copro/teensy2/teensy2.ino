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

#define TEENSY_COPRO 1
#include <microME.h>

#include <Audio.h>
#include <reSID.h>

#include <SD.h>
#include <SPI.h>

// GUItool: begin automatically generated code
AudioPlaySID       playSID;  
AudioOutputAnalog  dac1; 
AudioConnection    patchCord1(playSID, dac1);
// GUItool: end automatically generated code

#include <sermsg.h>

/* The local settings page */
uint8_t settings[10];
uint8_t *mode = &settings[0];

uint8_t io_status = 0;

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
  sdcard_init();
}

void msg()
{
//  Serial.println((int)SERMSG_MSGTYPE);
//  Serial.println((int)SERMSG_ADDRESS);
//  Serial.println((int)SERMSG_DATA);

  uint8_t type = SERMSG_MSGTYPE;
  
  if (type == SERMSG_GET_SHORT) {
    if (SERMSG_ADDRESS == 1) {
      digitalWrite(2,LOW);
      sermsg_send_get_reply(TARGET_TEENSY, 1, io_status);
    }
  }
  else if (SERMSG_MSGTYPE == SERMSG_SEND_SHORT) {
    if (SERMSG_ADDRESS == 13)
      digitalWrite(13,SERMSG_DATA);
    else
      settings[SERMSG_ADDRESS] = SERMSG_DATA;
  }
}

void loop() {
  sermsg_loop(TARGET_TEENSY);

  if (msgevent) {
    switch (SERMSG_TARGET)
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
      case 3:
        sdcard_msg();
        break;
      case 4:
        sdcard_file(0); // File IO for file 0
        break;
      case 8:
        sdcard_directory(); // File IO for the directory file only
        break;
    }
    msgevent = 0;
    return;
  }

  sid_loop();
  log_loop();
  sdcard_loop();
}
