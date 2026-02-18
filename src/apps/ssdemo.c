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
/*                        SealScript Demo                       */
/*                             v0.1                             */
/*                                                              */
/*        (c) 2001 Kostas Michalopoulos aka Bad Sector          */
/*                                                              */
/****************************************************************/

#include <allegro.h>
#include <seal.h>
#include <app.h>
#include <dialogs.h>
#include <editor.h>
#include <sscript.h>

p_appwin mainwin = NULL;
p_editor editor = NULL;
p_button go_btn = NULL;

#define MSG_RUNSCRIPT     10000

void my_very_own_sealscript_command(l_int args, l_text arg[]) /* nice name ;) */
{
   l_int i;
   msgbox(MW_INFO, MB_OK, "\n\nUser called my own command with %i args :)))\n\n", args);

   for (i = 0; i < args; i++)
      msgbox(MW_INFO, MB_OK, "Arg %i: %s", i, arg[i]);
}

static void handle_event(p_object o, p_event event)
{
   if (o->phase == PH_PREPROCESS && event->type & EV_MESSAGE)
   {
      if (event->message == MSG_RUNSCRIPT)
      {
         p_sscript ss;
         clear_event(event);

         ss = sscript_init(malloc(sizeof(t_sscript)));
         ss->set_code(ss, editor->text);
         ss->define_command(ss, "SSDEMO", &my_very_own_sealscript_command);
         free(WINDOW(mainwin)->caption);
         WINDOW(mainwin)->caption = strdup("SealScript Demo - Compiling");
         TEST_SUB_VIEWS(VIEW(mainwin), WINDOW(mainwin)->draw_title(WINDOW(mainwin)));

         if (ss->compile(ss))
         {
           free(WINDOW(mainwin)->caption);
           WINDOW(mainwin)->caption = strdup("SealScript Demo - Running");
           TEST_SUB_VIEWS(VIEW(mainwin), WINDOW(mainwin)->draw_title(WINDOW(mainwin)));
           ss->execute(ss);
         }
         else
         {
            switch (ss->error.code)
            {
               case SSE_SYNTAX_ERROR:
                  msgbox(MW_ERROR, MB_OK, "Syntax Error - line %i", ss->error.line);
                  break;

               case SSE_NOT_ENOUGH_ARGS:
                  msgbox(MW_ERROR, MB_OK, "Not enough arguments - line %i", ss->error.line);
                  break;

               case SSE_LABEL_NOT_FOUND:
                  msgbox(MW_ERROR, MB_OK, "Label not found - line %i", ss->error.line);
                  break;

               default:
                  msgbox(MW_ERROR, MB_OK, "Unknown compilation error - line %i", ss->error.line);
                  break;
            }
         }

         free(WINDOW(mainwin)->caption);
         WINDOW(mainwin)->caption = strdup("SealScript Demo");
         TEST_SUB_VIEWS(VIEW(mainwin), WINDOW(mainwin)->draw_title(WINDOW(mainwin)));
         OBJECT(ss)->done(OBJECT(ss));
         free(ss);
      }
   }
}

void app_init()
{
   t_rect r = rect_assign(0, 0, 500, 350);
   mainwin = appwin_init(malloc(sizeof(t_appwin)), r, "SealScript Demo", WF_MINIMIZE, ap_id, &handle_event);
   VIEW(mainwin)->align |= TX_ALIGN_CENTER;
   OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(mainwin));

   go_btn = button_init(malloc(sizeof(t_button)), rect_assign(184, 323, 249, 348), "Run Script", MSG_RUNSCRIPT, BF_DEFAULT);
   OBJECT(mainwin)->insert(OBJECT(mainwin), OBJECT(go_btn));

   r = rect_assign(2, 21, 498, 298);
   editor = editor_init(malloc(sizeof(t_editor)), r, 0);
   VIEW(editor)->align |= TX_ALIGN_BOTTOM+TX_ALIGN_RIGHT;
   OBJECT(mainwin)->insert(OBJECT(mainwin), OBJECT(editor));
   editor->set_text(editor, "' SealScript demonstration program
' SealScript (C) 2001 Kostas Michalopoulos aka Bad Sector
' Click the button below to compile and run this demo

' Let's set a variable...
SET a_var, \"a value\"

' Let's show a message...
PRINT \"Welcome to SealScript demonstration program\"

' ...with variable set
PRINT \"We set a variable named `a_var' to `\", a_var, \"'\"

' Let's take a decision...
IF a_var = \"a value\" | PRINT \"Yeap. a_var is equal to `a value'\" | END

' Let's make some simple mathematics...
SET x, 5
PRINT \"x = \", x
ADD x, 5
PRINT \"x = \", x
SUB x, 2
PRINT \"x = \", x
MUL x, 9
PRINT \"x = \", x
DIV x, 2
PRINT \"x = \", x

' Let's make a new file...
OPEN \"ssdemo.txt\" FOR OUTPUT AS file
' ...and write something inside
WRITE file, \"Hi doc. I'm a file!\"
' and of course close it and release used memory
CLOSE file

' Now we can erase it
ERASE \"ssdemo.txt\"

' Let's make a sound...
BEEP

' Isn't so easy, but we can make a loop...
SET i, 1
LOOP_START:
  PRINT \"Hi! I'm message number \", i
  ADD i, 1
  IF i = 6 | GOTO LOOP_END | END
  GOTO LOOP_START
LOOP_END:

' Let's see what time is it now...
TIME time
' ...and let user know
PRINT \"Current time is \", time

' We can speed up script executing by disabling
' multitasking using the MULTITASK OFF command
' but isn't recommended.

' SSDEMO.EXE has defined a new command named
' SSDEMO. This command can take as many parameters
' as you want and will pop up some message boxes.
' Now we'll call this command. Note that SSDEMO is
' only available from inside SSDEMO.EXE because
' it's defined from it.

' Let's execute the SSDEMO command...

SSDEMO \"Hello, there!\", \"I'm the second argument\"


' That's all folks!
' SealScript (C) 2001 Kostas Michalopoulos aka Bad Sector

");
}

app_begin(void)
{
   if (ap_process == AP_INIT)
   {
      AP_SETNUMOFCALLS(1);
      app_init();
   }
} app_end;

SetInfoAppName("SealScript Demo");
SetInfoDesciption("SealScript Demo");
SetInfoCopyright("Copyright (c) Kostas Michalopoulos 2001");
SetInfoManufacturer("Kostas Michalopoulos");
