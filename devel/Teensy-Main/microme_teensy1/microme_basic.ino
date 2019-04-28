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

#define MAX_BASIC_LINES 1418

static uint16_t end_ptr = 0;
static int32_t highest_line_num = 0;

static int16_t basic_line_idx[MAX_BASIC_LINES];

void microme_basic_new()
{
  memset(basic_prog, 0, sizeof(basic_prog));
  memset(basic_line_idx, -1, sizeof(basic_line_idx));
  end_ptr = 0;
  highest_line_num = 0;

  return;
}

// The Line-Based editor, This could probably be a lot simpler
uint8_t microme_basic_addline(uint16_t num, char *line)
{
  uint16_t line_ptr;
  int16_t next_idx;
  int16_t val = 0;

  if (num > MAX_BASIC_LINES) {
    uvga.println("ERROR: Line number too high");
    return (1);
  }

  if (isanumber(line)) {
    // Empty line string - remove the line
    if (basic_line_idx[num] == -1) {
      // line does not exist, just return
      return(0);
    }

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
      val = basic_line_idx[next_idx] - basic_line_idx[num];
  
      memmove(&basic_prog[basic_line_idx[next_idx] - val ],
              &basic_prog[basic_line_idx[next_idx]], 
              end_ptr - basic_line_idx[next_idx]);
  
      // shift the next_index
      basic_line_idx[num] = -1;
      basic_line_idx[next_idx++] -= val;
    
      // shift all folowing indexes
      for (; next_idx < MAX_BASIC_LINES; next_idx++) {
        if (basic_line_idx[next_idx] != -1)
          basic_line_idx[next_idx] -= val;
      }
  
      // shift the end pointer
      end_ptr -= val;
      basic_prog[end_ptr] = 0; //terminate the string
    }
    else {
      //It's the last line ....
      basic_prog[basic_line_idx[next_idx]] = 0;
      end_ptr = basic_line_idx[next_idx];
      basic_line_idx[next_idx] = -1;

      //just deleted the last line ... calculate new highest line number

      next_idx--;
      for (; next_idx > 0; next_idx--) {
        if (basic_line_idx[next_idx] != -1)
          break;
      }

      highest_line_num = next_idx;
      
    }   
  }
  else if (num > highest_line_num) {
  // should make sure it fits
  // Appending to the end
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

    // shift the next_index
    basic_line_idx[next_idx] += strlen(line) + 1;

    next_idx++;
    // shift all folowing indexes
    for (; next_idx < MAX_BASIC_LINES; next_idx++) {
      if (basic_line_idx[next_idx] != -1)
        basic_line_idx[next_idx] += strlen(line) +1;
    }
    
    // shift end ptr
    end_ptr += strlen(line) + 1;
    basic_prog[end_ptr] = 0;
  }
  else {
    //replacing a line 
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

// DEBUG output: (current program)
/*
  for (val=0;val<MAX_BASIC_LINES;val++)
    if (basic_line_idx[val] != -1) {
      uvga.print("Line: ");
      uvga.print(val);
      uvga.print(" Index: ");
      uvga.println(basic_line_idx[val]);
    }

    uvga.print("highest_line_num: ");
    uvga.println(highest_line_num);
    uvga.print("end_ptr: ");
    uvga.println(end_ptr);

  uvga.println(basic_prog);
*/
  return(0);
}

static void make_index()
{
  // Run through the file and try to set the line-number makers  
  uint8_t i, number[8];
  uint16_t num, pos = 0;

  uvga.print("Indexing ... ");

  while (pos < end_ptr) {
    for (i=0; i<8; i++) {
      if (basic_prog[pos+i] > '9' || basic_prog[pos+i] < '0') {
        break;
      }
      else {
        number[i] = basic_prog[pos+i];
      }
    }
    
    if (i == 8 || i == 0) {
      //Line number too long or short, bail out
      uvga.println("Indexing failed");
      return;       
    }
    else {
      number[i] = 0; //Terminate string
      num=atoi((char*)number);
      basic_line_idx[num] = pos;
      highest_line_num = num;
      uvga.print((int)num);
      uvga.print(", ");
    }

    while (pos < end_ptr) {
      if (basic_prog[pos++] == '\n' ) 
        break; 
    }
  }

  uvga.println();
}

uint8_t microme_basic_load(char *filename) 
{
  uint16_t pos = 0;
  if (strcmp(filename,"/") == 0) {
    return reinit(); 
  }
  
  microme_basic_new();

  uvga.print("Loading ");
  uvga.print(filename);
  uvga.println(" ...");

  if (open(0,filename)) {
    while (available(0)) {
      uint8_t len = read(0, (uint8_t*)basic_prog, pos, MAX_FILE_CHUNK_SIZE);
      pos+=len;
    }
  }
  else {
    uvga.println("ERROR!: Can't open file");
    return(1);
  }
  close(0);
  
  end_ptr = pos;
  make_index();
  return(0);
}

uint8_t microme_basic_save(char *filename) 
{
  uint16_t pos = 0;
  uint8_t len = 0;

  uvga.print("Saving ");
  uvga.print(filename);
  uvga.println(" ...");

  if (open_w(0,filename)) {
    while (pos < end_ptr) {
      len = end_ptr - pos;
      if (len > MAX_FILE_CHUNK_SIZE)
        len = MAX_FILE_CHUNK_SIZE;
      write(0, (uint8_t*)basic_prog, pos, len);
      pos+=len;
    }
  }
  else {
    uvga.println("ERROR!: Can't open file");
    return(1);
  }
  close(0);
  
  end_ptr = pos;
  make_index();
  return(0);
}
