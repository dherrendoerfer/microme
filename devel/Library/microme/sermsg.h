/*
 * microME Serial Communication library
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
 /*
Sermsg format:


Short Query
BYTE 0: MESAGE TARGET&TYPE
BYTE 1: MESSAGE TARGET ADDRESS
BYTE 3: CHKSUM

RESPONSE: OK/NOK, ADDRESS, DATA, CHKSUM

Long Query
BYTE 0: MESAGE TARGET&TYPE
BYTE 1: MESSAGE TARGET ADDRESS
BYTE 2: MESSAGE LENGTH
BYTE 3: MESSAGE LENGTH
BYTE 4: MESSAGE LENGTH
BYTE 5: MESSAGE LENGTH
BYTE 3: CHKSUM

RESPONSE: OK, TRANSMISSION ID, CHKSUM


Short Message
BYTE 0: MESAGE TARGET&TYPE
BYTE 1: MESSAGE TARGET ADDRESS
BYTE 2: MESSAGE DATA
BYTE 3: CHKSUM

RESPONSE: OK/NOK

Medium Message
BYTE 0: MESAGE TARGET&TYPE
BYTE 1: MESSAGE DATA
BYTE 2: MESSAGE DATA
BYTE 3: MESSAGE DATA
BYTE 4: MESSAGE DATA
BYTE 5: MESSAGE DATA
BYTE 6: MESSAGE DATA
BYTE 7: CHKSUM

RESPONSE: OK/NOK

Long Message
BYTE 0: MESAGE TARGET&TYPE
BYTE 1: MESSAGE TARGET ADDRESS
BYTE 2: MESSAGE LENGTH
BYTE 3: MESSAGE LENGTH
BYTE 4: MESSAGE LENGTH
BYTE 5: MESSAGE LENGTH
BYTE 6: CHKSUM

RESPONSE: OK/NOK, TRANSMISSION ID, CHKSUM


DATA BLOCKs:

Then :
BYTE 0: TRANSMISSION ID
BYTE 1: BLOCK NUMBER
BYTE 0-N: DATA
BYTE N+1: CHKSUM
for up to 512 bytes

RESPONSE: OK/NOK, BLOCK NR, CHKSUM


*/

#define SERMSG_SEND_SHORT 0x10
#define SERMSG_SEND_MED   0x20
#define SERMSG_SEND_LONG  0x30
#define SERMSG_GET_SHORT  0x40
#define SERMSG_GET_LONG   0x50
#define SERMSG_BLOCK      0x60
#define SERMSG_PUT_SHORT  0x70
#define SERMSG_SEND_VAR   0x80

volatile uint8_t sermsg_busy;

static uint8_t msgbuffer[260];
static uint8_t msgevent = 0;

static void flush_in(HardwareSerial *serialport)
{
  uint8_t tmp;
  while (serialport->available() > 0)
    tmp = serialport->read();
}

static int sermsg_waitread(HardwareSerial *serialport, uint8_t *buffer, uint8_t pos, uint8_t len)
{
  uint16_t timeout;
  while (len) {
    timeout=100;
    while (serialport->available() == 0 && timeout-- ) {
      delay(1);
    }
    if (!timeout) {
      return 1;
    }
    buffer[pos++]=serialport->read();
    len--;
  }
  return 0;
}

static void sermsg_init()
{

}

static void sermsg_loop(HardwareSerial *serialport)
{
  uint8_t count=0;
  uint8_t CSUM=0;
  uint8_t datagram=0;
  uint8_t type = 0;
  uint8_t length = 0;
  uint8_t i;

  if (serialport->available() > 0){
    msgbuffer[count++]=serialport->read();
  }
  else
  {
    return;
  }

  type=msgbuffer[0] & 0xF0;

  switch (type)
  {
  case SERMSG_GET_SHORT:
    if (sermsg_waitread(serialport,msgbuffer,count,2))
      goto timeout;
    count=2;
    break;
  case SERMSG_SEND_SHORT:
    if (sermsg_waitread(serialport,msgbuffer,count,3))
      goto timeout;
    count=3;
    break;
  case SERMSG_PUT_SHORT:
    if (sermsg_waitread(serialport,msgbuffer,count,3))
      goto flush;
      datagram=1;
      count=3;
    break;
    case SERMSG_SEND_VAR:
      if (sermsg_waitread(serialport,msgbuffer,count,3))
        goto timeout;
      count=3;
      break;
  default:
    return;
  }

  /*Checksum*/
  for (i=0; i<count; i++)
    CSUM =+ msgbuffer[i];

  // Message without confirmation, quick exit
  if (datagram) {
    if (msgbuffer[count] == CSUM) {
      msgevent = 1;
      return;
    }
    else {
      return;
    }
  }

  if (msgbuffer[count] != CSUM){
    serialport->write('-'); /*error*/
    return;
  }
  else {
    serialport->write('+'); /*ok*/
  }

  // Messages with a data segment
  switch (type)
  {
  case SERMSG_SEND_VAR:
    length=msgbuffer[2];

    if (sermsg_waitread(serialport,msgbuffer, 3 ,length+1))
      goto timeout;
      /*Inner Checksum*/
      CSUM=0;
      for (i=0; i<length; i++)
        CSUM =+ msgbuffer[i+3];

      if (msgbuffer[length+4] != CSUM){
        serialport->write('-'); /*error*/
        return;
      }
      else {
        serialport->write('+'); /*ok*/
        msgevent = 1;
        return;
      }
      break;
  default:
        msgevent=1;
        return;
  }


timeout:
  serialport->write('T'); /*Timeout*/
flush:
  flush_in(serialport);
  return;
}

