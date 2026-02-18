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
 * 02/04/2002: First release by Tobias Johansson
 *
 * 02/05/2002: Cleaned up the code to match the rest of Seal,
 *             made it work better with skins and replaced
 *             the old About box with "seal2_app_about()".
 *             (Tobias J)
 *
 * 02/06/2002: Updated icon path (orudge)
 * 18/01/2003: Killed some bugs related to the new skinning system (xduffy)
 */

#include <seal.h>
#include <app.h>
#include <view.h>
#include <cards.h>
#include <menus.h>
#include <button.h>
#include <helpsys.h>

#define MSG_CRUEL_NEW     100001
#define MSG_CRUEL_HELP    100002    
#define MSG_CARDS_ABOUT   100003
#define MSG_CRUEL_SELGAME 100004    // Select game
#define MSG_DEAL_CARDS    100005
#define MSG_STAT          100006    // Show statistics
#define MSG_STATRES       100007    // Reset statistics


p_appwin   winMain;
p_view     vGame;
p_menuview gameMenu;

t_card deck[53];
t_card piles[16][13];            // 0-11 is the lower piles, 12-15 suite piles
l_int  piles_count[16];          // The number of cards in the piles

l_int from_pile;                 // Currently selected pile
l_int cur_game;                  // Number of the current game
l_int was_double_clicked = 0;    // Used to check wether a pile was couble clicked

l_int  win_count;                // Number of won games
l_int  lost_count;               // Number of lost games

l_bool started_game = FALSE;     // Is set to TRUE whenever a card is moved. Is used
                                 // to check whether to add one to lost_count or not.

SetInfoAppName      ("Cruel");
SetInfoDesciption   ("A simple single-player cardgame.");
SetInfoCopyright    ("Copyright (c) 1999-2002 SEAL Developers. All Rights Reserved.");
SetInfoManufacturer ("Tobias Johansson");


// Function prototypes
void   redraw_win_title (void);
void   init_game        (void);
void   move_card        (l_int from_pile, l_int to_pile);
void   deselect_all     (void);
void   deal_cards       (void);
void   redraw_vGame     (p_view v);
void   trans_event      (p_object o, p_event event);
p_menu create_game_menu (void);



void redraw_win_title (void) 
{
   // Redraws the title of the window to show
   // which game is currently selected.

   if (winMain) 
   {
      _free(WINDOW(winMain)->caption);
      WINDOW(winMain)->caption = set_format_text(NULL, "Cruel - game %d", cur_game);
      TEST_SUB_VIEWS(VIEW(winMain), WINDOW(winMain)->draw_title(WINDOW(winMain)));
   }
}



void init_game (void) 
{
   l_int  curCard;
   l_int  curPile;
   l_int  curSuitePile = 12;

   from_pile = 0;
   redraw_win_title();

   for (curPile=0 ; curPile<=15 ; curPile++) piles_count[curPile] = 0;

   // Move all the aces from the deck to the suite piles
   curPile = 0;
   for (curCard=1; curCard<53 ; curCard++) 
   {
      deck[curCard].showFace = true;
      if (cards_get_value(deck[curCard]) == 1) 
      {
         piles[curSuitePile][0] = deck[curCard];
         piles_count[curSuitePile] = 1;
         curSuitePile++;
      }
      else
      {
         if (piles_count[curPile] == 4)
            curPile++;
         piles[curPile][piles_count[curPile]] = deck[curCard];
         piles_count[curPile]++;
      }
   }
}



void move_card (l_int from_pile, l_int to_pile) 
{
   // Move the topmost card of from_pile to the
   // topmost position of to_pile.

   piles_count[to_pile]++;
   piles[to_pile][piles_count[to_pile]-1] = piles[from_pile][piles_count[from_pile]-1];
   piles_count[from_pile]--;

   started_game = 1;
}



void deselect_all (void) 
{
   // Deselects all the cards in the deck.

   l_int cp;
   l_int cc;

   for (cp=0 ; cp<16 ; cp++) 
   {
      for (cc=0 ; cc<piles_count[cp] ; cc++)
         piles[cp][cc].selected = false;
   }
   from_pile = 0;
}



