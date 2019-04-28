/*
 * microME general definitions file
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


#ifdef TEENSY_MAIN

#define DEBUG_PRINT(args ...) uvga.print(args)
#define DEBUG_PRINTLN(args ...) uvga.println(args)

#define TARGET_ARDUINO &Serial1
#define TARGET_TEENSY2 &Serial2

#endif

#ifdef TEENSY_COPRO

#define DEBUG_PRINT(args ...) Serial.print(args)
#define DEBUG_PRINTLN(args ...) Serial.println(args)

#define TARGET_TEENSY &Serial1

#endif

#ifdef ARDUINO_IO

//#define DEBUG_PRINT(args ...) Serial.print(args)
//#define DEBUG_PRINTLN(args ...) Serial.println(args)
#define DEBUG_PRINT(args ...)
#define DEBUG_PRINTLN(args ...)

#define TARGET_TEENSY &Serial

#endif

/*Transfer Sizes*/
#define MAX_FILE_CHUNK_SIZE 64
#define FILE_TRANSFER_BUFFER (MAX_FILE_CHUNK_SIZE + 1)
#define MAX_FILENAME_LENGTH 32

/*File status flags */
#define FILE_STATUS_OPEN   0x01
#define FILE_STATUS_WRITE  0x02
