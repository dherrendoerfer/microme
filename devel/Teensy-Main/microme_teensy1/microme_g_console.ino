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

/*
  The 'very simple console'
  it outputs messages to the serial and graphic console.
*/
static const char *prompt = ">";
static const char *cursor = "_";

void microme_console_prompt()
{
  uvga.print(prompt);    
}

void microme_console_start()
{
  uvga.print(cursor);
}

void microme_console_update(uint8_t atom)
{
  uvga.print("\b");
  uvga.print((char)atom);  
  uvga.print(cursor);  
}

void microme_console_end()
{
  uvga.print("\b");
  uvga.println();  
}