void deal_cards (void) 
{
   l_int cur_pile;
   l_int cur_card; 
   l_int num_cards = 0;

   deselect_all();
   cards_init_deck(deck);

   for (cur_pile=0 ; cur_pile<12 ; cur_pile++) 
   {
      for (cur_card=0 ; cur_card<piles_count[cur_pile] ; cur_card++) 
      {
         num_cards++;
         deck[num_cards] = piles[cur_pile][cur_card];
         deck[num_cards].showFace = true;
      }
   }

   for (cur_pile=0 ; cur_pile<12 ; cur_pile++) 
      piles_count[cur_pile] = 0;

   cur_pile = 0;
   cur_card = 1;
   while (cur_card<=num_cards) 
   {
      piles_count[cur_pile]++;
      piles[cur_pile][piles_count[cur_pile]-1] = deck[cur_card];
      if (piles_count[cur_pile] == 4 && cur_pile+1<12) 
         cur_pile++;
      cur_card++;
   }
}



void redraw_vGame (p_view v) 
{
   t_point p;
   l_int   cPile;
   t_rect  r;
   BITMAP  *out;

   //DEBUG_printf("About to paint\n");
   r = v->size_limits(v);
   out = v->begin_paint(v, &p, r);
   if (out)
   {
      // Draw green background.
      //DEBUG_printf("Cruel: Draw: rectfill()\n");
      rectfill(out, p.x, p.y, p.x+600, p.y+400, COLOR(CO_GREEN));
      
      //DEBUG_printf("Cruel: Draw: blitcards, round 1\n");
      for (cPile=0 ; cPile<4 ; cPile++)
         cards_blit_card(out,
                         p.x + 30 + ((CARD_WIDTH + 15) * cPile),
                         p.y + 20,
                         piles[12+cPile][piles_count[12+cPile]-1]);
      
      //DEBUG_printf("Cruel: Draw: blitcards, round 2\n");
      for (cPile=0 ; cPile<6 ; cPile++)
         if (piles_count[cPile])
            cards_blit_card(out,
                            p.x + 30 + ((CARD_WIDTH + 15) * cPile),
                            p.y + CARD_HEIGHT + 65,
                            piles[cPile][piles_count[cPile]-1]);

      //DEBUG_printf("Cruel: Draw: blitcards, round 3\n");
      for (cPile=6 ; cPile<12 ; cPile++)
         cards_blit_card(out,
                         p.x + 30 + ((CARD_WIDTH + 15) * (cPile - 6)),
                         p.y + (2 * CARD_HEIGHT) + 80,
                         piles[cPile][piles_count[cPile]-1]);

      //DEBUG_printf("Cruel: Draw: blit symbol\n");
      cards_blit_symbol(out,
                        p.x + 460,
                        p.y + 20,
                        C_CIRCLE);

      //DEBUG_printf("Cruel: Draw: textout DEAL\n");
      textout_printf(out, v->font, -1,
                     p.x + 462, p.y + 95,
                     p.x + 460 + CARD_WIDTH, p.y + 120,
                     TX_ALIGN_CENTER, COLOR(CO_BLACK),
                     TX_NOCOLOR, 0, "DEAL");
      //DEBUG_printf("Cruel: Draw: rect()\n");
      rect(out, p.x, p.y, p.x + r.b.x, p.y + r.b.y, COLOR(CO_BLACK));
   }
   v->end_of_paint(v, r);
   //DEBUG_printf("Painting done\n");
}



