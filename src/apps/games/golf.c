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
 * 17/01/2002: First release by Tobias Johansson
 *
 * 02/05/2002: Cleaned up the code to match the rest of Seal,
 *             made it work better with skins,
 *             added functions for statistics,
 *             replaced the old About box with "seal2_app_about()".
 *             (Tobias J)
 *
 * 02/06/2002: Updated icon path (orudge)
 */

#include <seal.h>
#include <app.h>
#include <view.h>
#include <cards.h>
#include <menus.h>
#include <helpsys.h>


#define MSG_GOLF_NEW      100001
#define MSG_GOLF_DECK     100002
#define MSG_GOLF_HELP     100003
#define MSG_CARDS_ABOUT   100004
#define MSG_GOLF_SELGAME  100005
#define MSG_GOLF_STAT     100006
#define MSG_GOLF_RES_STAT 100007


p_appwin   winMain;
p_view     vGame;
p_menuview gameMenu;

t_card deck[53];

t_card piles[7][5]; 
t_card stock[20];
t_card waste[53];

l_int pilesCount[7];
l_int stockCount;
l_int wasteCount;

l_int win_count = 0;
l_int lost_count = 0;
l_int cur_game;
l_int started_game;


SetInfoAppName      ("Golf");
SetInfoDesciption   ("Simple cardgame for Seal 2.");
SetInfoCopyright    ("Copyright (c) 1999-2002 SEAL Developers. All Rights Reserved.");
SetInfoManufacturer ("Tobias Johansson");


void   show_statistics  (void);
void   init_game        (void);
void   redraw_vGame     (p_view v);
void   redraw_win_title (void);
void   trans_event      (p_object o, p_event event);
p_menu create_game_menu (void);



void init_game (void) 
{
   l_int tc;
   l_int tp;
   l_int curCard = 52;

   redraw_win_title();

   // Fill up the seven lower piles.
   for (tp=0 ; tp<7 ;tp++) 
   {
      for (tc=0 ; tc<5 ; tc++) 
      {
         piles[tp][tc].showFace = true;
         piles[tp][tc].card = deck[curCard].card;
         curCard--;
      }
      pilesCount[tp] = 5;
   }

   // Fill up the stock.
   for (tc=0 ; tc<16 ; tc++) 
      stock[tc] = deck[tc+1];

   stockCount = 16;

   // Add th topmost card to the waste.
   waste[0] = deck[17];
   waste[0].showFace = true;
   wasteCount = 1;
}



void redraw_vGame (p_view v) 
{
   t_point p;
   l_int   cCard;
   l_int   cPile;
   l_int   curY;

   t_rect  r = v->size_limits(v); 
   BITMAP  *out = v->begin_paint(v, &p, r);

   if (out) 
   {
      // Draw green background.
      rectfill(out, p.x, p.y, p.x+600, p.y+400, COLOR(CO_GREEN));

      // Draw the seven piles.
      for (cPile=0 ; cPile<7 ; cPile++) 
      {
         curY = 16;
         for (cCard=0 ; cCard<pilesCount[cPile] ; cCard++) 
         {
            if (cCard-1 >= 0) 
               curY += CARD_LOFFSET;
            cards_blit_card(out, p.x+14+(cPile*80), p.y+curY, piles[cPile][cCard]);
         }
      }

      // Draw the waste.
      for (cCard=0 ; cCard<wasteCount ; cCard++)
         cards_blit_card(out, p.x+95+(cCard*7), p.y+250, waste[cCard]);

      // Draw back of the stock.
      if (stockCount)
         cards_blit_back(out, p.x+14, p.y+250);
      else
         cards_blit_symbol(out, p.x+14, p.y+250, C_EMPTY);

      // Print out the number of cards in the stock.
      textout_printf(out, v->font, -1, 
                     p.x + 14,  p.y + 348, 
                     p.x + 89,  p.y + 360, 
                     TX_ALIGN_CENTER, COLOR(CO_WHITE), 
                     TX_NOCOLOR, 0, "%d", stockCount);

      rect(out, p.x, p.y, p.x + r.b.x, p.y + r.b.y, COLOR(CO_BLACK));
   }

   v->end_of_paint(v, r);
}



