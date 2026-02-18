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

/* Revision History as of 09/01/2002:
 *
 * 01/12/2001: First release (with Seal 2 Lambda).
 *
 * 09/01/2002: Changed system menu icon to "sol.ico"
 *             Made the "Help" menu item open the helpfile properly.
 *             Changed background color to "color_3d_face" and
 *             marked end hole with "E". Minor color changes.  (Tobias J)
 *
 * 03/04/2002: Better skin support added.  (Tobias J)
 *
 * 02/05/2002: Reformatted the code to match the rest of Seal.
 *             Fixed some skinning problems with the menu.
 *             Fixed problem with coordinates when selecting a peg.
 *             Replaced old About box with "seal2_app_about()".  (Tobias J)
 *
 * 02/06/2002: Updated icon path (orudge)
 */



#include <seal.h>
#include <app.h>
#include <view.h>
#include <time.h>
#include <stdlib.h>
#include <menus.h>
#include <helpsys.h>


// #define TXT_SOL_ABOUT "\"Peg Solitaire\"\n\nVersion 1.2\n\n\n(c)2001 Tobias Johansson\n(tobbe_snodd@hotmail.com)\n"

#define MSG_GAMERESTART 100001
#define MSG_GAMEHELP    100002
#define MSG_NEWORIGINAL 100003
#define MSG_NEWENGLISH  100004
#define MSG_NEWFRENCH   100005


SetInfoAppName("Peg Solitaire");
SetInfoDesciption("A single-player board game");
SetInfoCopyright("Copyright (c) 1999-2002 SEAL Developers. All Rights Reserved.");
SetInfoManufacturer("Tobias Johansson");


p_appwin mainWin;
p_view gameArea;
p_menuview gameMenu;

// BITMAP  *srcPic;
l_int   pegs[8][8];
t_point curSel;
t_point endPos;
l_int   pegsize = 24;
t_point coords[50];



l_int check_if_won (void) 
{
   l_int c = 0;
   l_int x;
   l_int y;

   for (y=1 ; y<8 ; y++)
   {
      for (x=1 ; x<8 ; x++) 
         c += pegs[x][y];
   }

   if (c == -15 && pegs[endPos.x][endPos.y] == 1) 
      return 1;

   return 0;
}



l_int move_peg (t_point from, t_point to) 
{
   if (to.x > 0 && to.x == from.x - 2 && to.y == from.y) 
   {
      if (pegs[from.x - 1][from.y] == 1 && pegs[to.x][to.y] == 0) 
      {
         pegs[to.x][to.y] = 1;
         pegs[from.x][from.y] = 0;
         pegs[from.x - 1][from.y] = 0;
         return 0;
      }
   }
   else 

   if (to.x < 8 && to.x == from.x + 2 && to.y == from.y) 
   {
      if (pegs[from.x + 1][from.y] == 1 && pegs[to.x][to.y] == 0) 
      {
         pegs[to.x][to.y] = 1;
         pegs[from.x][from.y] = 0;
         pegs[from.x + 1][from.y] = 0;
         return 0;
      }
   }
   else 

   if (to.y > 0 && to.y == from.y - 2 && to.x == from.x) 
   {
      if (pegs[from.x][from.y - 1] == 1 && pegs[to.x][to.y] == 0) 
      {
         pegs[to.x][to.y] = 1;
         pegs[from.x][from.y] = 0;
         pegs[from.x][from.y - 1] = 0;
         return 0;
      }
   }
   else 

   if (to.y < 8 && to.y == from.y + 2 && to.x == from.x) 
   {
      if (pegs[from.x][from.y + 1] == 1 && pegs[to.x][to.y] == 0) 
      {
         pegs[to.x][to.y] = 1;
         pegs[from.x][from.y] = 0;
         pegs[from.x][from.y + 1] = 0;
         return 0;
      }
   }

   return 1;
}



