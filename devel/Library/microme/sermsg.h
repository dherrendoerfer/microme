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

#include <sermsg_COBS.h>

// little helpers
#define SERMSG_MSGTYPE (recvbuffer[0] & 0xF0)
#define SERMSG_TARGET  (recvbuffer[0] & 0x0F)

#define SERMSG_ADDRESS recvbuffer[1]
#define SERMSG_DATA  recvbuffer[2]

#define SERMSG_CSUM  recvbuffer[3]

#define SERMSG_VAR_LENGTH recvbuffer[2]
#define SERMSG_VAR_MESSAGE(I) recvbuffer[I+3]

// Supported commands
#define SERMSG_SEND_SHORT 0x10
#define SERMSG_GET_SHORT  0x20
#define SERMSG_PUT_SHORT  0x30
#define SERMSG_SEND_VAR   0x40
#define SERMSG_GET_VAR    0x50

// Variables
volatile uint8_t sermsg_busy;

// Receive buffer
static volatile uint8_t recvbuffer[132];
static volatile uint8_t recv_count = 0;

// Send buffer
static volatile uint8_t databuffer[132];
static volatile uint8_t *msgbuffer = &databuffer[4];

static volatile uint8_t msgevent = 0;

static void flush_in(HardwareSerial *serialport)
{
  uint8_t tmp;
  while (serialport->available() > 0)
    tmp = serialport->read();
}

static int sermsg_recv_reply(HardwareSerial *serialport, uint8_t length)
{
  uint16_t timeout;
  uint8_t pos = 0;
  uint8_t tmp = 0;
  uint8_t len = length;

  len=len+2;  //Add expected overhead for COBS packet and packet end (0)

  while (len) {
    timeout=300;
    while (timeout && (serialport->available() < 1)) {
      delay(1);
      timeout--;
    }
    if (!timeout) {
      DEBUG_PRINTLN("RCVRPL: TIMEOUT !!");
      DEBUG_PRINT("RCVRPL: POS: ");
      DEBUG_PRINTLN((int)pos);
      DEBUG_PRINTLN((int)recvbuffer[pos-1]);

      return(0);
    }

    tmp = serialport->read();
//    uvga.print((char)tmp);
//    uvga.print(" ");

    if (tmp == 0) {
      if (len == 1) {
        return(COBS_decode128(recvbuffer, length + 1));
      }
    }
    else {
      recvbuffer[pos++]=tmp;
    }
    len--;
  }

  DEBUG_PRINTLN("RCVRPL: FRAME?");
  return(0);
}

static void sermsg_init()
{

}

/* The sermsg server loop holds the implementation to every
 * request implemented below. Each pass of the loop should
 * not handle more that one mesage frame, so it does not
 * block other functions of the controller
 */

static void sermsg_readpkt(uint8_t length)
{
  uint8_t CSUM = 0;
  uint8_t i, expect_len, type;

  for (i = 0; i < length - 1; i++)
    CSUM += recvbuffer[i];

  if (recvbuffer[i] != CSUM ) {
    DEBUG_PRINTLN("READPKT: CSUM MISMATCH !!");
    DEBUG_PRINT("CALCULATED: ");
    DEBUG_PRINTLN((int)CSUM);
    DEBUG_PRINT("EXPECTED: ");
    DEBUG_PRINTLN((int)recvbuffer[i-1]);
    DEBUG_PRINT("PACKET LENGTH: ");
    DEBUG_PRINTLN((int)length);
    for (i = 0; i < length ; i++) {
      DEBUG_PRINT((int)recvbuffer[i]);
      DEBUG_PRINT(",");
    }
    DEBUG_PRINTLN();
    return;
  }

  type = SERMSG_MSGTYPE;

  switch(type) {
  case SERMSG_GET_SHORT:
    expect_len = 3;
    break;
  case SERMSG_SEND_SHORT:
    expect_len = 4;
    break;
  case SERMSG_PUT_SHORT:
    expect_len = 4;
    break;
  case SERMSG_SEND_VAR:
    expect_len = 0;
    break;
  case SERMSG_GET_VAR:
    expect_len = 4;
    break;
  default:
    return;
  }

  if ( expect_len ) {
    if ( expect_len == length)
      msgevent = 1;
  }
  else {
    if ( 4 < length)
      msgevent = 1;
  }

  return;
}


static void sermsg_loop(HardwareSerial *serialport)
{
  uint8_t data = 0;

  if (serialport->available() > 0) {
    data = serialport->read();
    recvbuffer[recv_count++] = data;
  }
  else {
    return;
  }

  if (data == 0){
    uint8_t length;
    length = COBS_decode128(recvbuffer, recv_count-1);
    recv_count = 0;

    if (length)
      sermsg_readpkt(length);
  }
}


