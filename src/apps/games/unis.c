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

/*           unis.c
   
     A simple brick game inspired by Tetris, for SEAL 2.0

     Author: Björn Hallström (heor@softhome.net)
        WWW: http://www.student.lu.se/~kem01bha/seal

     Copyright (c) Björn Hallström 2002
*/

#include <allegro.h>
#include <seal.h>
#include <app.h>
#include <grfx.h>
#include <stattext.h>
#include <button.h>
#include <stdlib.h>
#include <random.h>

p_button   but;
p_stattext lab;
p_grfx     game_area = NULL;
BITMAP     *out;
p_appwin   win;  
l_char     curtain_y;
l_char     b_y;
l_char     b_x;
l_int      score;
l_char     b_color;
l_bool     game_lost;
l_bool     speed;
l_bool     remove_a_line_next_time;
l_bool     draw_the_curtain;
l_bool     wait_for_a_keypress;
l_int      game_speed;


l_char blocks [10][4] = {
 {1,0,0,0},
 {0,0,0,0},
 {0,0,0,0},
 {0,0,0,0},
 {0,0,0,0},
 {0,0,0,0},
 {0,0,0,0},
 {0,0,0,0},
 {0,0,0,0},
 {0,0,0,0}};
 
#define MSG_NEWGAME 100001

void init_game (void)
{
	l_char x, y;
	
	for (x = 0; x <= 3; x++)
	{
		for (y = 0; y <= 9; y++)
		{
			blocks [y][x] = 0;
		}
	}

	game_speed = 350;
	game_lost = false;
	b_y = -1;
	b_x = 1;
	b_color = rand () % 2 + 1;
	score = 0;
	speed = false;
	remove_a_line_next_time = false;
	curtain_y = 0;
	draw_the_curtain = false;	

	wait_for_a_keypress = true;
}


void draw_block (l_int x, l_int y, l_int color, BITMAP *out, t_rect r, t_point p)
{
	l_char  xx, yy;
   l_int   col [3];
   l_int   x_p, y_p;
   
   x_p = 2 + p.x + x*30;
   y_p = 2 + p.y + y*30;
	
	col [1] = makecol (250, 150, 150);
	col [2] = makecol (150, 150, 250);
	col [3] = makecol (250, 250, 250);
	line (out, x_p, y_p, x_p + 27, y_p, col [color]);
	line (out, x_p, y_p, x_p, y_p + 28, col [color]);

	col [1] = makecol (120, 20, 20);
	col [2] = makecol (20, 20, 120);
	col [3] = makecol (120, 120, 120);
	line (out, x_p + 28, y_p, x_p + 28, y_p + 27, col [color]);
	line (out, x_p, y_p + 28, x_p + 28, y_p + 28, col [color]);
	
	for (yy = 1; yy <= 27; yy++)
	{
		if (color == 1)
		{
			line (out, x_p + 1, y_p + yy, x_p + 27, y_p + yy, makecol (170 + yy * 2, 20 + yy * 2, 20 + yy * 2));
		}
		if (color == 2)
		{
			line (out, x_p + 1, y_p + yy, x_p + 27, y_p + yy, makecol (20 + yy * 2, 20 + yy * 2, 170 + yy * 2));
		}
		if (color == 3)
		{
			line (out, x_p + 1, y_p + yy, x_p + 27, y_p + yy, makecol (170 + yy * 2, 170 + yy * 2, 170 + yy * 2));
		}
	}
}

