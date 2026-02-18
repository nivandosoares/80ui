/******************************************************************
 * SEAL 2.0                                                       *
 * Copyright (c) 1999-2002 SEAL Developers. All Rights Reserved.  *
 *                                                                *
 * Web site: http://sealsystem.sourceforge.net/                   *
 * For e-mail (current maintainer) look in readme.txt             *
 ******************************************************************/

/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/****************************************************************/
/*                                                              */
/*                           sscript.c                          */
/*                      SealScript Library                      */
/*                             v0.1                             */
/*                                                              */
/*        (c) 2001 Kostas Michalopoulos aka Bad Sector          */
/*                                                              */
/****************************************************************/

#include <pc.h>
#include <dos.h>

#include <allegro.h>
#include <seal.h>
#include <app.h>
#include <sscript.h>

/* Do NOT use any command with 0 */
/* Maximum value is 250 */
#define CMD_SET         1
#define CMD_PRINT       2
#define CMD_IF          3
#define CMD_END         4
#define CMD_ADD         5
#define CMD_SUB         6
#define CMD_MUL         7
#define CMD_DIV         8
#define CMD_OPEN        9
#define CMD_WRITE       10
#define CMD_READ        11
#define CMD_CLOSE       12
#define CMD_ERASE       13
#define CMD_BEEP        14
#define CMD_STOP        15
#define CMD_GOTO        16
#define CMD_TIME        17
#define CMD_MULTITASK   18
#define CMD_DEFINED    255

p_sscript (*sscript_init)(p_sscript o) = &_sscript_init;
p_sscript_ (*sscript__init)(p_sscript_ o) = &_sscript__init;

typedef struct t_fileinf *p_fileinf;
typedef struct t_fileinf
{
  l_text name;
  l_int mode;
  l_int pos;
} t_fileinf;

typedef struct t_labelinf *p_labelinf;
typedef struct t_labelinf
{
  l_text name;
  l_int pos;
  p_labelinf next;
} t_labelinf;

typedef struct t_usercmd *p_usercmd;
typedef struct t_usercmd
{
  l_text name;
  l_int mempos;
  p_usercmd next;
} t_usercmd;

l_bool sscript_done(p_object o)
{
  if (!obj_done(o)) return false;
  free(SSCRIPT(o)->ss->code);
  free(SSCRIPT(o)->ss->c);
  //SSCRIPT(o)->ss->done(SSCRIPT(o)->ss);
  return true;
}

p_sscript _sscript_init(p_sscript o)
{
  if (!o) return NULL;
  clear_type(o, sizeof(t_sscript));
  obj_init(&o->obclass);

  OBJECT(o)->done = &sscript_done;

  o->obclass.tag = TAG_OBJECT;

  o->ss = _sscript__init(malloc(sizeof(t_sscript_)));
  OBJECT(o)->insert(OBJECT(o), OBJECT(o->ss));

  o->version = 0; /* version 0.1 */
  o->ver_hi = 0;  /* 0.x */
  o->ver_lo = 1;  /* x.1 */

  o->set_code = &sscript_set_code;
  o->compile = &sscript_compile;
  o->define_command = &sscript_define_command;
  o->var_hook = NULL;
  o->execute = &sscript_execute;

  return o;
}

/* sets SealScript code to code. To use this code you have first
to compile it using compile() function. */
void sscript_set_code(p_sscript o, l_text code)
{
  if (o->ss->code) free(o->ss->code);
  o->ss->code = strdup(code);
}

/* compiles SealScript code to p-code for faster executing */
l_bool sscript_compile(p_sscript o)
{
  l_bool r = o->ss->compile(o->ss);
  o->error = o->ss->error;
  return r;
}

