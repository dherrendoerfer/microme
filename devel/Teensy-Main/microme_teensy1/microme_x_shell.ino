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

#define KEY_ENTER 13
#define KEY_BACKSPACE 8

static uint8_t isanumber(char *val)
{
  int i;
  for (i=0; i< strlen(val); i++) {
    if (val[i] <= '9' && val[i] >= '0')
      continue;
    else
      return 0;
  }
  return 1;
}

uint8_t get_line(uint8_t *line, uint16_t length)
{
  uint8_t line_length = 0;
  uint8_t key;

  microme_console_start();  

  /*Get a line from the input sources */
  while(true) {
    if(microme_input_available()){
      // BREAK KEY 
      microme_read_input(&key);
      if (key == 0x03) {
        microme_console_end();
        return 1;
      }
      //Line editing
      if (key == KEY_BACKSPACE) {
        if (line_length > 0) {
          line_length--;
          line[line_length] = 0;
          microme_console_update(key);
        }
      }
      else if (key != KEY_ENTER ) {
        if (line_length < length - 1) {
          line[line_length++] = key;
          line[line_length] = 0; // push the line end ahead
          microme_console_update(key);
        }
      }
      else
        break;
    }
  }
  microme_console_end();

  return(line_length);
}

static uint8_t line[INPUT_BUFFER_SIZE] = {0};
static uint8_t args[INPUT_BUFFER_SIZE] = {0};

static uint8_t microme_shell()
{
  uint8_t line_length = 0;
  uint8_t i;
  uint8_t argc;
  char *argv[64]; //hard limit

  uvga.println("Ready.");

  while (true) {
    line[0]=0;
    args[0]=0;
    i = 0;
    argc = 1;

    /* Read a line from the console */
    microme_console_prompt();  
    line_length = get_line(line, sizeof(line));

    /* Empty line bailout */
    if (line_length == 0)
      continue;
  
    /*Debug*/
//    microme_write_ioconsole((char*)line);
  
    /*Make a cheap argc, argv*/
    memcpy(args,line,256);
  
    i = 1;
    while (i < line_length) {
      switch(args[i]){
      case '\\':
        i++;
        break;
      case ' ':
        args[i] = 0;
        break;
      case '\"':
        args[i++] = 0;
        while (i<line_length) {
          if (args[i] == '\\')
            i++;
          else if (args[i] == '\"') {
            args[i] = 0;
            break;
          }
          i++;
        }
        break;
      case '\'':
        args[i++] = 0;
        while (i<line_length) {
          if (args[i] == '\\')
            i++;
          else if (args[i] == '\'') {
            args[i] = 0;
            break;
          }
          i++;
        }
        break;
      }
      i++;
    }
  
    argc = 1;
    argv[0]=(char*)&args[0];
    i=0;
    
    while (i < line_length) {
      if (args[i] == 0) {
        while (args[i] == 0 && i < line_length)
          i++;
        if (i < line_length)
          argv[argc++]=(char*)&args[i];  
      }
      i++;
    }
  
    /*DEBUG output*/
/*    
    uvga.print("argc = ");
    uvga.println(argc);
  
    for (i=0; i<argc; i++) {
      uvga.print("argv[");
      uvga.print(i);
      uvga.print("] = ");
      uvga.println(argv[i]);
    }
*/  
    /*Shell builtins:*/
  
    if (isanumber(argv[0])) {
//      uvga.println("is a number");
//      uvga.println(atoi(argv[0]));
  
      if (atoi(argv[0]) > 0)
        microme_basic_addline(atoi(argv[0]),(char*)line);
      else
        uvga.println("WARN: invalid line number");
        
      continue;
    } 
    else if (strcmp(argv[0],"list") == 0) {
      uvga.print(basic_prog);
      continue;
    }
    else if (strcmp(argv[0],"run") == 0) {
      microme_musl_exec(basic_prog);
      return(0);
    }
    else if (strcmp(argv[0], "new") == 0) {
      microme_basic_new();
      return(0);
    }
    else if (strcmp(argv[0],"music") == 0) {
      music();
      return(0);
    }
    else if (strcmp(argv[0],"load") == 0) {
      microme_basic_load(argv[1]);
      return(0);
    }
    else if (strcmp(argv[0],"save") == 0) {
      microme_basic_save(argv[1]);
      return(0);
    }

    break;
  }

  microme_musl_exec((char*)line);

  return(0);
}

