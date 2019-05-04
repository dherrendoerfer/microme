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

void lpt_init()
{
  pinMode(7,OUTPUT);   //PRT: D0
  digitalWrite(7,LOW);
  pinMode(8,OUTPUT);   //PRT: D1
  digitalWrite(8,LOW);
  pinMode(9,OUTPUT);   //PRT: D2
  digitalWrite(9,LOW);
  pinMode(10,OUTPUT);  //PRT: D3
  digitalWrite(10,LOW);
  pinMode(11,OUTPUT);  //PRT: D4
  digitalWrite(11,LOW);
  pinMode(12,OUTPUT);  //PRT: D5
  digitalWrite(12,LOW);
  pinMode(A0,OUTPUT);  //PRT: D6
  digitalWrite(A0,LOW);
  pinMode(A1,OUTPUT);  //PRT: D7
  digitalWrite(A1,LOW);
  
  pinMode(3,OUTPUT);   //PRT: STROBE
  digitalWrite(3,LOW);
  
  /* Set these low, they will become inputs */
  digitalWrite(A2,LOW);
  digitalWrite(A3,LOW);
  digitalWrite(A4,LOW);
  digitalWrite(A5,LOW);
  digitalWrite(A6,LOW);
  
  pinMode(A2,INPUT);   //PRT: ACK
  pinMode(A3,INPUT);   //PRT: BUSY
  pinMode(A4,INPUT);   //PRT: PAPER
  pinMode(A5,INPUT);   //PRT: SELECT
  pinMode(A6,INPUT);   //PRT: ERROR
}

void lpt_msg()
{
  
}

uint8_t mode_from_data(uint8_t data)
{
  switch(data){
  case 1:
    return INPUT;
  case 2:
    return INPUT_PULLUP;
  }
  return OUTPUT;
}

void lptio_msg()
{
  /* Direct port manipulation */
  uint8_t type=SERMSG_MSGTYPE;

  if (type == SERMSG_SEND_SHORT) {
    /* Set port values */
    switch (SERMSG_ADDRESS) {
      case 0:
        digitalWrite(7, SERMSG_DATA);
        break;  
      case 1:
        digitalWrite(8, SERMSG_DATA);
        break;  
      case 2:
        digitalWrite(9, SERMSG_DATA);
        break;  
      case 3:
        digitalWrite(10, SERMSG_DATA);
        break;  
      case 4:
        digitalWrite(11, SERMSG_DATA);
        break;  
      case 5:
        digitalWrite(12, SERMSG_DATA);
        break;  
      case 6:
        digitalWrite(A0, SERMSG_DATA);
        break;  
      case 7:
        digitalWrite(A1, SERMSG_DATA);
        break;  
      case 8:
        digitalWrite(3, SERMSG_DATA);
        break;  
      case 9:
        digitalWrite(A2, SERMSG_DATA);
        break;  
      case 10:
        digitalWrite(A3, SERMSG_DATA);
        break;  
      case 11:
        digitalWrite(A4, SERMSG_DATA);
        break;  
      case 12:
        digitalWrite(A5, SERMSG_DATA);
        break;  

    /* Set port modes */
      case 20:
        pinMode(7,mode_from_data(SERMSG_DATA));
        break;
      case 21:
        pinMode(8,mode_from_data(SERMSG_DATA));
        break;
      case 22:
        pinMode(9,mode_from_data(SERMSG_DATA));
        break;
      case 23:
        pinMode(10,mode_from_data(SERMSG_DATA));
        break;
      case 24:
        pinMode(11,mode_from_data(SERMSG_DATA));
        break;
      case 25:
        pinMode(12,mode_from_data(SERMSG_DATA));
        break;
      case 26:
        pinMode(A0,mode_from_data(SERMSG_DATA));
        break;
      case 27:
        pinMode(A1,mode_from_data(SERMSG_DATA));
        break;
      case 28:
        pinMode(3,mode_from_data(SERMSG_DATA));
        break;
      case 29:
        pinMode(A2,mode_from_data(SERMSG_DATA));
        break;
      case 30:
        pinMode(A3,mode_from_data(SERMSG_DATA));
        break;
      case 31:
        pinMode(A4,mode_from_data(SERMSG_DATA));
        break;
      case 32:
        pinMode(A5,mode_from_data(SERMSG_DATA));
        break;
     /*General reset*/
      case 99:
        lpt_init();
        break;  

    }
    sermsg_send_confirm(TARGET_TEENSY, 1);
  } 
  else if (type == SERMSG_GET_SHORT) {
    switch (SERMSG_ADDRESS) {
      case 0:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(7));
        break;  
      case 1:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(8));
        break;  
      case 2:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(9));
        break;  
      case 3:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(10));
        break;  
      case 4:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(11));
        break;  
      case 5:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(12));
        break;  
      case 6:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(A0));
        break;  
      case 7:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(A1));
        break;  
      case 8:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(3));
        break;  
      case 9:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(A2));
        break;  
      case 10:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(A3));
        break;  
      case 11:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(A4));
        break;  
      case 12:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, digitalRead(A5));
        break; 
      /* Analog read */
      case 20:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, analogRead(A0));
        break; 
      case 21:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, analogRead(A1));
        break; 
      case 22:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, analogRead(A2));
        break; 
      case 23:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, analogRead(A3));
        break; 
      case 24:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, analogRead(A4));
        break; 
      case 25:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, analogRead(A5));
        break; 
      default:
        sermsg_send_get_reply(TARGET_TEENSY, SERMSG_ADDRESS, 0);
    }
  }    
}


int lpt_loop()
{
  
}