/* defines a new command for SealScript. Command's name is placed in `newcmd'
in UPPER CASE characters. cmdproc is the procedure that will be called when
this command is executed. Parameters are passed in arg[] array and the count
of them in args. Example: ss->define_command(ss, "PLAY", &cmd_play); */
void sscript_define_command(p_sscript o, l_text newcmd, void (*cmdproc)(l_int args, l_text arg[]))
{
  p_usercmd cl = (p_usercmd) o->ss->commands;
  p_usercmd p;
  p_usercmd n;

  if (!cmdproc) return;

  /* find last command */
  if (!cl) {
    cl = (p_usercmd) malloc(sizeof(t_usercmd));
    cl->name = strdup(newcmd);
    cl->mempos = (l_int) cmdproc;
    cl->next = NULL;
    (p_usercmd) o->ss->commands = cl;
    DEBUG_printf("SS: %i %s\n", cl->mempos, newcmd);
    return;
  };
  p = cl;
  if (p)
  do {
    if (p->next) p = p->next;
  } while (p->next);
  n = (p_usercmd) malloc(sizeof(t_usercmd));
  n->name = strdup(newcmd);
  strupr(n->name);
  n->mempos = (l_int) cmdproc;
  n->next = NULL;
  p->next = n;
  DEBUG_printf("SS: %i %s\n", n->mempos, newcmd);
}

/* executes the script */
void sscript_execute(p_sscript o)
{
  o->ss->execute(o->ss);
  o->error = o->ss->error;
}

/* t_sscript_ functions - not documented */
int next_arg(l_int head, l_text ins, l_int ch, l_text arg)
{
  l_int i;
  l_int st = 0;

  strcpy(arg, "");
  i = -1;
  if (head < strlen(ins)) {if (ins[head] != ch) {
    do
      {
        head++;
        if (ins[head-1] == '"') {
          i++;
          arg[i] = '"';
          st = 1-st;
        } else
        if (st == 0) {
         if (ins[head-1] != ch) {
          i++;
          arg[i] = ins[head-1];
         } else {
          //head = strlen(ins);
          break;
         }
        }
        if (st == 1 && ins[head-1] != '"') {
          i++;
          arg[i] = ins[head-1];
        };
      }
    while (head < strlen(ins));
    i++;
    arg[i] = 0;
  };};
  return head;
}

p_sscript_var find_var(p_sscript_var first, l_text name)
{
  p_sscript_var p = first;
  if (!p) return NULL;
  do {
    if (!stricmp(p->name, name)) return p;

    p = p->next;
  } while(p);
  return NULL;
}

void cut_spaces(l_text s)
{
  l_int fl = strlen(s)+1;
  l_int i;

  if (strlen(s) > 0)
  if (s[0] == 32 || s[0] == 8) do {
    for (i=0;i<strlen(s)-1;i++) s[i]=s[i+1];
    s[strlen(s)-1] = 0;
    fl--;
  } while ((s[0] == 32 || s[0] == 8) && strcmp(s, ""));

  if (strlen(s) > 0)
  if (s[strlen(s)-1] == 32 || s[strlen(s)-1] == 8) do {
    s[strlen(s)-1] = 0;
    fl--;
  } while ((s[strlen(s)-1] == 32 || s[strlen(s)-1] == 8) && strcmp(s, ""));
}

void set_var(p_sscript_var first, l_text name, l_text value)
{
  p_sscript_var v;
  p_sscript_var p;
  if (!first) return;

  v = find_var(first, name);
  if (v) {
    if (!strcmp(value, "")) {  /* if value is nothing then delete var */
      v->prev->next = v->next;
      v->next->prev = v->prev;
      free(v->name);
      free(v->value);
      free(v);
      return;
    };
    free(v->value);
    v->value = strdup(value);
    return;
  };
  /* add new variable */
  p = first;
  do {
    if (p->next) p = p->next; else break;
  } while(1);

  v = (p_sscript_var) malloc(sizeof(t_sscript_var));
  v->name = strdup(name);
  strupr(v->name);
  v->value = strdup(value);
  v->prev = p;
  v->next = NULL;
  p->next = v;
}

l_text get_var(p_sscript_var first, l_text name)
{
  p_sscript_var v;
  if (!first || !name || !strcmp(name, "")) return "(nothing)";

  v = find_var(first, name);
  if (!v) return "(not found)";

  return v->value;
}