static void sermsg_send_confirm(HardwareSerial *serialport, uint8_t status)
{
  int transmit_length;
  if (status)
    msgbuffer[0] = '+';
  else
    msgbuffer[0] = '-';

  transmit_length = COBS_encode128(databuffer,1);
  databuffer[transmit_length++] = 0;

  serialport->write((char*)databuffer,transmit_length);
}


/* Send one byte, wait for the confirmation
 *
  Short set
  BYTE 0: MESAGE TYPE and TARGET
  BYTE 1: MESSAGE TARGET ADDRESS
  BYTE 2: MESSGAGE DATA
  BYTE 3: CHKSUM
 * */
static int sermsg_send_short(HardwareSerial *serialport, uint8_t target, uint8_t address, uint8_t data)
{
//  uint8_t buffer[8];
  uint8_t tmp;

  msgbuffer[0]=SERMSG_SEND_SHORT + target;
  msgbuffer[1]=address;
  msgbuffer[2]=data;

  // CSUM ( dont need a for loop here )
  msgbuffer[3]=msgbuffer[0]+msgbuffer[1]+msgbuffer[2];

  tmp=COBS_encode128(databuffer,4);
  databuffer[tmp++] = 0; //EOM

  serialport->write((char*)databuffer,tmp);

  /*Get a response*/
  //sermsg_waitread(serialport, buffer, 0, 1);
  sermsg_recv_reply(serialport, 1); // Want 1 byte return code

  if (recvbuffer[0] == '+') {
    digitalWrite(13,LOW);
    return(0);
  }

  digitalWrite(13,HIGH);
  return(1);
}

/* Request one byte, send the confirmation
 *
  Short Query
  BYTE 0: MESAGE TYPE and TARGET
  BYTE 1: MESSAGE TARGET ADDRESS
  BYTE 2: CHKSUM
 * */
static int sermsg_get_short(HardwareSerial *serialport, uint8_t target, uint8_t address, uint8_t *data)
{
  uint8_t CSUM = 0;
  uint8_t tmp;

  msgbuffer[0]=SERMSG_GET_SHORT + target;
  msgbuffer[1]=address;

  // CSUM ( dont need a for loop here )
  msgbuffer[2]=msgbuffer[0]+msgbuffer[1];

  tmp=COBS_encode128(databuffer,3);
  databuffer[tmp++] = 0; //EOM

  serialport->write((char*)databuffer,tmp);

  /*Get a response*/
  sermsg_recv_reply(serialport, 3); // Want 3 byte return code

  CSUM = recvbuffer[0]+recvbuffer[1];

  if (CSUM != recvbuffer[2]) {
    DEBUG_PRINTLN("GETSHORT: CSUM MISMATCH !!");
    DEBUG_PRINT("CALCULATED: ");
    DEBUG_PRINTLN((int)CSUM);
    DEBUG_PRINT("EXPECTED: ");
    DEBUG_PRINTLN((int)recvbuffer[2]);
    for (int i = 0; i < 3 ; i++) {
      DEBUG_PRINT((int)recvbuffer[i]);
      DEBUG_PRINT(",");
    }
    DEBUG_PRINTLN();
    return(1);
  }

  if ( recvbuffer[0] == address ) {
    sermsg_send_confirm(serialport, 1);
    *data=recvbuffer[1];
    digitalWrite(13,LOW);
    return(0);
  }

  sermsg_send_confirm(serialport, 0);
  digitalWrite(13,HIGH);
  return(1);
}

/* Send the reply to a 'short_get' request
 *
 Short get reply
 BYTE 0: ADDRESS
 BYTE 1: Requested DATA
 BYTE 2: CHKSUM
 * */
static int sermsg_send_get_reply(HardwareSerial *serialport, uint8_t address, uint8_t data)
{
  uint8_t tmp;

  msgbuffer[0]=address;
  msgbuffer[1]=data;

  // CSUM ( dont need a for loop here )
  msgbuffer[2]=msgbuffer[0]+msgbuffer[1];

  tmp=COBS_encode128(databuffer,3);
  databuffer[tmp++] = 0; //EOM

  serialport->write((char*)databuffer,tmp);

  sermsg_recv_reply(serialport, 1); // Want 1 byte return code

  if (recvbuffer[0] == '+') {
    digitalWrite(13,LOW);
    return(0);
  }

  digitalWrite(13,HIGH);
  return(1);
}

unsigned long last_put_millis = 0;

/* Send a byte, dont wait for a confirmation (or expect one)
 *
  Short put
  BYTE 0: MESAGE TYPE and TARGET
  BYTE 1: MESSAGE TARGET ADDRESS
  BYTE 2: MESSGAGE DATA
  BYTE 3: CHKSUM
 * */