void redraw_win_title (void) 
{
   // Redraws the title of the window to show
   // which game is currently selected.

   if (winMain) 
   {
      _free(WINDOW(winMain)->caption);
      WINDOW(winMain)->caption = set_format_text(NULL, "Golf - game %d", cur_game);
      TEST_SUB_VIEWS(VIEW(winMain), WINDOW(winMain)->draw_title(WINDOW(winMain)));
   }
}



void show_statistics (void)
{
   float winPerc  = 0;
   l_int lostPerc = 0;
   l_int totGames = lost_count + win_count;

   if (totGames)
   {
      winPerc  = ((float) win_count  / totGames) * 100;
      lostPerc = 100 - (l_int) winPerc;
   }

   msgbox(MW_INFO, MB_OK, "Games played:   %d\n\nGames won:     %d  (%d%%)\n\nGames lost:      %d  (%d%%)", totGames, win_count, (l_int) winPerc, lost_count, lostPerc);

}



void trans_event (p_object o, p_event event) 
{
   l_int c;
   l_int cc;
   l_int cPile;
   l_int ret_game;
   l_dword msg;
   t_point p;

   if (event->type & EV_MOUSE) 
   {
      if (OBJECT(mouse)->state & MO_SF_MOUSELUP) 
      {
         p = vGame->get_local_point(vGame, mouse->where);

         if (p.x > +14 && p.x < +105 && p.y > +250 && p.y < +(250+CARD_HEIGHT))
         {
            // The stock is clicked. Move a new card to the waste. 
            if (stockCount) {
               wasteCount ++;
               waste[wasteCount-1] = stock[stockCount-1];
               waste[wasteCount-1].showFace = true;
               stockCount --;
            }
         } 
         else

         if (p.x > +14 && p.x < 566 && p.y > +16 && p.y < +179) 
         {
            // One of the seven piles is clicked.
            cPile = (int) (p.x - 14) / 80;   // Which pile?
            c = pilesCount[cPile]-1;


            // If the value of the clicked card equals the value
            // of the topmost card in the waste plus/minus one... 

            if (cards_get_value(piles[cPile][c]) == cards_get_value(waste[wasteCount-1]) - 1 
                || cards_get_value(piles[cPile][c]) == cards_get_value(waste[wasteCount-1]) + 1) 
            {
               // ...and the card in the waste isn't a king...
               if (cards_get_value(waste[wasteCount-1]) < 13) 
               {
                  // ...move the card to the waste!
                  wasteCount ++;
                  waste[wasteCount-1] = piles[cPile][c];
                  pilesCount[cPile]--;
               }
               started_game = 1;
            }
         }

         redraw_vGame(vGame);
         clear_event(event);

         // Game over?
         if (wasteCount == 52)
            msgbox(MW_INFO, MB_OK, "Well done! You did it!\n");
      }
   }


   if (event->type == EV_MESSAGE) 
   {
      switch (event->message) 
      {
         case MSG_GOLF_NEW:

            cur_game = cards_shuffle_deck_ex(deck);
            init_game();

            if (started_game) 
               lost_count++;
            started_game = 0;

            redraw_vGame(vGame);
            clear_event(event);
            break;


         case MSG_GOLF_SELGAME:

            ret_game = cards_game_select(cur_game);
            if (ret_game) 
            {
               cur_game = ret_game;
               cards_init_deck(deck);
               cards_shuffle_deck_select(deck, cur_game);
               init_game();

               if (started_game) 
                  lost_count++;
               started_game = 0;
            }
            redraw_vGame(vGame);
            clear_event(event);
            break;


         case MSG_GOLF_STAT:

            show_statistics();
            clear_event(event);
            break;


         case MSG_GOLF_RES_STAT:

            msg = msgbox(MW_INFO, MB_YES|MB_NO, "Are you sure you want to reset the statistics?");

            if (msg == MSG_YES) 
            {
               win_count = 0;
               lost_count = 0;
            }
            clear_event(event);
            break;


         case MSG_GOLF_DECK:

            cards_select_deck();
            redraw_vGame(vGame);
            clear_event(event);
            break;


         case MSG_GOLF_HELP:

            open_help("./help/golf.hlp");
            clear_event(event);
            break;


         case MSG_ABOUT:

            // msgbox(MW_INFO, MB_OK, "\"Golf\" cardgame for Seal 2\n\n(c) 2001 Tobias Johansson\n\nEmail:   tobbe_snodd@hotmail.com\nWeb:   http://hem.passagen.se/sealfiles\n");
            seal2_app_about("Golf", load_image("/system/bmp/icons/golf.ico,32"));
            clear_event(event);
            break;


         case MSG_CARDS_ABOUT:

            cards_about();
            clear_event(event);
            break;

      }
   }
}



