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

#define MAX_BASIC_LINES 1500

static uint16_t end_ptr = 0;
static int32_t highest_line_num = 0;

static int16_t basic_line_idx[MAX_BASIC_LINES];

uint8_t microme_basic_new()
{
  memset(basic_prog, 0, sizeof(basic_prog));
  memset(basic_line_idx, -1, sizeof(basic_line_idx));
  end_ptr = 0;
  highest_line_num = 0;

  return(0);
}

// The Line-Based editor, This could probably be a lot simpler
uint8_t microme_basic_addline(int16_t num, char *line)
{
  uint16_t line_ptr;
  int16_t next_idx;
 
  // should make sure it fits
  // Appending to the end
  if (num > highest_line_num) {
    line_ptr = end_ptr;
    strcpy(&basic_prog[line_ptr],line);
    basic_prog[line_ptr + strlen(line)  ] = '\n';
    basic_prog[line_ptr + strlen(line) +1 ] = 0;
    basic_line_idx[num] = line_ptr;
    end_ptr += strlen(line) +1;
    highest_line_num = num;
  } 
  else if (basic_line_idx[num] == -1) {
    // inserting a line
    next_idx = num;

    // find the next (following) line
    while (basic_line_idx[next_idx] == -1)
      next_idx++;

    memmove(&basic_prog[basic_line_idx[next_idx] + strlen(line) + 1 ],
           &basic_prog[basic_line_idx[next_idx]], 
           end_ptr - basic_line_idx[next_idx]);
    line_ptr = basic_line_idx[next_idx];

    // add the line text
    memcpy(&basic_prog[line_ptr],line,strlen(line));
    basic_prog[line_ptr + strlen(line)  ] = '\n';
    basic_line_idx[num] = line_ptr;
    end_ptr += strlen(line) + 1;

    // shift the next_index
    basic_line_idx[next_idx] += strlen(line) + 1;

    // shift all folowing indexes
    for (; next_idx < MAX_BASIC_LINES; next_idx++) {
      if (basic_line_idx[next_idx] != -1)
        basic_line_idx[next_idx] += strlen(line) +1;
    }
  }
  else {
    //replacing a line 
    int16_t val = 0;

    next_idx = num + 1;

    // find the next (following) line
    while (basic_line_idx[next_idx] == -1) {
      next_idx++;
      if (next_idx == MAX_BASIC_LINES) {
        next_idx = num; //there is no next line ... 
        break;
      }
    }

    if (next_idx != num) {
      //need to size the gap between the 2 indexes so the new line fits
      if (strlen(line) + 1 != basic_line_idx[next_idx] - basic_line_idx[num]) {
        //need to enlarge
        val = strlen(line) + 1 - (basic_line_idx[next_idx] - basic_line_idx[num]);
    
        memmove(&basic_prog[basic_line_idx[next_idx] + val ],
                &basic_prog[basic_line_idx[next_idx]], 
                end_ptr - basic_line_idx[next_idx]);
    
        // shift the next_index
        basic_line_idx[next_idx++] += val;
      
        // shift all folowing indexes
        for (; next_idx < MAX_BASIC_LINES; next_idx++) {
          if (basic_line_idx[next_idx] != -1)
            basic_line_idx[next_idx] += val;
        }
    
        // shift the end pointer
        end_ptr += val;
        basic_prog[end_ptr] = 0; //terminate the string
       }
      // else  
        // It fits

      // finally, copy the new line in place of the old one
      memcpy(&basic_prog[basic_line_idx[num]],line,strlen(line));
      basic_prog[basic_line_idx[num] + strlen(line)  ] = '\n';
    }
    else {
      //It's the last line ....
      memcpy(&basic_prog[basic_line_idx[next_idx]],line,strlen(line));
      basic_prog[basic_line_idx[next_idx] + strlen(line)  ] = '\n';
      basic_prog[basic_line_idx[next_idx] + strlen(line) +1 ] = 0;
      end_ptr = basic_line_idx[next_idx] + strlen(line) +1;
    }
  }
  //  uvga.println(basic_prog);

  return(0);
}

