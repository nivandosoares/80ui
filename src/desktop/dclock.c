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

/* Taskbar clock for SEAL 2.0. Based on the Bad Seal 0.53 clock. */

#include <allegro.h>
#include <seal.h>
#include <app.h>
#include <dialogs.h>
#include "desktop.h"
#include <dos.h>

l_color face3d;

typedef struct t_clock *p_clock;

typedef struct t_clock {
   struct t_view obclass;
   char *text;
} t_clock;

void clock_draw(p_view o)
{
   t_rect r = o->get_local_extent(o);
   t_point p;
   BITMAP *out = o->begin_paint(o, &p, r);

   if (out)
   {
      rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, face3d);
      textout_draw_rect(out, o->font, ((p_clock)(o))->text, -1, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, TX_ALIGN_CENTER, COLOR(0), face3d, 0);
   }

   o->end_of_paint(o, r);
}

t_point clock_size_minimum(p_view o)
{
   return(point_assign(0, 0));
};

p_clock _clock_init(p_clock o, t_rect r)
{
   if (!o) return NULL;

   clear_type(o, sizeof(t_clock));

   view_init(&o->obclass, r);
   o->text = (char *) malloc(9);

   VIEW(o)->draw = &clock_draw;
   VIEW(o)->size_minimum = &clock_size_minimum;

   return(o);
}

p_clock (*clock_init)(p_clock o, t_rect r) = &_clock_init;

p_clock seal_clock;

void refresh(p_object o)
{
   struct time t;
//   char s[4];
   char ampm[3] = "am";
   gettime(&t);

   if (t.ti_hour > 12)
   {
      t.ti_hour -= 12;
      strcpy(ampm, "pm");
   }

   sprintf(seal_clock->text, "%d:%02d%s", t.ti_hour, t.ti_min, ampm);  // OCR
/*  itoa(t.ti_hour, s, 10);
   strcpy(clock->text, s);
   strcat(clock->text, ":");
   itoa(t.ti_min, s, 10);
   strcat(clock->text, s);
   strcat(clock->text, ampm);*/

   VIEW(seal_clock)->draw(VIEW(seal_clock));
}

void create_clock()
{
   t_rect r = rect_assign(0, 0, 45, 15);
   AP_SETNUMOFCALLS(1);
   face3d = registry_color("3d_face");

   seal_clock = clock_init(malloc(sizeof(t_stattext)), r);
   VIEW(seal_clock)->change_bounds(VIEW(seal_clock), r);
   OBJECT(seal_clock)->process_tick = 100;
   OBJECT(seal_clock)->func_callback = &refresh;
   add_to_userbox(OBJECT(seal_clock));
   init_stillprocess(OBJECT(seal_clock), 100);
}