p_menu create_game_menu (void) 
{
   p_menu menu;

   menu = new_menu(
                 new_sub_menu("Game", new_menu(
                    new_menu_item("New game", "", 0, MSG_GOLF_NEW, NULL,
                    new_menu_item("Select game", "", 0, MSG_GOLF_SELGAME, NULL,
                    new_menu_line(
                    new_menu_item("Show statistics", "", 0, MSG_GOLF_STAT, NULL,
                    new_menu_item("Reset statistics", "", 0, MSG_GOLF_RES_STAT, NULL,
                    new_menu_line(
                    new_menu_item("Exit", "ALT+F4", 0, MSG_CLOSE, NULL, 
                    NULL)))))))),
                 new_sub_menu("Options", new_menu(
                    new_menu_item("Select deck", "", 0, MSG_GOLF_DECK, NULL,
                    NULL)),
                 new_sub_menu("Help", new_menu(
                    new_menu_item("Help", "", 0, MSG_GOLF_HELP, NULL,
                    new_menu_line(
                    new_menu_item("About Golf", "", 0, MSG_ABOUT, NULL, 
                    new_menu_item("About \"cards.xdl\"", "", 0, MSG_CARDS_ABOUT, NULL, 
                    NULL))))), 
                 NULL)))
              );

   return menu;
}



void create_window (void) 
{
   l_int tb_h = 20;// = apppastitle->d.a.y + apppastitle->d.b.y + 17;       // Height of titlebar

   winMain = appwin_init(_malloc(sizeof(t_appwin)), 
                         rect_assign(0, 0, 585, 400 + tb_h),
                         "Golf", 
                         WF_MINIMIZE|WF_ABOUT|WF_SYSMENU, 
                         ap_id, 
                         &trans_event);
   VIEW(winMain)->align |= TX_ALIGN_CENTER;
   winMain->icon16 = load_image("/system/bmp/icons/golf.ico,16");
   OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(winMain));


   gameMenu = hormenu_init(_malloc(sizeof(t_menuview)), 
                           rect_assign(0, tb_h + 1, 585, tb_h + 20), 
                           create_game_menu());
   OBJECT(winMain)->insert(OBJECT(winMain), OBJECT(gameMenu));


   vGame = view_init(_malloc(sizeof(t_view)),
                     rect_assign(4, tb_h + 26, 580, 395 + tb_h));
   VIEW(vGame)->draw = &redraw_vGame;
   OBJECT(winMain)->insert(OBJECT(winMain), OBJECT(vGame));

}



void get_reg_settings (void)
{
      // Get settings from registry if they exists.
      // Otherwise create keys, set the values to 
      // zero and select a random game.

      if (!key_exists("software/golf")) 
         create_key("software", "golf");


      if (!key_exists("software/golf/lastgame")) 
      {
         create_key("software/golf", "lastgame");
         cur_game = cards_shuffle_deck_ex(deck);
      }
      else
      {
         cur_game = get_key_integer("software/golf/lastgame");
         cards_shuffle_deck_select(deck, cur_game);
      }


      if (!key_exists("software/golf/won")) 
      {
         create_key("software/golf", "won");
         win_count = 0;
      }
      else
         win_count = get_key_integer("software/golf/won");


      if (!key_exists("software/golf/lost")) 
      {
         create_key("software/golf", "lost");
         lost_count = 0;
      }
      else
         lost_count = get_key_integer("software/golf/lost");
}



app_begin (void) 
{
   if (ap_process == AP_INIT) 
   {
      cards_init_deck(deck);
      get_reg_settings();
      init_game();
      create_window();
      redraw_win_title();
   }
   else

   if (ap_process == AP_DONE)
   {
      if (started_game) 
         lost_count++;

      set_key_integer("software/golf/lastgame", cur_game);
      set_key_integer("software/golf/won", win_count);
      set_key_integer("software/golf/lost", lost_count);
      cards_uninit_deck();
   }

} app_end;