void draw_game (p_view o) 
{
	t_rect  r = o->get_local_extent (o);
	t_point p;
	l_int   x, y;
	
	out = o->begin_paint (o, &p, r);
	if (out)
	{
		rectfill (out, p.x + 1, p.y + 1, p.x + 121, p.y + r.b.y - 1, makecol (215, 215, 235));
		rect (out, p.x, p.y, p.x + 122, p.y + r.b.y, makecol (0, 0, 0));
		
		draw_block (b_x, b_y, b_color, out, r, p);
		
		for (x = 0; x < 4; x++)
		{
			for (y = 0; y < 10; y++)
			{
				if (blocks [y][x] != 0) 
				{
					draw_block (x, y, blocks [y][x], out, r, p);
				}
			}
		}
		
		if (wait_for_a_keypress)
		{
			textout_printf (out, o->font, -1, p.x + 15, p.y + 82, 
                         p.x + 105, p.y + 99, TX_ALIGN_CENTER,
                         color_3d_text, TX_NOCOLOR, 0, "Press a key to start", out);
      }
		
	}
	o->end_of_paint(o, r);
}

void remove_line (l_char line_num)
{
	l_char y;
	
	for (y = line_num; y > 0; y--)
	{
		blocks [y][0] = blocks [y - 1][0];
		blocks [y][1] = blocks [y - 1][1];
		blocks [y][2] = blocks [y - 1][2];
		blocks [y][3] = blocks [y - 1][3];
	}
}

/* Checks if there is any line with the same color on all for blocks, then return the line number */
l_bool check_for_lines (l_char *line_num)
{
	l_char y;
	(*line_num) = -1;
	
	for (y=0; y<10; y++)
	{
		if ((blocks [y][0] == blocks [y][1]) && (blocks [y][1] == blocks [y][2]) && 
		    (blocks [y][2] == blocks [y][3]) && (blocks [y][1] != 0))
		{
			(*line_num) = y;
		}
	}
	
	if ((*line_num) > -1)
	{
		return (true);
	}
	else
	{
		return (false);
	}
}
		

void advance_game (p_object o)
{
	l_char line_number;
	l_text  txt;
	l_char  cx, cy;
	
	if (wait_for_a_keypress)
	{
		// nothing
	}
	
	if (game_lost)
	{
		draw_the_curtain = true;
		if (curtain_y > 9)
		{
			draw_the_curtain = false;
		}
	}
	
	if (draw_the_curtain)
	{
		game_speed = 200 - curtain_y * 15;
		OBJECT (game_area)->process_tick = game_speed;
		
		for (cx = 0; cx <= 3; cx++)
		{
			for (cy = 0; cy <= curtain_y; cy++)
			{
				blocks [cy][cx] = 3;
			}
		}
		curtain_y++;
	}
	
	if (blocks [b_y][b_x] != 0)
	{
		game_lost = true;
	}
	
	if (remove_a_line_next_time)
	{
		remove_line (9);
		remove_a_line_next_time = false;
	}
	
	if ((!game_lost) && (!wait_for_a_keypress)) 
	{
		if ( (blocks [b_y + 1][b_x] == 0) && (b_y != 9))
		{
			b_y ++;
		}
		else
		{
			if (speed)
			{
				OBJECT (game_area)->process_tick = game_speed;
				speed = false;
			}
			blocks [b_y][b_x] = b_color;
			b_y = 0;
			b_color = rand () % 2 + 1;

		}
		
		if (check_for_lines (&line_number))
		{
			remove_line (line_number);
			score ++;
			if ((score % 5) == 0)
			{
				remove_a_line_next_time = true;
				if (game_speed > 50)
				{
					game_speed -= 18;
				}
			}
			txt = set_format_text (NULL, "%d", score);
			lab->set_text (lab, txt);
		}	
		
	}
	

	draw_game (VIEW (o));
}