void draw_bricks (p_view v) 
{
   l_int   x;
   l_int   y;
   t_rect  rc;
   t_point p;

   l_int  pegmod = (pegsize/2) - pegsize;
   t_rect r = v->get_local_extent(v);

   BITMAP *out = v->begin_paint(v, &p, r);

   if (out) 
   {
      for (y=1 ; y<8 ; y++) 
      {
         for (x=1 ; x<8 ; x++) 
         {
            switch (pegs[x][y]) 
            {
               case 0:          // No peg.

                  circlefill(out, p.x + (x * (pegsize + 4)) + pegmod - 1,
                                  p.y + (y * (pegsize + 4)) + pegmod + 3,
                                  (pegsize/2), 
                                  COLOR(CO_BLACK));

                  circlefill(out, p.x + (x * (pegsize + 4)) + pegmod,
                                  p.y + (y * (pegsize + 4)) + pegmod + 4,
                                  (pegsize/2), 
                                  COLOR(CO_DARKGRAY));

                  if (endPos.x == x && endPos.y == y) 
                  {
                     textout_printf(out, v->font, -1,
                                    p.x + (x * (pegsize + 4)) - 22, 
                                    p.y + (y * (pegsize + 4)) - 14,
                                    p.x + (x * (pegsize + 4)),
                                    p.y + (y * (pegsize + 4)),
                                    TX_ALIGN_CENTER, 
                                    color_3d_face,
                                    TX_NOCOLOR, 
                                    0, 
                                    "E");
                  }
                  break;


               case 1:        // Peg.

                  circlefill(out, p.x + (x * (pegsize + 4)) + pegmod ,
                                  p.y + (y * (pegsize + 4)) + pegmod + 4,
                                  (pegsize/2), 
                                  (curSel.x == x && curSel.y == y)?COLOR(CO_DARKGRAY):COLOR(CO_BLACK));

                  circlefill(out, p.x + (x * (pegsize + 4)) + pegmod - 1,
                                  p.y + (y * (pegsize + 4)) + pegmod + 3,
                                  (pegsize/2), 
                                  (curSel.x == x && curSel.y == y)?COLOR(CO_LIGHTGREEN):COLOR(CO_GREEN));

                  break;
            }
         }
      }
   }

   v->end_of_paint(v, r);
}



void init_pegs (l_int type) 
{
   l_int x;
   l_int y;

   switch (type) 
   {
      case 1:          // The original solitaire

         for (x=1 ; x<8 ; x++) 
         {
            for (y=1 ; y<8 ; y++) 
               pegs[x][y] = 1;  
         }

         pegs[4][4] = 0;
         endPos.x = 4;
         endPos.y = 4;

         break;


      case 2:         // French solitaire

         for (x=1 ; x<8 ; x++) 
         {
            for (y=1 ; y<8 ; y++)
               pegs[x][y] = 0;  
         }

         for (x=2 ; x<7 ; x++) 
            pegs[x][4] = 1;

         for (y=2 ; y<7 ; y++) 
            pegs[4][y] = 1;

         endPos.x = 4;
         endPos.y = 4;
         break;


      case 3:      // English solitaire

         for (x=1 ; x<8 ; x++) 
         {
            for (y=1 ; y<8 ; y++) 
               pegs[x][y] = 0;  
         }

         for (x=2 ; x<7 ; x++) 
            pegs[x][3] = 1;

         for (y=1 ; y<8 ; y++) 
            pegs[4][y] = 1;

         pegs[3][6] = 1; pegs[3][7] = 1;
         pegs[5][6] = 1; pegs[5][7] = 1;

         endPos.x = 4;
         endPos.y = 3;
         break;
   }

   // Set the "corner" holes to -1 to 
   // prevent them from being drawn.
   pegs[1][1] = -1; pegs[2][1] = -1;
   pegs[1][2] = -1; pegs[2][2] = -1;
   pegs[6][1] = -1; pegs[7][1] = -1;
   pegs[6][2] = -1; pegs[7][2] = -1;
   pegs[1][6] = -1; pegs[1][7] = -1;
   pegs[2][6] = -1; pegs[2][7] = -1;
   pegs[6][6] = -1; pegs[6][7] = -1;
   pegs[7][6] = -1; pegs[7][7] = -1;

   draw_bricks(gameArea);
}



