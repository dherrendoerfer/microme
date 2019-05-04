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


static void microme_musl_exec(char *line)
{
  struct musl *m;

  if(!(m = mu_create())) {
    uvga.println("ERROR: Couldn't create MUSL structure");
    return;
  }
  mu_add_func(m, "print", my_print);
  mu_add_func(m, "cls", my_cls);
  mu_add_func(m, "wait", my_wait);
  mu_add_func(m, "pokea", my_pokea);
  mu_add_func(m, "pokeb", my_pokeb);
  mu_add_func(m, "peeka", my_peeka);
  mu_add_func(m, "peekb", my_peekb);
  
  mu_add_func(m, "settime", my_settime);

  mu_add_func(m, "random", my_rand);
  mu_add_func(m, "drawline", my_drawline);
  mu_add_func(m, "setcolor", my_setcolor);
  mu_add_func(m, "setpos", my_setpos);

  mu_add_func(m, "input$", my_input_s);
  mu_add_func(m, "input", my_input);
  mu_add_func(m, "get$", my_get_s);

  //cancel any outstanding BREAKs
  signal_break = 0;

  if(!mu_run(m, line, &signal_break)) {
    uvga.print("ERROR:Line ");
    uvga.print( mu_cur_line(m));
    uvga.print(": ");
    uvga.println(mu_error_msg(m));
    uvga.println(mu_error_text(m));
  }

  if (signal_break) {
    microme_write_ioconsole("STOPPED by BREAK");
    signal_break = 0;
  }

  mu_cleanup(m);
}


/*@ ##PRINT(exp1, exp2, ...)
 *# Prints all its parameters, followed by a newline
 */
static struct mu_par my_print(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv = {mu_int, {argc}};
  int i;
  for(i = 0; i < argc; i++) {
    if(i > 0)
      uvga.print(" ");
    if(argv[i].type == mu_str)
      uvga.print(argv[i].v.s);
    else
      uvga.print((long)argv[i].v.i);
  }
  uvga.println();
  return rv;
}

/*@ ##CLS()
 *# Clears the screen
 */
static struct mu_par my_cls(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  uvga.clear();
  uvga.moveCursor(0,0);
  return rv;
}

/*@ ##wait( time )
 *# waits <time> in ms
 */
static struct mu_par my_wait(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  delay(mu_par_int(m, 0, argc, argv));

  return rv;
}

/*@ ##pokea( dev, addr, val )
 *# Sends a byte to device A
 */
static struct mu_par my_pokea(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  sermsg_send_short(TARGET_ARDUINO, mu_par_int(m, 0, argc, argv), 
                              mu_par_int(m, 1, argc, argv), 
                              mu_par_int(m, 2, argc, argv));

  return rv;
}

/*@ ##pokeb( dev, addr, val )
 *# Sends a byte to device B
 */
static struct mu_par my_pokeb(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  sermsg_send_short(TARGET_ARDUINO, mu_par_int(m, 0, argc, argv), 
                              mu_par_int(m, 1, argc, argv), 
                              mu_par_int(m, 2, argc, argv));

  return rv;
}

/*@ ##peeka( dev, addr )
 *# Reads a byte from device A
 */
static struct mu_par my_peeka(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;
  uint8_t data;

  rv.type = mu_int;
  rv.v.i = 0;

  if(!sermsg_get_short(TARGET_ARDUINO, mu_par_int(m, 0, argc, argv), 
                              mu_par_int(m, 1, argc, argv), 
                              &data)) {

    rv.v.i = data;
  }  
      
  return rv;
}

/*@ ##peekb( dev, addr )
 *# Reads a byte from device B
 */
static struct mu_par my_peekb(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;
  uint8_t data;

  rv.type = mu_int;
  rv.v.i = 0;

  if(!sermsg_get_short(TARGET_TEENSY2, mu_par_int(m, 0, argc, argv), 
                              mu_par_int(m, 1, argc, argv), 
                              &data)) {

    rv.v.i = data;
  }  
      
  return rv;
}

/*@ ##SETTIME([fmt])
 */
static struct mu_par my_settime(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;
  int _hr, _min, _sec, _day, _month, _yr;
  
  if(argc < 6) {
    mu_throw(m, "settime requires 6 arguments");
    return rv;
  }
  
  _hr=mu_par_int(m, 0, argc, argv);
  _min=mu_par_int(m, 1, argc, argv);
  _sec=mu_par_int(m, 2, argc, argv);
  _day=mu_par_int(m, 3, argc, argv);
  _month=mu_par_int(m, 4, argc, argv);
  _yr=mu_par_int(m, 5, argc, argv);
    
  setTime(_hr, _min, _sec, _day, _month, _yr); 

  return rv;
}