void trans_event (p_object o, p_event event) 
{
   l_int cPile;
   l_int cp;
   l_int selGame;

   l_int totGames;
   l_int lostPerc;
   float winPerc;

   l_dword msg;
   t_point p;

   RETVIEW(o, event);

   if (event->type & EV_MOUSE)
   {
      p = VIEW(vGame)->get_local_point(VIEW(vGame), mouse->where);

      if (OBJECT(mouse)->state & MO_SF_MOUSELUP) 
      {
         if (was_double_clicked) 
         {
            was_double_clicked = 0;
            deselect_all();
         }
         else


         // A "suite pile" was clicked
         if (p.x>29 && p.x<359 && p.y>20 && p.y<116) 
         {
            cPile = (int) (p.x - 29) / (CARD_WIDTH + 15);
            cPile += 12;
            if (from_pile 
                && (cards_get_value(piles[from_pile-1][piles_count[from_pile-1]-1]) == cards_get_value(piles[cPile][piles_count[cPile]-1]) + 1) 
                && (cards_get_suite(piles[from_pile-1][piles_count[from_pile-1]-1]) == cards_get_suite(piles[cPile][piles_count[cPile]-1]))) 
            {
               move_card(from_pile-1, cPile);
               deselect_all();
            } 
            else
               deselect_all();
         }
         else


         // Another pile was clicked
         if (p.x>29 && p.x<531 && p.y>161 && p.y<368)
         {
            cPile = (int) (p.x - 29) / (CARD_WIDTH + 15);
            if (p.y>263)
               cPile+=6;

            if (from_pile && (cards_get_value(piles[from_pile-1][piles_count[from_pile-1]-1]) == cards_get_value(piles[cPile][piles_count[cPile]-1]) - 1) 
                && (cards_get_suite(piles[from_pile-1][piles_count[from_pile-1]-1]) == cards_get_suite(piles[cPile][piles_count[cPile]-1]))) 
            { 
               move_card(from_pile-1, cPile);
               deselect_all();
            }
            else 
            if (piles_count[cPile] && !from_pile)
            {
               piles[cPile][piles_count[cPile]-1].selected = true;
               from_pile = cPile+1;
            }
            else
               deselect_all();
         } 
         else


         // Deal symbol was clicked
         if (p.x>459 && p.x<531 && p.y>20 && p.y<116) 
         {
            deal_cards();
         }
         else
            deselect_all();


         // Is the game won?
         if (piles_count[12]+piles_count[13]+piles_count[14]+piles_count[15] == 52) 
         {
            msgbox(MW_INFO, MB_OK, "Well done! You won!");
            win_count = 1;
         }

         redraw_vGame(vGame);
         clear_event(event);
      }


      if (OBJECT(mouse)->state & MO_SF_MOUSELDOUBLE) 
      {
         // One of the twelve "play piles" was double clicked
         if (p.x>29 && p.x<531 && p.y>161 && p.y<368)
         {
            cPile = (int) (p.x - 29) / (CARD_WIDTH + 15);
            if (p.y>240) 
               cPile+=6;

            for (cp=12 ; cp<16 ; cp++) 
            {
               if (piles_count[cPile] && (cards_get_value(piles[cPile][piles_count[cPile]-1]) == cards_get_value(piles[cp][piles_count[cp]-1]) + 1) && cards_get_suite(piles[cp][piles_count[cp]-1]) == cards_get_suite(piles[cPile][piles_count[cPile]-1]))
                  move_card(cPile, cp);
            }
         }

         was_double_clicked = 1;
         deselect_all();
         redraw_vGame(vGame);
         clear_event(event);
      }
   }


   if (event->type == EV_MESSAGE)
   {
      switch (event->message)
      {
         case MSG_DEAL_CARDS:

            deal_cards();
            redraw_vGame(vGame);
            clear_event(event);
            break;


         case MSG_CRUEL_SELGAME:

            cards_init_deck(deck);
            selGame = cards_game_select(cur_game);
            if (selGame) 
            {
               cur_game = selGame;
               cards_shuffle_deck_select(deck, cur_game);
               init_game();
               redraw_win_title();

               if (started_game) 
                  lost_count++;
               started_game = 0;
            }
            redraw_vGame(vGame);
            clear_event(event);
            break;


         case MSG_CRUEL_NEW: 

            cards_init_deck(deck);
            cur_game = cards_shuffle_deck_ex(deck);
            init_game();

            if (started_game)
               lost_count++;
            started_game = 0;

            redraw_vGame(vGame);
            clear_event(event);
            break;


         case MSG_STAT:

            totGames = lost_count + win_count;
            winPerc  = 0;
            lostPerc = 0;

            if (totGames)
            {
               winPerc  = ((float) win_count  / totGames) * 100;
               lostPerc = 100 - (l_int) winPerc;
            }
            msgbox(MW_INFO, MB_OK, "Games played:   %d\n\nGames won:     %d  (%d%%)\n\nGames lost:      %d  (%d%%)", totGames, win_count, (l_int) winPerc, lost_count, lostPerc);
            clear_event(event);
            break;


         case MSG_STATRES:

            msg = msgbox(MW_INFO, MB_YES|MB_NO, "Are you sure you want to reset the statistics?");
            if (msg == MSG_YES) 
            {
               win_count = 0;
               lost_count = 0;
            }
            clear_event(event);
            break;


         case MSG_CRUEL_HELP: 
            open_help("./help/cruel.hlp");
            clear_event(event);
            break;


         case MSG_ABOUT: 

            // msgbox(MW_INFO, MB_OK, "\"Cruel\" cardgame for Seal 2\n\n(c) 2002 Tobias Johansson\n\nEmail:   tobbe_snodd@hotmail.com\nWeb:   http://hem.passagen.se/sealfiles\n");
            seal2_app_about("Cruel", load_image("/system/bmp/icons/cruel.ico,32"));
            clear_event(event);
            break;


         case MSG_CARDS_ABOUT:

            cards_about();
            clear_event(event);
            break;

      }
   }
}



