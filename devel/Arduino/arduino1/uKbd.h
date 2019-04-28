/*
 * This file is part of uLibx.
 *
 * Copyright (C) 2018  D.Herrendoerfer
 *
 *   uLibx is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   uLibx is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with uLibx.  If not, see <http://www.gnu.org/licenses/>.
 *
*/

/* Note: Data  pin must be on PORTD (D0 - D8) 
 *       Clock pin must be interruptible ( D2, D3 on Nano)  */

static volatile uint8_t uKbd_buffer[32]; 
static volatile uint8_t uKbd_ptr_floor = 0;
static volatile uint8_t uKbd_ptr_ceil = 0;

static volatile uint8_t uKbd_ignoreirq = 0;

static uint8_t uKbd_irqpin = 0; 
static uint8_t uKbd_datapin = 0; 
static uint8_t uKbd_datamask = 0; 

static volatile uint8_t uKbd_parity_err = 0; 
static volatile uint8_t uKbd_stopbit_err = 0; 


void uKBD_irq()
{
  static uint8_t uKbd_seq = 0;
  static uint8_t uKbd_parity = 0;
  static uint8_t uKbd_scancode = 0;
  static uint8_t uKbd_devdata = 0;
  static uint8_t uKbd_lastdata = 1;
  
  static uint32_t uKbd_tm;
  static uint32_t uKbd_tm_prev = 0;

  if (uKbd_ignoreirq)
    return;
    
  uKbd_tm=millis();

  uKbd_devdata = PIND & uKbd_datamask;
    
  // Timeout catch
  if ( uKbd_tm - uKbd_tm_prev > 100) {
    uKbd_seq = 0;
    uKbd_scancode = 0;
    uKbd_parity = 0;
  }

  switch(uKbd_seq) {
  case 0: // Start bit
    if (uKbd_lastdata && uKbd_devdata==0)
      uKbd_seq++;
    break;  
  case 1: // Bit0
  case 2: // Bit1
  case 3: // Bit2
  case 4: // Bit3
  case 5: // Bit4
  case 6: // Bit5
  case 7: // Bit6
  case 8: // Bit7
    if (uKbd_devdata) {
      uKbd_parity++;
      uKbd_scancode |= (1 << (uKbd_seq-1));
    }
    uKbd_seq++;
    break;
  case 9: // Parity (Odd)
    if (uKbd_devdata) 
      uKbd_parity++;
    if (uKbd_parity & 0x01 == 0) {
      // Parity error
      pinMode(uKbd_irqpin, OUTPUT);
      digitalWrite(uKbd_irqpin, LOW);
      delayMicroseconds(50);
      pinMode(uKbd_irqpin, INPUT_PULLUP);
      uKbd_seq = 0;
      uKbd_scancode = 0;
      uKbd_parity = 0;
      uKbd_parity_err++;
    }
    else {
      uKbd_seq++;
    }
    break;
  case 10: // Stop bit
    if (uKbd_devdata) {
      uKbd_buffer[uKbd_ptr_ceil++] = uKbd_scancode;
      uKbd_ptr_ceil = uKbd_ptr_ceil & 0x1F; 
    }
    else {
      uKbd_stopbit_err++;  
    }
    uKbd_seq = 0;
    uKbd_scancode = 0;
    uKbd_parity = 0;
    break;
  }

  uKbd_lastdata = uKbd_devdata;
  uKbd_tm_prev = uKbd_tm;
  return;  
}

static uint8_t uKbd_send_byte(uint8_t data)
{
  /* Note: there are no timouts in this code. According to the
   * standard, the keyboard will continue sending clock signals
   * until the host picks the line back up. */

  uint8_t parity=0;  

  uKbd_ignoreirq = 1;

  pinMode(uKbd_datapin, OUTPUT);
  pinMode(uKbd_irqpin, OUTPUT);

  //Start
  digitalWrite(uKbd_irqpin, LOW);
  delayMicroseconds(100);
  digitalWrite(uKbd_datapin, LOW);
  pinMode(uKbd_irqpin, INPUT_PULLUP);
  while (digitalRead(uKbd_irqpin) == HIGH);

  //Data
  for (int i=0; i<8; i++) {
    if (data & (1<<i)) {
      digitalWrite(uKbd_datapin, HIGH);
      parity++;
    }
    else {
      digitalWrite(uKbd_datapin, LOW);
    }
    
    while (digitalRead(uKbd_irqpin) == LOW);
    while (digitalRead(uKbd_irqpin) == HIGH);
  }

  //Parity (Odd)
  if (parity & 0x00 != 0) {
    digitalWrite(uKbd_datapin, HIGH);
  }
  else {
    digitalWrite(uKbd_datapin, LOW);
  }

  while (digitalRead(uKbd_irqpin) == LOW);
  while (digitalRead(uKbd_irqpin) == HIGH);

  //Ack check
  pinMode(uKbd_datapin, INPUT_PULLUP);
  while (digitalRead(uKbd_datapin) == HIGH);
  while (digitalRead(uKbd_irqpin) == HIGH);
  //Release
  while (digitalRead(uKbd_datapin) == LOW);
  while (digitalRead(uKbd_irqpin) == LOW);  

  uKbd_ignoreirq = 0;
}

uint8_t uKbd_available()
{
  if (uKbd_ptr_ceil < uKbd_ptr_floor)
    return(32 - uKbd_ptr_floor + uKbd_ptr_ceil);
  else
    return(uKbd_ptr_ceil - uKbd_ptr_floor);
}

uint8_t uKbd_read()
{
  uint8_t ret;

  if (uKbd_ptr_ceil != uKbd_ptr_floor) {
    ret = uKbd_buffer[uKbd_ptr_floor++];
    uKbd_ptr_floor = uKbd_ptr_floor & 0x1F;
    return ret;
  }
  
  while (uKbd_ptr_ceil == uKbd_ptr_floor);
  ret = uKbd_buffer[uKbd_ptr_floor++];
  uKbd_ptr_floor = uKbd_ptr_floor & 0x1F;
  return ret;
}

int uKbd_start(uint8_t datapin, uint8_t irqpin)
{
  uKbd_irqpin = irqpin;
  uKbd_datapin = datapin;
  uKbd_datamask = 1<<datapin;
  
  pinMode(uKbd_irqpin, INPUT_PULLUP);
  pinMode(uKbd_datapin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(uKbd_irqpin), uKBD_irq, FALLING);

  // send reset
  uKbd_send_byte(0xFF);
  uKbd_send_byte(0xFA);
  
  return 0;
}

static uint8_t uKbd_send_command(uint8_t cmd, uint8_t data)
{ 
  uint8_t ret;
  uKbd_send_byte(cmd);

  ret=uKbd_read();
  if (ret == 0xFA)
    uKbd_send_byte(data);
  
  ret=uKbd_read();
  if (ret == 0xFA)
    return 0;

  return 1;
}

uint8_t uKbd_lights(uint8_t set)
{
   return uKbd_send_command(0xED, (set & 0x07));
}

