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

/* Revision History:
 *
 * 13/05/2002: First release by Tobias Johansson
 * 02/06/2002: Updated icon path (orudge)
 * 25/01/2003: Made it work with the new skinning system (xduffy)
 */

#include <seal.h>
#include <app.h>
#include <view.h>
#include <menus.h>
#include <helpsys.h>


#define MSG_GAMERESTART_BLACK  100001
#define MSG_GAMERESTART_WHITE 100002
#define MSG_GAMEHELP         100003
#define MSG_PLAY_FAST        100004
#define BRICKSIZE            30

p_appwin mainWin;
p_view gameArea;
p_menuview gameMenu;



l_int bricks[9][9];

l_int c_human = 1;      // Color of players. 1=black and 2=white.
l_int c_computer = 2;

l_int p_human = 2;      // Players points.
l_int p_computer = 2;


l_int do_play_fast;     // True if 'Fast play' option is enabled.



static void init_game(void)
{
   l_int x, y;
   for (y=1 ; y<=8 ; y++)
   {
      for (x=1 ; x<=8 ; x++)
         bricks[x][y] = 0;
   }
   bricks[4][4]=1;
   bricks[5][5]=1;
   bricks[4][5]=2;
   bricks[5][4]=2;
   
   p_human = 2;
   p_computer = 2;
}



void draw_bricks_txt(p_view v, l_text disptext)
{
   // Draws bricks and grid to v and displays disptext (eg. "Thinking...")

   t_point p;
   l_int x, y;
   t_rect r;
   BITMAP *out;

   r = v->size_limits(v);
   out = v->begin_paint(v, &p, r);

   if (out)
   {
      rectfill(out, p.x, p.y, p.x + 240, p.y + 240, COLOR(CO_GREEN));
      for (x=0 ; x<=8 ; x++)
      {
         line(out, p.x + (x * BRICKSIZE), p.y, p.x + (x * BRICKSIZE), p.y + 240, COLOR(CO_BLACK));
         line(out, p.x, p.y + (x * BRICKSIZE), p.x + 240, p.y + (x * BRICKSIZE), COLOR(CO_BLACK));
      }
      for (y=1 ; y<=8 ; y++)
      {
         for (x=1 ; x<=8 ; x++)
         {
            switch (bricks[x][y])
            {
               case 1:
                  circlefill(out, p.x + (x * BRICKSIZE) - (BRICKSIZE / 2), 
                                  p.y + (y * BRICKSIZE) - (BRICKSIZE / 2), 
                                  (BRICKSIZE / 2) - 3, 
                                  COLOR(CO_BLACK));
                  break;

               case 2:
                  circlefill(out, p.x + (x * BRICKSIZE) - (BRICKSIZE / 2),
                                  p.y + (y * BRICKSIZE) - (BRICKSIZE / 2), 
                                  (BRICKSIZE / 2) - 3, 
                                  COLOR(CO_WHITE));
                  break;
            }
         }
      }

      // Display disptext
      textout_printf(out, v->font, -1, p.x + 90, p.y + 225, p.x + 150, p.y + 250, TX_ALIGN_CENTER, COLOR(CO_WHITE), COLOR(CO_GREEN), 0, "%s", disptext);

      // Player text and point
      textout_printf(out, v->font, -1, p.x + 12, p.y + 245, p.x + 60, p.y + 265, TX_ALIGN_LEFT, color_3d_text, TX_NOCOLOR, 0, "Player:");
      textout_printf(out, v->font, -1, p.x + 50, p.y + 245, p.x + 82, p.y + 265, TX_ALIGN_RIGHT, color_3d_text, TX_NOCOLOR, 0, "%d", p_human);

      // Computer text and points
      textout_printf(out, v->font, -1, p.x + 152, p.y + 245, p.x + 200, p.y + 265, TX_ALIGN_LEFT, color_3d_text, TX_NOCOLOR, 0, "Computer:");
      textout_printf(out, v->font, -1, p.x + 210, p.y + 245, p.x + 240, p.y + 265, TX_ALIGN_RIGHT, color_3d_text, TX_NOCOLOR, 0, "%d", p_computer);

      // Colored dots by the text
      if (c_human == 2)
      {
         circlefill(out, p.x + 4, p.y + 250, 3, COLOR(CO_WHITE));
         circlefill(out, p.x + 144, p.y + 250, 3, COLOR(CO_BLACK));
         circle(out, p.x + 4, p.y + 250, 4, COLOR(CO_BLACK));
         circle(out, p.x + 144, p.y + 250, 4, COLOR(CO_BLACK));
      } 
      else
      {
         circlefill(out, p.x + 4, p.y + 250, 3, COLOR(CO_BLACK));
         circlefill(out, p.x + 144, p.y + 250, 3, COLOR(CO_WHITE));
         circle(out, p.x + 4, p.y + 250, 4, COLOR(CO_BLACK));
         circle(out, p.x + 144, p.y + 250, 4, COLOR(CO_BLACK));
      }
   }
   button3d(v, out, p.x, p.y, p.x + 240, p.y + 240, 1);

   v->end_of_paint(v, r);
}