l_int set_pvar(l_text var, l_text data, l_int s)
{
  l_int i;
  var = strdup(var);
  cut_spaces(var);

  if (var[0] == '"' && var[strlen(var)-1] == '"') {
    data[s] = 1;  /* alphanumeric (string) value */
    s++;
    data[s] = strlen(var)-2;
    s++;
    for (i=1;i<strlen(var)-1;i++) {
      data[s] = var[i];
      s++;
    };
    free(var);
    return s;
  };
  data[s] = 2; /* integer */
  for (i=0;i<strlen(var);i++)
    if (var[i] != '0' &&
        var[i] != '1' &&
        var[i] != '2' &&
        var[i] != '3' &&
        var[i] != '4' &&
        var[i] != '5' &&
        var[i] != '6' &&
        var[i] != '7' &&
        var[i] != '8' &&
        var[i] != '9') data[s] = 4; /* unknown */
  if (data[s] == 2) {
    s++;
    data[s] = strlen(var);
    s++;
    for (i=0;i<strlen(var);i++) {
      data[s] = var[i];
      s++;
    };
    free(var);
    return s;
  };
  var = strupr(var);
  s++;
  data[s] = strlen(var);
  s++;
  for (i=0;i<strlen(var);i++) {
    data[s] = var[i];
    s++;
  };
  free(var);

  return s;
}

l_text get_pvar(p_sscript_var first, l_text data, l_int s, l_int *ns)
{
  l_text arg;
  l_int i, c, vtype;

  vtype = data[s];s++;
  c = data[s];s++;
  arg = (l_text) malloc(c+1);
  for (i=0;i<c;i++) {
    arg[i] = data[s];
    s++;
  };
  arg[c] = 0;

  if (vtype == 4) { /* variable */
    l_text tg = strdup(get_var(first, arg));
    free(arg);
    arg = tg;
  };

  *ns = s;

  return arg;
}