static int sermsg_send_short(HardwareSerial *serialport, uint8_t target, uint8_t address, uint8_t data)
{
  uint8_t buffer[8];
  uint8_t i;

  buffer[0]=SERMSG_SEND_SHORT + target;
  buffer[1]=address;
  buffer[2]=data;

  for (i=0; i<3; i++)
    buffer[3] =+ buffer[i];

  serialport->write(buffer,4);

  /*Get a response*/

  sermsg_waitread(serialport, buffer, 0, 1);

  if (buffer[0] == '+') {
    digitalWrite(13,LOW);
    return 0;
  }

  digitalWrite(13,HIGH);
  return 1;
}

static int sermsg_get_short(HardwareSerial *serialport, uint8_t target, uint8_t address, uint8_t *data)
{
  uint8_t buffer[8];
  uint8_t CSUM = 0;
  uint8_t i;

  buffer[0]=SERMSG_GET_SHORT + target;
  buffer[1]=address;

  for (i=0; i<2; i++)
    buffer[2] =+ buffer[i];

  serialport->write(buffer,3);

  /*Get a response*/

  sermsg_waitread(serialport, buffer, 0, 1);

  if (buffer[0] != '+') {
    digitalWrite(13,HIGH);
    return 1;
  }

  sermsg_waitread(serialport, buffer, 0, 3);

  /*Checksum*/
  for (i=0; i<2; i++)
    CSUM =+ buffer[i];

  if ( buffer[0] == address && buffer[2] == CSUM) {
    serialport->write('+');
    *data=buffer[2];
    digitalWrite(13,LOW);
    return 0;
  }

  serialport->write('-');
  digitalWrite(13,HIGH);
  return 1;
}

static int sermsg_send_get_reply(HardwareSerial *serialport, uint8_t address, uint8_t data)
{
  uint8_t buffer[8];
  uint8_t i;

  buffer[0]=address;
  buffer[1]=data;

  for (i=0; i<2; i++)
    buffer[2] =+ buffer[i];

  serialport->write(buffer,3);

  sermsg_waitread(serialport, buffer, 0, 1);

  if (buffer[0] != '+') {
    digitalWrite(13,HIGH);
    return 1;
  }

  return 0;
}

unsigned long last_put_millis = 0;

static int sermsg_put_short(HardwareSerial *serialport, uint8_t target, uint8_t address, uint8_t data, uint8_t holdoff_time)
{
  uint8_t buffer[8];
  uint8_t i;

  /* We can't hammer the receiver*/
  if ((long)millis()-(long)last_put_millis < holdoff_time)
    return 5;

  last_put_millis = millis();

  buffer[0]=SERMSG_PUT_SHORT + target;
  buffer[1]=address;
  buffer[2]=data;

  for (i=0; i<3; i++)
    buffer[3] =+ buffer[i];

  serialport->write(buffer,4);

  return 0;
}

static int sermsg_send_var(HardwareSerial *serialport, uint8_t target, uint8_t address, uint8_t length, uint8_t *data)
{
  uint8_t buffer[258];
  uint8_t i;
  uint8_t CSUM = 0;

  buffer[0]=SERMSG_SEND_VAR + target;
  buffer[1]=address;
  buffer[2]=length;

  for (i=0; i<3; i++)
    buffer[3] =+ buffer[i];

  serialport->write(buffer,4);

  /*Get a response*/
  sermsg_waitread(serialport, buffer, 0, 1);

  if (buffer[0] != '+') {
    digitalWrite(13,HIGH);
    return 1;
  }

  /*Checksum*/
  CSUM = 0;
  for (i=0; i<length; i++)
    CSUM =+ data[i];

  serialport->write(data, length);
  serialport->write(CSUM);

  /*Get a response*/
  sermsg_waitread(serialport, buffer, 0, 1);

  if (buffer[0] != '+') {
    digitalWrite(13,HIGH);
    return 1;
  }

  digitalWrite(13,LOW);
  return 0;
}