void draw_bricks(p_view v)
{
   // Redraws the game area without any text

   draw_bricks_txt(v, "");
}



l_int fill_direction(l_int cx, l_int cy, l_int dx, l_int dy, l_int cur, l_int opp) 
{
   // Replace bricks in color opp with color cur. 
   // Start at (cx;cy) and continue adding dx and 
   // dy too cordinates until the current brick is
   // not in color opp.

   l_int c = 0;

   while (bricks[cx + dx][cy + dy] == opp) 
   {
      cx += dx;
      cy += dy;
      bricks[cx][cy] = cur;
      c++;
   }
   return c;
}



l_int check_direction(l_int cx, l_int cy, l_int dx, l_int dy, l_int cur, l_int opp) 
{
   // Start at (cx;cy) and if the brick cx+dx, cy+dy is
   // in the color opp, continue adding dx and dy until
   // the current coordinate doesn't contain a brick in
   // color opp. Returns the number of bricks if successful,
   // 0 otherwise.

   l_int c = 0;

   cx += dx;
   cy += dy;

   if (bricks[cx][cy] == opp) 
   {
      while (cx>0 && cx<9 && cy>0 && cy<9) 
      {
         cx += dx;
         cy += dy;
         c++;

         if (bricks[cx][cy] == cur) 
            return c;
         else 
         if (bricks[cx][cy] == 0)
            return 0;
      } // while (cx>0 && cx<9 && cy>0 && cy<9);
   }
   return 0;
}



l_int do_move(l_int cx, l_int cy, l_int cur, l_int opp, l_bool do_change) 
{
   //  Inserts a brick in color cur at (cx;cy) if do_change is TRUE and returns
   //  the number of changed bricks. Returns the number of bricks without changing
   //  anything if do_change is set to FALSE. 

   l_int changed  = 0;     // Number of changed bricks. Returned if do_change = TRUE.
   l_int possible = 0;     // Number of possible changed bricks. Returned if do_change = FALSE.
   l_int t = 0;            // Temporary variable. Stores return value from "check_direction()".

   if (cx > 2) 
   {
      // Check to the left
      t = check_direction(cx, cy, -1, 0, cur, opp);
      if (t && do_change) 
         changed += fill_direction(cx, cy, -1, 0, cur, opp);
      possible = t;

      // To the lower left
      if (cy > 2) 
      {
         t = check_direction(cx, cy, -1, -1, cur, opp);
         if (t && do_change)
            changed += fill_direction(cx, cy, -1, -1, cur, opp);
         possible += t;
      }

      // To the upper left
      if (cy < 7) 
      {
         t = check_direction(cx, cy, -1, 1, cur, opp);
         if (t && do_change)
            changed += fill_direction(cx, cy, -1, 1, cur, opp);
         possible += t;
      }
   }

   if (cx < 7) 
   {
      // Check to the right
      t = check_direction(cx, cy, 1, 0, cur, opp);
      if (t && do_change)
         changed += fill_direction(cx, cy, 1, 0, cur, opp);
      possible += t;

      // To the lower right
      if (cy > 2) 
      {
         t = check_direction(cx, cy, 1, -1, cur, opp);
         if (t && do_change) 
            changed += fill_direction(cx, cy, 1, -1, cur, opp);
         possible += t;
      }

      // To the upper right
      if (cy < 7) 
      {
         t = check_direction(cx, cy, 1, 1, cur, opp);
         if (t && do_change) 
            changed += fill_direction(cx, cy, 1, 1, cur, opp);
         possible += t;
      }
   }

   // Check upwards
   if (cy > 2) 
   {
      t = check_direction(cx, cy, 0, -1, cur, opp);
      if (t && do_change) 
         changed += fill_direction(cx, cy, 0, -1, cur, opp);
      possible += t;
   }

   // Check downwards 
   if (cy < 7) 
   {
      t = check_direction(cx, cy, 0, 1, cur, opp);
      if (t && do_change) 
         changed += fill_direction(cx, cy, 0, 1, cur, opp);
      possible += t;
   }

   if (do_change && changed) 
   {
      bricks[cx][cy] = cur;
      changed++;

  //    msgbox(MW_INFO, MB_OK, "Num of changed: %d", changed);

      return changed;
   } 
   else 
   {
 //     if (possible) msgbox(MW_INFO, MB_OK, "Num of possible: %d", possible);

      return possible;
   }
}