l_int compile_cmd(p_sscript_ o, p_labelinf li, l_bool addlabs, l_text cmd, l_text data, l_int hd, l_int *end_info)
{
  l_text arg = (l_text) malloc(16385);
  l_text com;
  l_text s;
  l_text s1;
  l_int head = 0;
  l_int i, err = SSE_NOTHING, fhd = hd;
  short hdd;

  data[hd]='C';hd++;
  data[hd]='C';hd++;

  head = next_arg(head, cmd, ' ', arg);
  com = strdup(arg);
  strupr(com);

  if (!strcmp(com, "SET")) {
    head = next_arg(head, cmd, ',', arg);
    if (head < strlen(cmd)) {
      data[hd]=CMD_SET;
      hd++;
      s = strdup(arg);
      strupr(s);
      cut_spaces(s);
      data[hd]=strlen(s);
      hd++;
      for (i=0;i<strlen(s);i++) {
        data[hd] = s[i];
        hd++;
      };
      head = next_arg(head, cmd, ',', arg);
      hd = set_pvar(arg, data, hd);
      free(s);
    } else {
      err = SSE_NOT_ENOUGH_ARGS;
      hd -= 2;
    };
  } else
  if (!strcmp(com, "PRINT") || cmd[0] == '=') {
    data[hd] = CMD_PRINT;
    hd++;
    if (cmd[0] == '=') head = 1;
    while (head < strlen(cmd)) {
      head = next_arg(head, cmd, ',', arg);
      hd = set_pvar(arg, data, hd);
    };
  } else
  if (!strcmp(com, "IF")) {
    head = next_arg(head, cmd, ' ', arg); /* value1 */
    s = strdup(arg);
    head = next_arg(head, cmd, ' ', arg); /* =, <> */
    s1 = strdup(arg);
    cut_spaces(s1);
    if (head < strlen(cmd) && (!strcmp(s1, "=") || !strcmp(s1, "<>"))) {
      head = next_arg(head, cmd, ' ', arg); /* value2 */
      data[hd]=CMD_IF;
      hd++;
      if (!strcmp(s1, "=")) data[hd] = 1; else
      if (!strcmp(s1, "<>")) data[hd] = 2;
      hd++;
      hd = set_pvar(s, data, hd);
      hd = set_pvar(arg, data, hd);
      *end_info = hd;
      hd += 4;
    } else {
      err = SSE_NOT_ENOUGH_ARGS;
      if (head < strlen(cmd)) err = SSE_SYNTAX_ERROR;
      hd -= 2;
    };
    free(s1);
    free(s);
  } else
  if (!strcmp(com, "END")) {
    l_int ihd;
    data[hd]=CMD_END;
    hd++;
    ihd = hd;
    memcpy(o->c+*end_info, &ihd, 4);
    *end_info = -1;
  } else
  if (!strcmp(com, "ADD") || !strcmp(com, "SUB") || !strcmp(com, "MUL") || !strcmp(com, "DIV")) {
    head = next_arg(head, cmd, ',', arg);
    if (head < strlen(cmd)) {
      if (!strcmp(com, "ADD")) data[hd]=CMD_ADD;
      if (!strcmp(com, "SUB")) data[hd]=CMD_SUB;
      if (!strcmp(com, "MUL")) data[hd]=CMD_MUL;
      if (!strcmp(com, "DIV")) data[hd]=CMD_DIV;
      hd++;
      s = strdup(arg);
      strupr(s);
      cut_spaces(s);
      data[hd]=strlen(s);
      hd++;
      for (i=0;i<strlen(s);i++) {
        data[hd] = s[i];
        hd++;
      };
      head = next_arg(head, cmd, ',', arg);
      hd = set_pvar(arg, data, hd);
      free(s);
    } else {
      err = SSE_NOT_ENOUGH_ARGS;
      hd -= 2;
    };
  } else
  if (!strcmp(com, "OPEN")) {
    head = next_arg(head, cmd, ' ', arg); /* filename */
    s = strdup(arg);
    cut_spaces(s);
    head = next_arg(head, cmd, ' ', arg); /* FOR */
    if (strcmp(arg, "FOR")) err = SSE_SYNTAX_ERROR;
    head = next_arg(head, cmd, ' ', arg); /* INPUT, OUTPUT, APPEND */
    s1 = strdup(arg);
    s1 = strupr(s1);
    cut_spaces(s1);
    head = next_arg(head, cmd, ' ', arg); /* AS */
    if (head < strlen(cmd) && (!strcmp(s1, "INPUT") || !strcmp(s1, "OUTPUT") || !strcmp(s1, "APPEND"))) {
      head = next_arg(head, cmd, ' ', arg); /* number */
      data[hd] = CMD_OPEN;
      hd++;
      hd = set_pvar(s, data, hd);
      data[hd] = 255;
      if (!strcmp(s1, "INPUT")) data[hd] = 1;
      if (!strcmp(s1, "OUTPUT")) data[hd] = 2;
      if (!strcmp(s1, "APPEND")) data[hd] = 3;
      hd++;
      s = strdup(arg);
      strupr(s);
      cut_spaces(s);
      data[hd]=strlen(s);
      hd++;
      for (i=0;i<strlen(s);i++) {
        data[hd] = s[i];
        hd++;
      };
    } else {
      err = SSE_NOT_ENOUGH_ARGS;
      if (head < strlen(cmd)) err = SSE_SYNTAX_ERROR;
      hd -= 2;
    };
    free(s1);
    free(s);
  } else
  if (!strcmp(com, "WRITE")) {
    data[hd]=CMD_WRITE;
    hd++;
    while (head < strlen(cmd)) {
      head = next_arg(head, cmd, ',', arg);
      hd = set_pvar(arg, data, hd);
    };
  } else
  if (!strcmp(com, "READ")) {
    head = next_arg(head, cmd, ',', arg);
    if (head < strlen(cmd)) {
      data[hd]=CMD_READ;
      hd++;
      hd = set_pvar(arg, data, hd);
      head = next_arg(head, cmd, ',', arg);
      strupr(arg);
      cut_spaces(arg);
      data[hd] = strlen(arg);
      hd++;
      for (i=0;i<strlen(arg);i++) {
        data[hd] = arg[i];
        hd++;
      };
    } else {
      err = SSE_NOT_ENOUGH_ARGS;
      hd -= 2;
    };
  } else
  if (!strcmp(com, "CLOSE")) {
    if (head < strlen(cmd)) {
      data[hd]=CMD_CLOSE;
      hd++;
      head = next_arg(head, cmd, ',', arg);
      hd = set_pvar(arg, data, hd);
    } else {
      err = SSE_NOT_ENOUGH_ARGS;
      hd -= 2;
    };
  } else
  if (!strcmp(com, "ERASE")) {
    if (head < strlen(cmd)) {
      data[hd]=CMD_ERASE;
      hd++;
      head = next_arg(head, cmd, ',', arg);
      hd = set_pvar(arg, data, hd);
    } else {
      err = SSE_NOT_ENOUGH_ARGS;
      hd -= 2;
    };
  } else
  if (!strcmp(com, "BEEP")) {
    data[hd]=CMD_BEEP;
    hd++;
  } else
  if (!strcmp(com, "STOP")) {
    data[hd]=CMD_STOP;
    hd++;
  } else
  if (!strcmp(com, "GOTO")) {
    p_labelinf p=NULL;
    l_int togo = 0;
    if (head < strlen(cmd)) {
      data[hd]=CMD_GOTO;
      hd++;
      head = next_arg(head, cmd, ',', arg);
      if (!addlabs) {
        p = li;
        do {
          if (!strcmp(p->name, arg)) {
            togo = p->pos;
            break;
          };
          p = p->next;
        } while (p);
      };
      memcpy(data+hd, &togo, 4);
      hd += 4;
      if (!p) err = SSE_LABEL_NOT_FOUND;
    } else {
      err = SSE_NOT_ENOUGH_ARGS;
      hd -= 2;
    };
  } else
  if (!strcmp(com, "TIME")) {
    if (head < strlen(cmd)) {
      data[hd]=CMD_TIME;
      hd++;
      head = next_arg(head, cmd, ',', arg);
      strupr(arg);
      cut_spaces(arg);
      data[hd] = strlen(arg);
      hd++;
      for (i=0;i<strlen(arg);i++) {
        data[hd] = arg[i];
        hd++;
      };
    } else {
      err = SSE_NOT_ENOUGH_ARGS;
      hd -= 2;
    };
  } else
  if (!strcmp(com, "MULTITASK")) {
    data[hd]=CMD_MULTITASK;
    hd++;
    head = next_arg(head, cmd, ',', arg);
    strupr(arg);
    if (!strcmp(arg, "OFF")) data[hd] = 1; else data[hd] = 2;
    hd++;
  } else
  if (com[strlen(com)-1] == ':' && addlabs) { /* Label */
    p_labelinf p;
    p_labelinf l;

    hd -= 2;
    com[strlen(com)-1] = 0;
    p = (p_labelinf) malloc(sizeof(t_labelinf));
    p->name = strdup(com);
    p->pos = hd;
    p->next = NULL;
    l = li;
    if (l->next)
    do {
      l = l->next;
    } while (l->next);
    l->next = p;
  } else
  {
    p_usercmd cl = (p_usercmd) o->commands;
    if (!cl) {
      hd -= 2;
    } else {
      p_usercmd p = cl;
      l_bool found = FALSE;

      do {
        if (!strcmp(p->name, com)) {
          l_int ohead;
          l_int args = 0;
          data[hd] = CMD_DEFINED;
          hd++;
          memcpy(data+hd, &p->mempos, 4);
          hd += 4;
          ohead = head;
          while (head < strlen(cmd)) {
            head = next_arg(head, cmd, ',', arg);
            args++;
          };
          memcpy(data+hd, &args, 4);
          hd += 4;
          head = ohead;
          while (head < strlen(cmd)) {
            head = next_arg(head, cmd, ',', arg);
            hd = set_pvar(arg, data, hd);
          };
          found = TRUE;
          break;
        };
        p = p->next;
      } while (p);
      if (!found) hd -= 2;
    };
  };

  o->error.code = err;
  free(com);
  free(arg);
  if (hd != fhd) {
    hdd = (int) hd-fhd-2;
    memcpy(data+fhd, &hdd, 2);
  };
  return hd;
}