/*@ ##RANDOM() RANDOM(N) RANDOM(N,M)
 *{
 ** {{RANDOM()}} - Chooses a random number
 ** {{RANDOM(N)}} - Chooses a random number in the range [1, N]
 ** {{RANDOM(N,M)}} - Chooses a random number in the range [N, M]
 *}
 */
  static struct mu_par my_rand(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv = {mu_int, {0}};
  rv.v.i = rand();

  if(argc == 1) {
    int f = mu_par_int(m,0, argc, argv);
    if(f <= 0) 
      mu_throw(m, "Parameter to RANDOM(N) must be greater than 0");
    rv.v.i = (rv.v.i % f) + 1;
  } 
  else if(argc == 2) {
    int f = (mu_par_int(m,1, argc, argv) - mu_par_int(m,0, argc, argv) + 1);
    if(f <= 0) 
      mu_throw(m, "Parameters to RANDOM(N,M) must satisfy M-N+1 > 0");
    rv.v.i = (rv.v.i % f) + mu_par_int(m,0, argc, argv);
  }

  return rv;
}

/*@ ##drawline( x1,y1, x2,y2, color)
 *# draw a line
 */
static struct mu_par my_drawline(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  uvga.drawLine( mu_par_int(m, 0, argc, argv), 
                 mu_par_int(m, 1, argc, argv), 
                 mu_par_int(m, 2, argc, argv),
                 mu_par_int(m, 3, argc, argv),
                 mu_par_int(m, 4, argc, argv), 0);

  return rv;
}

/*@ ##setcolor( back, front )
 *# set colors
 */
static struct mu_par my_setcolor(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  uvga.setBackgroundColor(mu_par_int(m, 0, argc, argv));
  uvga.setForegroundColor(mu_par_int(m, 1, argc, argv));

  return rv;
}

/*@ ##setpos( line, col )
 *# set cursor position
 */
static struct mu_par my_setpos(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  uvga.moveCursor( mu_par_int(m, 0, argc, argv),
                   mu_par_int(m, 1, argc, argv));

  return rv;
}

/*@ ##input$( prompt )
 *# get an input line string
 *# get an input number
 */
static struct mu_par my_input_s(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  uint16_t i;

  if(argc == 0)
    uvga.print("> ");
  else {
    uvga.print(mu_par_str(m, 0, argc, argv));
    uvga.print(' ');
  }

  rv.type = mu_str;
  rv.v.s = (char*)malloc(INPUT_BUFFER_SIZE + 1);
  if(!rv.v.s) {
    mu_throw(m, "Out of memory");
  }

  rv.v.s[0] = '\0';
  if(get_line((uint8_t*)rv.v.s, INPUT_BUFFER_SIZE)) {
    for(i=0; i<INPUT_BUFFER_SIZE; i++) {
      if(rv.v.s[i] == '\n'){
        rv.v.s[i] = 0;
        break;
      }
    }
  }
  if(argc > 1) {
    const char * name = mu_par_str(m, 1, argc, argv);
    mu_set_str(m, name, rv.v.s);
  }
      
  return rv;
}

static struct mu_par my_input(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;
  uint8_t buffer[50];

  if(argc == 0)
    uvga.print("> ");
  else {
    uvga.print(mu_par_str(m, 0, argc, argv));
    uvga.print(' ');
  }

  rv.type = mu_int;
  rv.v.i = 0;
  if(get_line(buffer, sizeof buffer - 1))
    rv.v.i = atoi((char*)buffer);
  
  if(argc > 1) {
    const char * name = mu_par_str(m, 1, argc, argv);
    mu_set_int(m, name, rv.v.i);
  }
      
  return rv;
}

/*@ ##get$()
 *# get a key char
 */
static struct mu_par my_get_s(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  rv.type = mu_str;
  rv.v.s = (char*)malloc(2);
  if(!rv.v.s) {
    mu_throw(m, "Out of memory");
  }

  rv.v.s[0] = 0;
  rv.v.s[1] = 0;

  if(microme_input_available()){
    microme_read_input((uint8_t*)&rv.v.s[0]);
  }

  if(argc > 1) {
    const char * name = mu_par_str(m, 1, argc, argv);
    mu_set_str(m, name, rv.v.s);
  }
      
  return rv;
}



