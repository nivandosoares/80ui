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
/*                           sscript.h                          */
/*                      SealScript Library                      */
/*                             v0.1                             */
/*                                                              */
/*        (c) 2001 Kostas Michalopoulos aka Bad Sector          */
/*                                                              */
/****************************************************************/

#ifndef __SSCRIPT_H_INCLUDED_
#define __SSCRIPT_H_INCLUDED_

#ifdef __cplusplus
extern "C" {
#endif

/* var hooking methods */
#define VH_SET_VAR      0               /* when script wants to set a var */
#define VH_GET_VAR      1               /* when script wants to get a var */

/* error codes */
#define SSE_NOTHING             0       /* No error */
#define SSE_SYNTAX_ERROR        1       /* General syntax error */
#define SSE_NOT_ENOUGH_ARGS     2       /* Command needs more parameters */
#define SSE_LABEL_NOT_FOUND     3       /* GOTO can't find specified label */

typedef struct t_sscript *p_sscript;
typedef struct t_sscript_ *p_sscript_;

/* use t_ss_error structure to take information about last error */
typedef struct t_ss_error
{
  l_int code;
  l_int line;
  l_char reserved[128];
} t_ss_error;

/* use t_sscript object to make a new SealScript engine */
typedef struct t_sscript
{
  struct t_object obclass;

  p_sscript_ ss;
  t_ss_error error;
  l_int version;
  l_int ver_hi;
  l_int ver_lo;

  l_char reserved[1024];

  void (*set_code)(p_sscript o, l_text code);
  l_bool (*compile)(p_sscript o);
  void (*define_command)(p_sscript o, l_text newcmd, void (*cmdproc)(l_int args, l_text arg[]));

  /* if `what' is VH_GET_VAR then if you have to return a value for variable
  that it's name is stored in `name', then place this value to `value' and
  return the function with TRUE. Else you MUST return the function with FALSE.

  if `what' is VH_SET_VAR then if you have a variable that it's name is the
  same with that stored in `name' and user is allowed to set a value to this
  variable, you have to set this variable to `value' and return the function
  with TRUE. Else return the function with FALSE.

  Note: this function will be used in a next version and it's added only for
  compatibility reasons */
  l_bool (*var_hook)(p_sscript o, l_text name, l_text value, l_int what);

  void (*execute)(p_sscript o);

  l_char reservd[1024];
} t_sscript;

#define SSCRIPT(o) ((p_sscript)(o))

/* sets SealScript code to code. To use this code you have first
to compile it using compile() function. */
void sscript_set_code(p_sscript o, l_text code);

/* compiles SealScript code to p-code for faster executing. If any error
occurs returns FALSE and sets the error.error to error code */
l_bool sscript_compile(p_sscript o);

/* defines a new command for SealScript. Command's name is placed in `newcmd'
in UPPER CASE characters. cmdproc is the procedure that will be called when
this command is executed. Parameters are passed in arg[] array and the count
of them in args.

Example:

  define_command(ss, "PLAY", &cmd_play);

*/
void sscript_define_command(p_sscript o, l_text newcmd, void (*cmdproc)(l_int args, l_text arg[]));

/* executes the script */
void sscript_execute();

/* Initializes a new SealScript control */
p_sscript _sscript_init(p_sscript o);
extern p_sscript (*sscript_init)(p_sscript o);

/* Real SealScript engine objects */
/* WARNING! Do NOT use those objects. They are for internal use only.
  Use t_sscript instead... */

typedef struct t_sscript_var *p_sscript_var;
typedef struct t_sscript_var
{
  l_text name;
  l_text value;

  p_sscript_var next;
  p_sscript_var prev;
} t_sscript_var;

typedef struct t_sscript_
{
  struct t_object obclass;
  l_text code;
  l_text c;  // OCR: was unsigned l_text
  l_int csize;
  l_bool multitask;
  p_sscript_var vars;
  t_ss_error error;
  void *commands;

  l_bool (*compile)(p_sscript_ o);
  void (*execute)(p_sscript_ o);
  void (*done)(p_sscript_ o);
} t_sscript_;

/*
p_sscript_ _sscript__init(p_sscript_ o);
extern p_sscript__init(p_sscript_ o); 
do you mean this perhaps? ... (You are lost in a nasty twisty maze of _ underscores <grin>)
*/
extern p_sscript_ _sscript__init(p_sscript_ o);

#ifdef __cplusplus
}
#endif

#endif