l_bool sscript__compile(p_sscript_ o)
{
  l_int i, cmd_c, codelen = strlen(o->code);
/* UNUSED
  l_int tofree, hdr, j;
*/
  l_char cmd[16385];
/* UNUSED
  l_text d;
*/
  l_int end_info;
  l_int ei[2000];
  l_int ei_head = 0;
  l_bool in_str = FALSE;
  p_labelinf li = (p_labelinf) malloc(sizeof(t_labelinf));

  li->name = strdup("PROGRAM_START");
  li->pos = 0;
  li->next = NULL;

  if (o->c) free(o->c);
  o->c = (l_text) malloc(512000);
  o->csize = 0;

  cmd_c = 0;
  ei[0] = codelen;
  o->error.line = 0;
  for (i=0;i<codelen;i++) {
    if (o->code[i] == '\n' || (o->code[i] == '|' && !in_str)) {
      cmd[cmd_c] = 0;

      cut_spaces(cmd);

      if (o->code[i] == '\n') o->error.line++;

      if (strcmp(cmd, "")) {
        o->csize = compile_cmd(o, li, TRUE, cmd, o->c, o->csize, &end_info);
        if (end_info == -1) {
          ei_head--;
          end_info = ei[ei_head];
        } else
        if (end_info != ei[ei_head]) {
          ei_head++;
          ei[ei_head] = end_info;
        };
        if (o->error.code != SSE_NOTHING) {
          free(o->c);
          do {
            p_labelinf next = li->next;
            free(li);
            li = next;
          } while (li);
          return FALSE;
        };
      };

      in_str = FALSE;

      cmd[0] = 0;
      cmd_c = 0;
    } else {
      if (o->code[i] == '"') in_str = !in_str;
      cmd[cmd_c] = o->code[i];
      cmd_c++;
    };
  };
  o->csize = 0;

  in_str = FALSE;

  cmd_c = 0;
  ei[0] = codelen;
  for (i=0;i<codelen;i++) {
    if (o->code[i] == '\n' || (o->code[i] == '|' && !in_str)) {
      cmd[cmd_c] = 0;

      cut_spaces(cmd);

      if (strcmp(cmd, "") && (cmd[0] != 39)) {
        o->csize = compile_cmd(o, li, FALSE, cmd, o->c, o->csize, &end_info);
        if (end_info == -1) {
          ei_head--;
          end_info = ei[ei_head];
        } else
        if (end_info != ei[ei_head]) {
          ei_head++;
          ei[ei_head] = end_info;
        };
        if (o->error.code != SSE_NOTHING) {
          free(o->c);
          do {
            p_labelinf next = li->next;
            free(li);
            li = next;
          } while (li);
          return FALSE;
        };
      };

      in_str = FALSE;

      cmd[0] = 0;
      cmd_c = 0;
    } else {
      if (o->code[i] == '"') in_str = !in_str;
      cmd[cmd_c] = o->code[i];
      cmd_c++;
    };
  };
  o->c = (l_text) realloc(o->c, o->csize+1);
  do {
    p_labelinf next = li->next;
    free(li);
    li = next;
  } while (li);

  return TRUE;
}

