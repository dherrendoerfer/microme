
/*
 * microME Teensy Main-Processor sample firmware
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

#define TEENSY_MAIN 1
#include <microME.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*RTC*/
#include <TimeLib.h>

/*MUSL*/
#define WITH_REGEX 1
#ifdef WITH_REGEX
#include <sys/types.h>
#include <regex.h>
#endif

#include "musl.h"

#define INPUT_BUFFER_SIZE 256

/*VGA*/
#include <uVGA.h>
uVGA uvga;

/*SERIAL BUS*/
#include <sermsg.h>

#define UVGA_DEFAULT_REZ
#include <uVGA_valid_settings.h>

UVGA_STATIC_FRAME_BUFFER(uvga_fb);

/* Global variables */
  /*Kbd buffer*/
volatile static uint8_t kbd_buffer[32] = {0};
volatile uint8_t kbd_buff_ceil = 0;
volatile uint8_t kbd_buff_floor = 0;
  /*CTRL+C catch*/
volatile uint8_t signal_break = 0;

  /*Basic Progmem*/
static char basic_prog[64*1024] = {0};

const uint32_t FreeMem(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    if (hTop == NULL)
      uvga.println("malloc(1) failed!");
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is (approximately) the free, available ram.
    return stackTop - heapTop;
}

static time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

void setup()
{
  int ret;

  uvga.set_static_framebuffer(uvga_fb);

  Serial1.begin(115200);
  Serial2.begin(500000);
  
  ret = uvga.begin(&modeline);

  if(ret != 0) {
    while(1);
  }
  
  pinMode(13,OUTPUT); //LED
  pinMode(11,INPUT); // Arduino Input Event
  pinMode(12,INPUT); // Teensy2 Input Event
  setup_kbd(11);
  setup_ioirq(12);
}

void loop() {
  /**/
  uvga.setBackgroundColor(0);
  uvga.setForegroundColor(255);

  uvga.clear();

  uvga.println();
  uvga.print("   the ");
  uvga.setForegroundColor(B11100000);
  uvga.print("mi");
  uvga.setForegroundColor(B00011100);
  uvga.print("cro");
  uvga.setForegroundColor(B00000011);
  uvga.print("Me");
  uvga.setForegroundColor(255);
  uvga.println(" home computer");
  uvga.println("         by dherrendoerfer");
  uvga.println("");
  uvga.println("    firmware version 0.80 beta ");
  uvga.println("");
  uvga.print("    basic program memory:    ");
  uvga.print(sizeof(basic_prog));
  uvga.println(" bytes");
  uvga.print("    available system memory: ");
  uvga.print(FreeMem());
  uvga.println(" bytes");
  uvga.print("    used framebuffer memory: ");
  uvga.print(UVGA_FB_SIZE(UVGA_HREZ, UVGA_VREZ, UVGA_RPTL, UVGA_TOP_MARGIN, UVGA_BOTTOM_MARGIN));
  uvga.println(" bytes");
  
  setSyncProvider(getTeensy3Time);   // the function to get the time from the RTC

  if(timeStatus()!= timeSet) 
    uvga.println("    RTC is not set.");
  else {
    uvga.print("    RTC date: ");
    uvga.print(month());
    uvga.print("/");
    uvga.print(day());
    uvga.print("/");
    uvga.print(year());
    uvga.print("  time: ");
    uvga.print(hour());
    uvga.print(":");
    uvga.print(minute()/10);
    uvga.println(minute()%10);
  }

/*
delay (1000);
  uint8_t buff[120];
  sprintf((char*)buff, "file.bas");

  uvga.println("Opening remote file file.bas");

  open(0,"file.bas");
  if ( stat(0) & FILE_STATUS_OPEN )
    uvga.println("File is open");
  else
    uvga.println("File is closed");

  uvga.print("Avail: ");
  uvga.println(available(0));

  while (available(0)) {
    uint8_t len = read(0,buff,0,65);
    for (int i=0; i< len; i++)
      uvga.print((char)buff[i]);
  }
  uvga.println();

  uvga.println("Closing remote file file.bas");
  close(0);

  if ( stat(0) & FILE_STATUS_OPEN )
    uvga.println("File is open");
  else
    uvga.println("File is closed");
*/
  
  microme_basic_new();
  
  while (true) {
    microme_shell();
  }

}