l_bool check_human_moveable (void) 
{
   // Loops through the whole grid to see if the player can insert
   // a brick somewhere. Returns the number of possible changes.

   l_int x;
   l_int y;
   l_int c = 0;

   for (x=1 ; x<9 ; x++) 
   {
      for (y=1 ; y<9 ; y++) 
         if (bricks[x][y] == 0)
            c += do_move(x, y, c_human, c_computer, 0);
   }
   if (c)
      return TRUE;
   return FALSE;
}



l_bool check_computer_moveable (void) 
{
   // Loops through the whole grid to see if the computer can insert
   // a brick somewhere. Returns the number of possible changes.

   l_int x;
   l_int y;
   l_int c = 0;

   for (x=1 ; x<9 ; x++) 
   {
      for (y=1 ; y<9 ; y++) 
         if (bricks[x][y] == 0)  
            c += do_move(x, y, c_computer, c_human, 0);
   }

   if (c)
      return TRUE;
   return FALSE;
}



l_bool check_game_over (void) 
{
   // Returns 1 if game over, returns 0 otherwise.

   l_bool isOver = FALSE;

   draw_bricks(gameArea);
                                                     // Game is over if:
   if ((p_human + p_computer) > 63) isOver = TRUE;   // Grid is full (64 bricks in total).
   if (p_human < 1) isOver = TRUE;                   // There's no bricks left in human's color
   if (p_computer < 1) isOver = TRUE;                // There's no bricks left in computers's color
   if (!check_human_moveable() 
       && !check_computer_moveable()) isOver = 1;    // Neither of the players can move.


   // Display the appropriate message depending on who has won the game.

   if (isOver == TRUE) 
   {
      if (p_human == p_computer)  
         msgbox(MW_INFO, MB_OK, "Player:      %d\nComputer:   %d\n\nIt's a draw! No one wins...", p_human, p_computer);
      else
      if (p_human > p_computer) 
         msgbox(MW_INFO, MB_OK, "Player:      %d\nComputer:   %d\n\nCongratulations!\nYou won, but it was just because of luck... ;-)", p_human, p_computer);
      else
      if (p_human < p_computer) 
         msgbox(MW_INFO, MB_OK, "Player:      %d\nComputer:   %d\n\nOh yes, I won!\nBetter luck next time... hehe...", p_human, p_computer);
   }

   return isOver;
}



void play_computer (void) 
{
   l_int tab_moves[66][3];
   l_int x;
   l_int y;
   l_int tmp;
   l_int retp;
   l_int c = 1;

   draw_bricks_txt(VIEW(gameArea), "Thinking...");
   if (!do_play_fast) 
      sleep(1);

   for (x=1 ; x<9 ; x++) 
   {
      for (y=1 ; y<9 ; y++) 
      {
         tmp = do_move(x, y, c_computer, c_human, 0);
         if (bricks[x][y] == 0) 
            tab_moves[c][0] = tmp;
         else
            tab_moves[c][0] = 0;
         
         tab_moves[c][1] = x;
         tab_moves[c][2] = y;
         c++;
      }
   }

   for (x=1 ; x<65 ; x++)
   {
      for (y=x ; y>1 ; y--) 
      {
         if (tab_moves[y][0] > tab_moves[y-1][0]) 
         {
            tmp = 0;
            tmp = tab_moves[y-1][0];
            tab_moves[y-1][0] = tab_moves[y][0];
            tab_moves[y][0] = tmp;

            tmp = tab_moves[y-1][1];
            tab_moves[y-1][1] = tab_moves[y][1];
            tab_moves[y][1] = tmp;

            tmp = tab_moves[y-1][2];
            tab_moves[y-1][2] = tab_moves[y][2];
            tab_moves[y][2] = tmp;
         }
      }
   }

   if (tab_moves[1][1] != 0) 
   {
      retp = do_move(tab_moves[1][1], tab_moves[1][2], c_computer, c_human, 1);
      if (retp) {
         p_computer += retp;
         p_human -= retp;
         p_human ++;
      }
   }

}