l_int execute_cmd(p_sscript_ o, l_int h)
{
  l_int i, c;
/* UNUSED 
  l_int j;
*/
  l_char sz_[2];
  short sz;
  l_text arg;

  sz_[0] = o->c[h];h++;
  sz_[1] = o->c[h];h++;

  memcpy(&sz, &sz_, 2);

  if (o->c[h] == CMD_SET) {
    l_text arg2;
    h++;
    c = o->c[h];h++;
    arg = (l_text) malloc(c+1);
    for (i=0;i<c;i++) {
      arg[i] = o->c[h];
      h++;
    };
    arg[c] = 0;
    arg2 = get_pvar(o->vars, o->c, h, &h);
    set_var(o->vars, arg, arg2);
    free(arg2);
    free(arg);
  } else
  if (o->c[h] == CMD_PRINT) {
    l_text msg = (l_text) malloc(1024);
    i = h;
    h++;
    strcpy(msg, "");
    if (h-i < sz)
    do {
      arg = get_pvar(o->vars, o->c, h, &h);
      strcat(msg, arg);
      free(arg);
    } while (h-i < sz);
    msgbox(MW_INFO, MB_OK, "%s", msg);
    free(msg);
  } else
  if (o->c[h] == CMD_IF) {
    l_int method, pos;
    l_text a2;
    l_bool doit;
    h++;
    method = o->c[h];
    h++;
    arg = get_pvar(o->vars, o->c, h, &h);
    a2 = get_pvar(o->vars, o->c, h, &h);
    memcpy(&pos, o->c+h, 4);
    h += 4;
    doit = (!strcmp(arg, a2));
    if (method == 2) doit = (!doit);
    if (!doit) h = pos;
  } else
  if (o->c[h] == CMD_END) {
    h++;
  } else
  if (o->c[h] == CMD_ADD || o->c[h] == CMD_SUB || o->c[h] == CMD_MUL || o->c[h] == CMD_DIV) {
    l_text arg2;
    l_int cm = o->c[h];
    l_int ii, ii2;
    h++;
    c = o->c[h];h++;
    arg = (l_text) malloc(c+1);
    for (i=0;i<c;i++) {
      arg[i] = o->c[h];
      h++;
    };
    arg[c] = 0;
    arg2 = get_pvar(o->vars, o->c, h, &h);
    ii = atoi(get_var(o->vars, arg));
    ii2 = atoi(arg2);
    switch (cm) {
      case CMD_ADD: ii += ii2; break;
      case CMD_SUB: ii -= ii2; break;
      case CMD_MUL: ii *= ii2; break;
      case CMD_DIV: ii /= ii2; break;
    };
    free(arg2);
    arg2 = (l_text) malloc(15);
    itoa(ii, arg2, 10);
    set_var(o->vars, arg, arg2);
    free(arg2);
    free(arg);
  } else
  if (o->c[h] == CMD_OPEN) {
    l_int ot;
    l_text fname;
    FILE *f;
    l_int c;
    p_fileinf fi;

    h++;
    fname = get_pvar(o->vars, o->c, h, &h);
    ot = o->c[h];
    h++;
    if (ot == 2) {
      f = fopen(fname, "wt");
      fclose(f);
    };
    if (ot == 3) {
      f = fopen(fname, "at");
      if (!f) fopen(fname, "wt");
      fclose(f);
    };

    c = o->c[h];h++;
    arg = (l_text) malloc(c+1);
    for (i=0;i<c;i++) {
      arg[i] = o->c[h];
      h++;
    };
    arg[c] = 0;

    fi = (p_fileinf) malloc(sizeof(t_fileinf));
    fi->name = strdup(fname);
    fi->mode = ot;
    fi->pos = 0;
    free(fname);
    fname = (l_text) malloc(33);
    itoa((int) fi, fname, 10);
    set_var(o->vars, arg, fname);

    free(fname);
    free(arg);
  } else
  if (o->c[h] == CMD_WRITE) {
    FILE *f;
    l_text fname;
    l_int mode = 0;
    l_int hi = h;
    p_fileinf fi;

    h++;
    fname = get_pvar(o->vars, o->c, h, &h);
    fi = (p_fileinf) atoi(fname);
    free(fname);
    fname = fi->name;
    mode = fi->mode;
    if (mode > 1) {
      f = fopen(fname, "at");
      if (!f) f = fopen(fname, "wt");
      if (h-hi < sz)
      do {
        arg = get_pvar(o->vars, o->c, h, &h);
        fputs(arg, f);
        free(arg);
      } while (h-hi < sz);
      fputs("\n", f);
      fclose(f);
    } else {
      if (h-hi < sz)
      do {
        arg = get_pvar(o->vars, o->c, h, &h);
        free(arg);
      } while (h-hi < sz);
    };
  } else
  if (o->c[h] == CMD_READ) {
    FILE *f;
    l_text fname;
    l_int mode = 0, c;
    p_fileinf fi;
    h++;

    fname = get_pvar(o->vars, o->c, h, &h);
    fi = (p_fileinf) atoi(fname);
    free(fname);
    fname = fi->name;
    mode = fi->mode;

    c = o->c[h];h++;
    arg = (l_text) malloc(c+1);
    for (i=0;i<c;i++) {
      arg[i] = o->c[h];
      h++;
    };
    arg[c] = 0;
    if (mode == 1) {
      f = fopen(fname, "rt");
      if (f) {
        l_text s = (l_text) malloc(16385);

        for (i=1;i<=fi->pos;i++) fgets(s, 16384, f);
        fi->pos++;
        fgets(s, 16384, f);
        s[strlen(s)-1] = 0;
        set_var(o->vars, arg, s);
        fclose(f);
        free(s);
        fi->pos++;
      }
    };
    free(arg);
  } else
  if (o->c[h] == CMD_CLOSE) {
    l_text fname;
    p_fileinf fi;
    h++;

    fname = get_pvar(o->vars, o->c, h, &h);
    fi = (p_fileinf) atoi(fname);
    free(fname);
    free(fi->name);
    free(fi);
  } else
  if (o->c[h] == CMD_ERASE) {
    l_text fname;
    h++;

    fname = get_pvar(o->vars, o->c, h, &h);
    remove(fname);
  } else
  if (o->c[h] == CMD_BEEP) {
    h++;
    sound(1000);delay(30);nosound();
  } else
  if (o->c[h] == CMD_STOP) {
    h = o->csize;
  } else
  if (o->c[h] == CMD_GOTO) {
    l_int togo;
    h++;
    memcpy(&togo, o->c+h, 4);
    h = togo;
  } else
  if (o->c[h] == CMD_TIME) {
    l_int c;
    struct time t;
    l_text s;
    l_text s1;
    h++;

    c = o->c[h];h++;
    arg = (l_text) malloc(c+1);
    for (i=0;i<c;i++) {
      arg[i] = o->c[h];
      h++;
    };
    arg[c] = 0;

    gettime(&t);
    s = (l_text) malloc(9);
    s1 = (l_text) malloc(3);
    itoa(t.ti_hour, s, 10);
    strcat(s, ":");
    itoa(t.ti_min, s1, 10);
    strcat(s, s1);
    strcat(s, ":");
    itoa(t.ti_sec, s1, 10);
    strcat(s, s1);
    free(s1);
    set_var(o->vars, arg, s);
    free(s);
    free(arg);
  } else
  if (o->c[h] == CMD_MULTITASK) {
    h++;
    if (o->c[h] == 1) o->multitask = FALSE; else o->multitask = TRUE;
    h++;
  } else
  if (o->c[h] == CMD_DEFINED) {
    l_int mempos;
    l_int args;
    l_text arg[1024];
    void (*cmdproc)(l_int args, l_text arg[]);

    h++;
    memcpy(&mempos, o->c+h, 4);
    h += 4;
    memcpy(&args, o->c+h, 4);
    h += 4;
    for (i=0;i<args;i++) {
      arg[i] = get_pvar(o->vars, o->c, h, &h);
    };

    (l_int) cmdproc = mempos;
    cmdproc(args, arg);
    for (i=0;i<args;i++) {
      free(arg[i]);
    };
  } else
  {};

  return h;
}