p_menu create_game_menu(void) 
{
   p_menu menu;

   menu = new_menu(
                 new_sub_menu("Game", new_menu(
                    new_menu_item("New random game", "", 0, MSG_CRUEL_NEW, NULL,
                    new_menu_item("Select game", "", 0, MSG_CRUEL_SELGAME, NULL,
                    new_menu_line(
                    new_menu_item("Show statistics", "", 0, MSG_STAT, NULL, 
                    new_menu_item("Reset statistics", "", 0, MSG_STATRES, NULL, 
                    new_menu_line(
                    new_menu_item("Exit", "ALT+F4", 0, MSG_CLOSE, NULL, 
                    NULL)))))))),
                 new_sub_menu("Help", new_menu(
                    new_menu_item("Help", "", 0, MSG_CRUEL_HELP, NULL,
                    new_menu_line(
                    new_menu_item("About Cruel", "", 0, MSG_ABOUT, NULL, 
                    new_menu_item("About \"cards.xdl\"", "", 0, MSG_CARDS_ABOUT, NULL, 
                    NULL))))), 
                 NULL))
              );

   return menu;
}



void create_window(void) 
{
   l_int tb_h = 20;// = apppastitle->d.a.y + apppastitle->d.b.y + 17;       // Height of titlebar

   ////DEBUG_printf("Cruel: appwin_init\n");
   winMain = appwin_init(_malloc(sizeof(t_appwin)),
                         rect_assign(0, 0, 572, 438), //415
                         "Cruel",
                         WF_MINIMIZE|WF_ABOUT|WF_SYSMENU,
                         ap_id,
                         &trans_event);
   //DEBUG_printf("Cruel: appwin_init done\n");
   VIEW(winMain)->align |= TX_ALIGN_CENTER;
   winMain->icon16 = load_image("/system/bmp/icons/cruel.ico,16");
   OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(winMain));

   //DEBUG_printf("Cruel: hormenu_init\n");
   gameMenu = hormenu_init(_malloc(sizeof(t_menuview)),
                           rect_assign(0, 1 + tb_h, 472, 20 + tb_h),
                           create_game_menu());
   //DEBUG_printf("Cruel: hormenu_init done\n");
   OBJECT(winMain)->insert(OBJECT(winMain), OBJECT(gameMenu));

   //DEBUG_printf("Cruel: view_init\n");
   vGame = view_init(_malloc(sizeof(t_view)),
                     rect_assign(4, 26 + tb_h, 567, 413 + tb_h));
   //DEBUG_printf("Cruel: view_init done\n");
   VIEW(vGame)->draw = &redraw_vGame;
   //DEBUG_printf("Cruel: insert into winMain\n");
   OBJECT(winMain)->insert(OBJECT(winMain), OBJECT(vGame));
   //DEBUG_printf("Cruel: Done!!!\n");
}



void get_reg_settings (void)
{
      // Get settings from registry if they exists.
      // Otherwise create keys, set the values to 
      // zero and select a random game.

      if (!key_exists("software/cruel")) 
         create_key("software", "cruel");


      if (!key_exists("software/cruel/lastgame")) 
      {
         create_key("software/cruel", "lastgame");
         cur_game = cards_shuffle_deck_ex(deck);
      }
      else
      {
         cur_game = get_key_integer("software/cruel/lastgame");
         cards_shuffle_deck_select(deck, cur_game);
      }


      if (!key_exists("software/cruel/won")) 
      {
         create_key("software/cruel", "won");
         win_count = 0;
      }
      else
         win_count = get_key_integer("software/cruel/won");


      if (!key_exists("software/cruel/lost")) 
      {
         create_key("software/cruel", "lost");
         lost_count = 0;
      }
      else
         lost_count = get_key_integer("software/cruel/lost");
}



app_begin (void) 
{
   if (ap_process == AP_INIT) 
   {
      //DEBUG_printf("Cruel: About to init card deck\n");
      cards_init_deck(deck);
      //DEBUG_printf("Cruel: About to get reg settings\n");
      get_reg_settings();
      //DEBUG_printf("Cruel: About to init game\n");
      init_game();
      //DEBUG_printf("Cruel: About to init create window\n");
      create_window();
      //DEBUG_printf("Cruel: About to redraw window title\n");
      redraw_win_title();
      //DEBUG_printf("Cruel: Init done\n");
   }
   else

   if (ap_process == AP_DONE)
   {
      if (started_game)
         lost_count++;

      // Store settings in registry when exiting
      set_key_integer("software/cruel/lastgame", cur_game);
      set_key_integer("software/cruel/won", win_count);
      set_key_integer("software/cruel/lost", lost_count);
      cards_uninit_deck();
   }

} app_end;