void trans_event (p_object o, p_event event)
{
   t_point p;
   l_int cx;
   l_int cy;
   l_int ret;
   l_int done = 0;

   if (event->type & EV_MOUSE) 
   {
      if (OBJECT(mouse)->state & MO_SF_MOUSELUP)
      {
         p = gameArea->get_local_point(gameArea, mouse->where);
         cx = (p.x / BRICKSIZE) + 1;
         cy = (p.y / BRICKSIZE) + 1;

         if (p.y<240 && p.y>0 && p.x<240 && p.x>0)
         {
            if (bricks[cx][cy]==0)
            {
               ret = do_move(cx, cy, c_human, c_computer, 1);
               if (ret)
               {
                  p_human += ret;
                  p_computer -= ret;
                  p_computer ++;
                  draw_bricks(gameArea); 
		  if (!check_game_over() && check_computer_moveable())
                  {
                     do
                     {
                        play_computer();
                     } while (!check_human_moveable() && !check_game_over());
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
         case MSG_GAMERESTART_BLACK: 
            init_game();
            c_human = 1;
            c_computer = 2;
            clear_event(event);
            break;

         case MSG_GAMERESTART_WHITE:
            init_game();
            c_human = 2;
            c_computer = 1;
            play_computer();
            clear_event(event);
            break;

         case MSG_PLAY_FAST:
            ret = menu_get_item_flags(gameMenu->menu, MSG_PLAY_FAST);
            if (ret & MIF_CHECKOK)
               do_play_fast = 1;
            else
               do_play_fast = 0; 
            clear_event(event);
            break;

         case MSG_GAMEHELP: 
            open_help("othello.hlp");
            clear_event(event);
            break;

         case MSG_ABOUT: 
            seal2_app_about("Othello", load_image("/system/bmp/icons/othello.ico,32"));
            clear_event(event);
            break;
      }
      draw_bricks(gameArea);
   } 
}



p_menu create_game_menu (void) 
{
   p_menu menu;

   menu = new_menu(
                   new_sub_menu("Game", new_menu(
                      new_menu_item("Restart (play black)", "", 0, MSG_GAMERESTART_BLACK, NULL,
                      new_menu_item("Restart (play white)", "", 0, MSG_GAMERESTART_WHITE, NULL,
                      new_menu_line(
                      new_menu_item("Exit", "ALT+F4", 0, MSG_CLOSE, NULL, 
                      NULL))))),
                   new_sub_menu("Options", new_menu(
                      new_menu_check_item("Fast play", "", do_play_fast, 0, MSG_PLAY_FAST, NULL,
                      NULL)),
                   new_sub_menu("Help", new_menu(
                      new_menu_item("Help", "", 0, MSG_GAMEHELP, NULL,
                      new_menu_line(
                      new_menu_item("About Othello", "", 0, MSG_ABOUT, NULL, 
                      NULL)))),
                   NULL)))
                );
   return menu;
}



void create_window (void)
{
   l_int  tb_h = 20;       // Height of titlebar

   mainWin = appwin_init(_malloc(sizeof(t_appwin)),
                         rect_assign(100, 100, 360, 390 + tb_h),
                         "Othello",
                         WF_MINIMIZE|WF_ABOUT,
                         ap_id,
                         &trans_event);
   mainWin->icon16 = load_image("/system/bmp/icons/othello.ico,16");
   VIEW(mainWin)->align |= TX_ALIGN_CENTER;
   OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(mainWin));


   gameMenu = hormenu_init(_malloc(sizeof(t_menuview)),
                           rect_assign(0, tb_h + 1, 240, tb_h + 20),
                           create_game_menu());
   OBJECT(mainWin)->insert(OBJECT(mainWin), OBJECT(gameMenu));


   gameArea = view_init(_malloc(sizeof(t_view)),
                        rect_assign(10, 26 + tb_h, 250, 310));
   VIEW(gameArea)->draw = &draw_bricks;
   VIEW(gameArea)->transparent = TRUE;
   OBJECT(mainWin)->insert(OBJECT(mainWin), OBJECT(gameArea));
}



void get_reg_settings (void)
{

      if (!key_exists("software/othello")) 
         create_key("software", "othello");


      if (!key_exists("software/othello/play_fast")) 
      {
         create_key("software/othello", "play_fast");
         do_play_fast = 0;
      }
      else
         do_play_fast = get_key_integer("software/othello/play_fast");

}



app_begin (void) 
{
   if (ap_process == AP_INIT) 
   {
      get_reg_settings();
      create_window();
      init_game();
      draw_bricks(gameArea);
   } 
   else

   if (ap_process == AP_FREE) 
   {
      set_key_integer("software/othello/play_fast", do_play_fast);
   }
} app_end;