void sscript__execute(p_sscript_ o)
{
  l_int head = 0;
  p_event event = (p_event) malloc(sizeof(t_event));

  do {
    head = execute_cmd(o, head);

    if (head >= o->csize) break;

    if (o->multitask) {
      OBJECT(desktop)->get_event(OBJECT(desktop), event);
      OBJECT(desktop)->translate_event(OBJECT(desktop), event);
    };
  } while (1);

  free(event);
}

p_sscript_ _sscript__init(p_sscript_ o)
{
  if (!o) return NULL;
  clear_type(o, sizeof(t_sscript));
  obj_init(&o->obclass);

  o->obclass.tag = TAG_OBJECT;

  o->multitask = TRUE;
  o->compile = &sscript__compile;
  o->execute = &sscript__execute;

  o->vars = (p_sscript_var) malloc(sizeof(t_sscript_var));
  o->vars->name = strdup("VERSION");
  o->vars->value = strdup("SealScript Version 0.1 - SealScript Library Version 0.1");
  o->vars->next = NULL;
  o->vars->prev = NULL;

  o->commands = NULL;

  return o;
}

#include "sscript.exp"

lib_begin(void)
{
  if (ap_process == AP_ALLOC)
  {
    AP_EXPORTLIB();
  } else
  if (ap_process == AP_INIT)
  {
    AP_SETNUMOFCALLS(1);
  };
} lib_end;

SetInfoAppName("SealScript");
SetInfoDescription("SealScript");
SetInfoCopyright("Copyright (c) Kostas Michalopoulos 2001");
SetInfoManufacturer("Kostas Michalopoulos");