static void game_translate_event (p_object o, p_event event) 
{
   l_int   retval;
   t_rect  r;
   t_point p;
   t_point c;

   RETVIEW(o, event);

   if (event->type & EV_MOUSE) 
   {
      if (OBJECT(mouse)->state & MO_SF_MOUSELUP) 
      {
         p = VIEW(gameArea)->get_local_point(VIEW(gameArea), mouse->where);

         c.x = ((p.x-1) / (pegsize + 4)) + 1;
         c.y = ((p.y-7) / (pegsize + 4)) + 1;

         if (pegs[c.x][c.y] != -1) 
         {
            if (curSel.x == 0 && curSel.y == 0 && pegs[c.x][c.y] == 1) 
            {
               if (! check_if_won()) 
               {
                  curSel.x = c.x;
                  curSel.y = c.y;
               }
            }
            else 

            if (c.x == curSel.x && c.y == curSel.y) 
            {
               curSel.x = 0;
               curSel.y = 0;
            }
            else 
            {
               if (curSel.x && curSel.y) 
               {
                  retval = move_peg(curSel, c);
                  curSel.x = 0;
                  curSel.y = 0;
                  draw_bricks(gameArea);
                  if (check_if_won()) 
                  {
                     msgbox(MW_INFO, MB_OK, "Well done! You did it!");
                     clear_event(event);
                  }
               }
            }
         }

         draw_bricks(gameArea);
         clear_event(event);
      }
   }

   if (event->type == EV_MESSAGE) 
   {
      switch (event->message)
      {
         case MSG_NEWORIGINAL:

            init_pegs(1);
            clear_event(event);
            break;

         case MSG_NEWENGLISH: 

            init_pegs(3);
            clear_event(event);
            break;

         case MSG_NEWFRENCH:

            init_pegs(2);
            clear_event(event);
            break;

         case MSG_GAMEHELP:

            open_help("./help/sol.hlp");
            clear_event(event);
            break;

         case MSG_ABOUT: 

            // msgbox(MW_INFO, MB_OK, TXT_SOL_ABOUT);
            seal2_app_about("Peg Solitaire", load_image("/system/bmp/icons/sol.ico,32"));
            clear_event(event);
            break;

      }
   }
}



static p_menu create_game_menu (void) 
{
   p_menu menu;

   menu = new_menu(
                 new_sub_menu("Game", new_menu(
                    new_menu_item("New standard solitaire", "", 0, MSG_NEWORIGINAL, NULL,
                    new_menu_item("New English solitaire", "", 0, MSG_NEWENGLISH, NULL,
                    new_menu_item("New French solitaire", "", 0, MSG_NEWFRENCH, NULL,
                    new_menu_line(
                    new_menu_item(TXT_CLOSE, "ALT+F4", 0, MSG_CLOSE, NULL,
                    NULL)))) )),
                 new_sub_menu(TXT_HELP, new_menu(
                    new_menu_item(TXT_HELP, "", 0, MSG_GAMEHELP, NULL,
                    new_menu_line(
                    new_menu_item(TXT_ABOUT, "", 0, MSG_ABOUT, NULL,
                    NULL)))),
                 NULL))
              );
   return menu;
}



void create_window (void)
{
   l_int tb_h = 20;// = apppastitle->d.a.y + apppastitle->d.b.y + 17;       // Height of titlebar

   mainWin = appwin_init(malloc(sizeof(t_appwin)), 
                         rect_assign(100, 100, 330, 342 + tb_h), 
                         "Peg Solitaire",
                         WF_MINIMIZE|WF_ABOUT, 
                         ap_id, 
                         &game_translate_event);
   mainWin->icon16 = load_image("/system/bmp/icons/sol.ico,16");
   OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(mainWin));


   gameMenu = hormenu_init(_malloc(sizeof(t_menuview)), 
                           rect_assign(0, 1 + tb_h, 230, 20 + tb_h), 
                           create_game_menu());
   OBJECT(mainWin)->insert(OBJECT(mainWin), OBJECT(gameMenu));


   gameArea = view_init(_malloc(sizeof(t_view)),
                        rect_assign(15, 26 + tb_h, 220, 226 + tb_h));
   VIEW(gameArea)->draw = &draw_bricks;
   VIEW(gameArea)->transparent = TRUE;
   OBJECT(gameArea)->translate_event = &game_translate_event;
   OBJECT(mainWin)->insert(OBJECT(mainWin), OBJECT(gameArea));
}



app_begin (void) 
{
   if (ap_process == AP_INIT) 
   {
      AP_SETNUMOFCALLS(1);
      create_window();
      init_pegs(1);
      draw_bricks(gameArea);
   }
} app_end;