static int sermsg_put_short(HardwareSerial *serialport, uint8_t target, uint8_t address, uint8_t data, uint8_t holdoff_time)
{
  uint8_t tmp;

  /* We don't want to cause too much load on the receiver */
  if ((long)millis()-(long)last_put_millis < holdoff_time)
    return(5);
  last_put_millis = millis();

  msgbuffer[0]=SERMSG_PUT_SHORT + target;
  msgbuffer[1]=address;
  msgbuffer[2]=data;

  // CSUM ( dont need a for loop here )
  msgbuffer[3]=msgbuffer[0]+msgbuffer[1]+msgbuffer[2];

  tmp=COBS_encode128(databuffer,4);
  databuffer[tmp++] = 0; //EOM

  serialport->write((char*)databuffer,tmp);

  return(0);
}

/* Send a number of bytes (up to 120) and wait for confirmation
 *
  Variable length send
  BYTE 0: MESAGE TYPE and TARGET
  BYTE 1: MESSAGE TARGET ADDRESS
  BYTE 2: MESSGAGE LENGTH
  BYTE 3 - N: MESSAGE DATA
  BYTE N+1: CHKSUM
 * */
static int sermsg_send_var(HardwareSerial *serialport, uint8_t target, uint8_t address, uint8_t length, uint8_t *data)
{
  uint8_t i,tmp, CSUM;

  msgbuffer[0]=SERMSG_SEND_VAR + target;
  msgbuffer[1]=address;
  msgbuffer[2]=length;

  // CSUM ( dont need a for loop here )
  CSUM = msgbuffer[0]+msgbuffer[1]+msgbuffer[2];

  for(i=0; i < length; i++) {
    CSUM += data[i];
    msgbuffer[i+3] = data[i];
  }
  msgbuffer[i+3] = CSUM;

  tmp=COBS_encode128(databuffer,i+4);
  databuffer[tmp++] = 0; //EOM

  serialport->write((char*)databuffer,tmp);

  sermsg_recv_reply(serialport, 1); // Want 1 byte return code

  if (recvbuffer[0] == '+') {
    digitalWrite(13,LOW);
    return(0);
  }

  digitalWrite(13,HIGH);
  return(1);
}

/* Request a number of bytes (up to 120) and wait for confirmation
 *
  Variable length request
  BYTE 0: MESAGE TYPE and TARGET
  BYTE 1: MESSAGE TARGET ADDRESS
  BYTE 2: REQUESTED LENGTH
  BYTE 3: CHKSUM
 * */
static int sermsg_read_var(HardwareSerial *serialport, uint8_t target, uint8_t address, uint8_t length, uint8_t *data)
{
  uint8_t i,tmp, CSUM;
  uint16_t wait;

  msgbuffer[0]=SERMSG_GET_VAR + target;
  msgbuffer[1]=address;
  msgbuffer[2]=length;

  // CSUM ( dont need a for loop here )
  msgbuffer[3] = msgbuffer[0]+msgbuffer[1]+msgbuffer[2];

  tmp = COBS_encode128(databuffer,4);
  databuffer[tmp++] = 0; //EOM

  serialport->write((char*)databuffer,tmp);

  wait = 500;
  while ( wait && serialport->available() < 1) {
    wait--;
    delay(1);
  }

  tmp = sermsg_recv_reply(serialport, length + 2 ); // Want length + 2 byte return code

  // Check the data
  CSUM = 0;
  for (i = 0; i < tmp - 1; i++)
    CSUM += recvbuffer[i];

  if (recvbuffer[i] != CSUM ) {
    DEBUG_PRINTLN("CSUM!");
    return(1);
  }

  if ( recvbuffer[0] == address ) {
    // Copy the data
    memcpy(&data[0], (void*)&recvbuffer[1], length);
    sermsg_send_confirm(serialport, 1);
    digitalWrite(13,LOW);
    return(0);
  }

  sermsg_send_confirm(serialport, 0);
  digitalWrite(13,HIGH);
  return(1);
}

/* Send the reply to a 'short_get' request
 *
 Short get reply
 BYTE 0: ADDRESS
 BYTE 1-N: Requested DATA
 BYTE N+1: CHKSUM
 * */
static int sermsg_send_var_get_reply(HardwareSerial *serialport, uint8_t address, uint8_t length, uint8_t* data)
{
  uint8_t i,tmp,CSUM;
  uint16_t wait;

  msgbuffer[0]=address;
  CSUM = address;

  // Copy the data & make CSUM
  for (i = 0; i < length; i++) {
    msgbuffer[i+1] = data[i];
    CSUM += data[i];
  }
  msgbuffer[i+1] = CSUM;

  tmp=COBS_encode128(databuffer,length+2);
  databuffer[tmp++] = 0; //EOM

  tmp = serialport->write((char*)databuffer,tmp);

  wait = 500;
  while ( wait && serialport->available() < 1) {
    wait--;
    delay(1);
  }

  sermsg_recv_reply(serialport, 1); // Want 1 byte return code

  if (recvbuffer[0] == '+') {
    digitalWrite(13,LOW);
    return(0);
  }

  digitalWrite(13,HIGH);
  return(1);
}
