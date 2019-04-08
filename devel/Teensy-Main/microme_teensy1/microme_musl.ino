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
  mu_add_func(m, "settime", my_settime);
  
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
 *# Sends a signal to device B
 */
static struct mu_par my_pokea(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  sermsg_send_short(&Serial1, mu_par_int(m, 0, argc, argv), 
                              mu_par_int(m, 1, argc, argv), 
                              mu_par_int(m, 2, argc, argv));

  return rv;
}

/*@ ##pokeb( dev, addr, val )
 *# Sends a signal to device B
 */
static struct mu_par my_pokeb(struct musl *m, int argc, struct mu_par argv[]) {
  struct mu_par rv;

  sermsg_send_short(&Serial1, mu_par_int(m, 0, argc, argv), 
                              mu_par_int(m, 1, argc, argv), 
                              mu_par_int(m, 2, argc, argv));

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