static void translate_event (p_object o, p_event event) 
{
	l_int  h = NULL;
	l_int  w = NULL;
	
	
	if (o->is_state (o, OB_SF_FOCUSED))
	{
		if ((event->type & EV_KEYBOARD) && (!wait_for_a_keypress))
		{
			if (OBJECT(keyb)->state & KB_SF_KEYDOWN)
			{
				switch (KEY_TO(keyb->code)) 
				{
					case KB_RIGHT : 
					{  
						if (b_x < 3)
						{
							if (blocks [b_y][b_x + 1] == 0)
							{
								b_x++;
								draw_game (VIEW (game_area));
							}
						}
						clear_event(event);
					}
					break;

					case KB_LEFT : 
					{  
						if (b_x > 0)
						{
							if (blocks [b_y][b_x - 1] == 0)
							{
								b_x--;
								draw_game (VIEW (game_area));
							}
						}
						clear_event(event);
					}
					break;
					
					case KB_DOWN : 
					{  
						OBJECT (game_area)->process_tick = 30;
						speed = true;
						clear_event(event);
					}
					break;
				}
			}
		}
		if ((event->type & EV_KEYBOARD) && (wait_for_a_keypress))
		{
			wait_for_a_keypress = false;
			clear_event(event);
		}
	}

	
	if (event->type == EV_MESSAGE) 
	{
		switch (event->message) 
		{
         case MSG_ABOUT:
         {
            BITMAP *ico = load_image ("/system/bmp/icons/unis.ico,32");
            seal2_app_about ("Unis 0.1.0", ico);
            destroy_bitmap (ico);
            clear_event (event);
         }
         break;
        
         case MSG_HELP: 
         {
            open_help ("./help/unis.hlp");
            clear_event (event);
         }
         break;

         case MSG_NEWGAME:
         {
            init_game ();
            clear_event (event);
         }
         break;

      }
   }  
}

static void translate_game_event (p_object o, p_event event) 
{
}


void init_window (void)
{
	t_rect   r;
	
	init_game ();	
	
	r = rect_assign (0, 0, 180, 345);
   
	win = appwin_init (_malloc (sizeof (t_appwin)), r, "Unis", WF_MINIMIZE|WF_ABOUT, ap_id, &translate_event);
   
	if (win) 
	{
		VIEW(win)->align |= TX_ALIGN_CENTER;
		win->icon16 = load_image ("/system/bmp/icons/unis.ico,16");
	}
	
	OBJECT (desktop)->insert(OBJECT (desktop), OBJECT (win));
   
	if (win)
	{
		but = button_init(malloc(sizeof(t_button)), rect_assign (r.b.x - 45, r.a.y + 60, r.b.x + 25, r.a.y + 80), "New game", MSG_NEWGAME, BF_DEFAULT);
		OBJECT (win)->insert (OBJECT (win), OBJECT (but));
		but = button_init(malloc(sizeof(t_button)), rect_assign (r.b.x - 45, r.a.y + 90, r.b.x + 25, r.a.y + 110), "Exit", MSG_CLOSE, BF_DEFAULT);
		OBJECT (win)->insert (OBJECT (win), OBJECT (but));

		lab = stattext_init (_malloc (sizeof (t_stattext)), rect_assign (r.b.x - 45, r.a.y + 32, r.b.x - 15, r.a.y + 47), TX_ALIGN_TOP, "Lines:");
		OBJECT (win)->insert (OBJECT (win), OBJECT (lab));
		lab = stattext_init (_malloc (sizeof (t_stattext)), rect_assign (r.b.x - 12, r.a.y + 32, r.b.x - 1, r.a.y + 47), TX_ALIGN_TOP, "0");
		OBJECT (win)->insert (OBJECT (win), OBJECT (lab));

		r = rect_assign (r.a.x + 5, r.a.y + 33, r.a.x + 127, r.b.y - 10);
		game_area = grfx_init (_malloc (sizeof (t_grfx)), r);
		VIEW (game_area)->draw = &draw_game;
		OBJECT (game_area)->func_callback = &advance_game;
		OBJECT (game_area)->translate_event = &translate_game_event;
		OBJECT (game_area)->process_tick = game_speed;
		OBJECT (win)->insert (OBJECT (win), OBJECT (game_area));
	}

}

app_begin ( void ) 
{
	if ( ap_process == AP_INIT ) /* When ap start */
	{
		AP_SETNUMOFCALLS (1); /* Set MAX of Calls */
		init_window (); /* Run the init void */
	}

	if (ap_process == AP_FREE) 
	{
	}
  
	if (ap_process == AP_DONE) /* When ap done */
	{
	}
} app_end;
