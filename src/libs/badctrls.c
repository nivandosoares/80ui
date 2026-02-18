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
/*                          badctrls.c                          */
/*                     Bad Controls for SEAL                    */
/*                            v0.4.2                            */
/*                                                              */
/*      (c) 2000, 2001 Kostas Michalopoulos aka Bad Sector      */
/*         first version for Bad Seal | www.badseal.org         */
/*                                                              */
/* ============================================================ */
/*  Controls included:                                          */
/*    t_bad_checkbox                          Checkbox control  */
/*    t_bad_button                        t_button replacement  */
/*    t_bad_vtrackbar                        vertical trackbar  */
/*    t_bad_tabbook                                   tab book  */
/*    t_bad_iconbox                         box with icons :-)  */
/*    t_bad_vscrollbar                      vertical scrollbar  */
/* ============================================================ */
/*                                                              */
/*                Seal - free desktop environment               */
/*                                                              */
/*                    Copyright (c) 1999,2000                   */
/*                         Michal Stencl                        */
/*                     All Rights Reserved                      */
/*                                                              */
/* mail : stenclpmd@ba.telecom.sk                               */
/* web  : http://www.home.sk/public/seal/                       */
/*                                                              */
/* This file is part of Seal.                                   */
/*                                                              */
/* Seal is free software; you can redistribute it and/or        */
/* modify it under the terms of the GNU General Public License  */
/* as published by the Free Software Foundation; either version */
/* 2, or (at your option) any later version.                    */
/*                                                              */
/* Seal is distributed in the hope that it will be useful, but  */
/* WITHOUT ANY WARRANTY; without even the implied warranty of   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See    */
/* the GNU General Public License for more details.             */
/*                                                              */
/* You should have received a copy of the GNU General Public    */
/* License along with Seal; see the file COPYING.  If not,      */
/* write to the Free Software Foundation, 675 Mass Ave,         */
/* Cambridge, MA 02139, USA.                                    */
/*                                                              */
/****************************************************************/

#include <allegro.h>
#include <seal.h>
#include <badctrls.h>
//#include <beditor.h>

SetInfoAppName("Bad Controls 0.4.2");
SetInfoDesciption("Bad Controls 0.4.2");
SetInfoCopyright("Copyright (c) Kostas Michalopoulos aka Bad Sector 2000-2001");
SetInfoManufacturer("Kostas Michalopoulos");

//#include "beditor.c"

/* common palette for Bad Controls
 0 3d face
 1 3d light
 2 3d shadow
 3 3d dark
 4 3d border
 5 text color
 6 flat color
 7 flat border
 8 highlight
 9 highlight text
10 iconbox text
11 filebox background
12 filebox text
*/

l_color pal_bad_common[] = {CO_LIGHTGRAY,
                            CO_WHITE,
                            CO_DARKGRAY,
                            CO_BLACK,
                            CO_BLACK,
                            CO_BLACK,
                            CO_WHITE,
                            CO_BLACK,
                            CO_BLUE,
                            CO_WHITE,
                            CO_BLACK,
                            CO_WHITE,
                            CO_BLACK,
                            CO_NOCOLOR};

p_bad_checkbox (*bad_checkbox_init)(p_bad_checkbox o, t_rect r, l_text cap, l_dword msg, l_int st)=&_bad_checkbox_init;
p_bad_button (*bad_button_init)(p_bad_button o, t_rect r, l_text cap, l_int style, l_dword msg)=&_bad_button_init;
p_bad_vtrackbar (*bad_vtrackbar_init)(p_bad_vtrackbar o, t_rect r, l_dword msg)=&_bad_vtrackbar_init;
p_bad_tabbook (*bad_tabbook_init)(p_bad_tabbook o, t_rect r, l_text ftitle)=&_bad_tabbook_init;
p_bad_iconbox (*bad_iconbox_init)(p_bad_iconbox o, t_rect r, l_big flags)=&_bad_iconbox_init;
p_bad_vscrollbar (*bad_vscrollbar_init)(p_bad_vscrollbar o, t_rect r, l_int flags)=&_bad_vscrollbar_init;

static DATAFILE *dat = NULL;

#define Get_Bitmap(n) (BITMAP*)GET_DATA(dat, n)

void light_image ( BITMAP *bmp, int x1, int y1, int x2, int y2, int dr, int dg, int db )
{
  int x = x1;
  int y = y1;
  int r, g, b;
  int col;
  if (x1 > x2) {
    int tmp = x1;
    x1 = x2;
    x2 = x1;
  };
  if (y1 > y2) {
    int tmp = y1;
    y1 = y2;
    y2 = y1;
  };

  for (;;) {
    for (;;) {
      col = getpixel(bmp, x, y);
      r = getr(col);
      g = getg(col);
      b = getb(col);

      r += dr;
      g += dg;
      b += db;

      if (r < 0) r = 0; else if (r > 255) r = 255;
      if (g < 0) g = 0; else if (g > 255) g = 255;
      if (b < 0) b = 0; else if (b > 255) b = 255;

      col = makecol(r, g, b);
      putpixel(bmp, x, y, col);

      x++;
      if (x > x2) break;
    };
    y++;
    if (y > y2) break;
    x = x1;
  };
};

void draw_bevel(BITMAP *out, int x1, int y1, int x2, int y2, int c1, int c2)
{
  if (out) {
    line(out, x1, y1, x2, y1, c1);
    line(out, x1, y1, x1, y2, c1);
    line(out, x2, y1, x2, y2, c2);
    line(out, x1, y2, x2, y2, c2);
  }
}

/* t_bad_checkbox */
l_bool bad_checkbox_done(p_object o)
{
  if (!view_done(o)) return false;

  afree(&(BAD_CHECKBOX(o)->caption));

  return true;
};

void bad_checkbox_set_state(p_object o, l_dword st, l_bool set)
{
  view_set_state(o, st, set);

  if ( st & OB_SF_FOCUSED ) {

    VIEW(o)->draw_view(VIEW(o));

  };
};

void bad_checkbox_translate_event(p_object o, t_event *event)
{
  RETVIEW(o, event);

  view_translate_event(o, event);

  /* check if we're clicked by the mouse */
  if (event->type & EV_MOUSE && OBJECT(mouse)->state & MO_SF_MOUSELDOWN)
  {
    l_bool in_view = false;

    if (VIEW(o)->is_mouse_in_view(VIEW(o))) {
      BAD_CHECKBOX(o)->grayed = true;
      BAD_CHECKBOX(o)->draw_state(BAD_CHECKBOX(o), in_view);
    };

    while (OBJECT(mouse)->state & MO_SF_MOUSELPRESS)
    {
      l_bool safe_in_view = in_view;
      in_view = VIEW(o)->is_mouse_in_view(VIEW(o));
      if (in_view != safe_in_view) {
        BAD_CHECKBOX(o)->grayed = in_view;
        BAD_CHECKBOX(o)->draw_state(BAD_CHECKBOX(o), in_view);
      };

      o->get_event(o, event);
    };

    if (in_view) {
      if (BAD_CHECKBOX(o)->state < 2) {
        BAD_CHECKBOX(o)->state = 1-BAD_CHECKBOX(o)->state;
        BAD_CHECKBOX(o)->grayed = false;
        BAD_CHECKBOX(o)->draw_state(BAD_CHECKBOX(o), false);
        set_event(event, EV_MESSAGE, BAD_CHECKBOX(o)->message, o);
        o->put_event(o, event);
        clear_event(event);
      } else {
        BAD_CHECKBOX(o)->grayed = false;
        BAD_CHECKBOX(o)->draw_state(BAD_CHECKBOX(o), false);
      };
    };
  };

};

void bad_checkbox_draw_state(p_bad_checkbox o, l_int astate)
{
  t_rect r = VIEW(o)->get_local_extent(VIEW(o));
  t_point p;

  BITMAP *out = VIEW(o)->begin_paint(VIEW(o), &p, r);

  if ( out ) {
    rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 0));

    if ((o->state != 2) && (o->grayed == false)) rectfill(out, r.a.x+p.x, r.a.y+p.y, r.a.x+p.x+14, r.a.y+p.y+14, VIEW(o)->get_color(VIEW(o), 6));
    line(out, r.a.x+p.x, r.a.y+p.y, r.a.x+p.x, r.a.y+p.y+14, VIEW(o)->get_color(VIEW(o), 2));
    line(out, r.a.x+p.x, r.a.y+p.y, r.a.x+p.x+14, r.a.y+p.y, VIEW(o)->get_color(VIEW(o), 2));
    line(out, r.a.x+p.x+14, r.a.y+p.y, r.a.x+p.x+14, r.a.y+p.y+14, VIEW(o)->get_color(VIEW(o), 1));
    line(out, r.a.x+p.x, r.a.y+p.y+14, r.a.x+p.x+14, r.a.y+p.y+14, VIEW(o)->get_color(VIEW(o), 1));

    line(out, r.a.x+p.x+1, r.a.y+p.y+1, r.a.x+p.x+1, r.a.y+p.y+13, VIEW(o)->get_color(VIEW(o), 3));
    line(out, r.a.x+p.x+1, r.a.y+p.y+1, r.a.x+p.x+13, r.a.y+p.y+1, VIEW(o)->get_color(VIEW(o), 3));
    line(out, r.a.x+p.x+13, r.a.y+p.y+1, r.a.x+p.x+13, r.a.y+p.y+13, VIEW(o)->get_color(VIEW(o), 0));
    line(out, r.a.x+p.x+1, r.a.y+p.y+13, r.a.x+p.x+13, r.a.y+p.y+13, VIEW(o)->get_color(VIEW(o), 0));

    textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x+17, r.a.y+p.y+1,
                           r.b.x+p.x-1, r.b.y+p.y-1, TX_ALIGN_LEFT, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);

    if ( OBJECT(o)->is_state(OBJECT(o), OB_SF_FOCUSED) ) {
      rect(out, r.a.x+p.x+16, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 4));
    };
    if (o->state == 1) {
      line(out, r.a.x+p.x+3, r.a.y+p.y+3, r.a.x+p.x+11, r.a.y+p.y+11, COLOR(0));
      line(out, r.a.x+p.x+11, r.a.y+p.y+3, r.a.x+p.x+3, r.a.y+p.y+11, COLOR(0));
      line(out, r.a.x+p.x+4, r.a.y+p.y+3, r.a.x+p.x+11, r.a.y+p.y+10, COLOR(0));
      line(out, r.a.x+p.x+10, r.a.y+p.y+3, r.a.x+p.x+3, r.a.y+p.y+10, COLOR(0));
      line(out, r.a.x+p.x+3, r.a.y+p.y+4, r.a.x+p.x+10, r.a.y+p.y+11, COLOR(0));
      line(out, r.a.x+p.x+11, r.a.y+p.y+4, r.a.x+p.x+4, r.a.y+p.y+11, COLOR(0));
    }
  };

  VIEW(o)->end_of_paint(VIEW(o), r);
};

void bad_checkbox_change_state(p_bad_checkbox o, l_int newstate)
{
  o->state = newstate;
  VIEW(o)->draw_view(VIEW(o));
};

void bad_checkbox_change_caption(p_bad_checkbox o, l_text newcap)
{
  afree(&(BAD_CHECKBOX(o)->caption));
  o->caption = (l_text)_strdup((char*)newcap);
  VIEW(o)->draw_view(VIEW(o));
}

void bad_checkbox_draw(p_view o)
{
  BAD_CHECKBOX(o)->draw_state(BAD_CHECKBOX(o), false);
};

p_bad_checkbox _bad_checkbox_init(p_bad_checkbox o, t_rect r, l_text cap, l_dword msg, l_int st)
{
  if (!o) return NULL;

  clear_type(o, sizeof(t_bad_checkbox));

  view_init(&o->obclass, r);

  o->caption = (l_text)_strdup((char*)cap);
  o->message = msg;
  o->state = st;

  OBJECT(o)->translate_event = &bad_checkbox_translate_event;
  OBJECT(o)->set_state = &bad_checkbox_set_state;
  OBJECT(o)->done = &bad_checkbox_done;

  VIEW(o)->draw = &bad_checkbox_draw;
  VIEW(o)->set_palette(VIEW(o), pal_bad_common);

  o->draw_state = &bad_checkbox_draw_state;
  o->change_state = &bad_checkbox_change_state;
  o->change_caption = &bad_checkbox_change_caption;

  return o;
};

/* t_bad_button */
l_bool bad_button_done(p_object o)
{
  if (!view_done(o)) return false;

  afree(&(BAD_BUTTON(o)->caption));

  return true;
};

void bad_button_set_state(p_object o, l_dword st, l_bool set)
{
  view_set_state(o, st, set);

  if ( st & OB_SF_FOCUSED ) {

    VIEW(o)->draw_view(VIEW(o));

  };
};

void bad_button_translate_event(p_object o, t_event *event)
{
  RETVIEW(o, event);

  view_translate_event(o, event);

  /* check if we're clicked by the mouse */
  if (event->type & EV_MOUSE && OBJECT(mouse)->state & MO_SF_MOUSELDOWN)
  {
    l_bool in_view = false;

    if (VIEW(o)->is_mouse_in_view(VIEW(o))) {
      BAD_BUTTON(o)->draw_state(BAD_BUTTON(o), in_view);
    };

    while (OBJECT(mouse)->state & MO_SF_MOUSELPRESS)
    {
      l_bool safe_in_view = in_view;
      in_view = VIEW(o)->is_mouse_in_view(VIEW(o));
      if (in_view != safe_in_view) {
        BAD_BUTTON(o)->draw_state(BAD_BUTTON(o), in_view);
      };

      o->get_event(o, event);
    };

    if (in_view) {
      if (BAD_BUTTON(o)->state < 2) {
        if (BAD_BUTTON(o)->switch_button == true)
          BAD_BUTTON(o)->state = 1-BAD_BUTTON(o)->state;
        else
          BAD_BUTTON(o)->state = 0;
        BAD_BUTTON(o)->draw_state(BAD_BUTTON(o), false);
        set_event(event, EV_MESSAGE, BAD_BUTTON(o)->message, o);
        o->put_event(o, event);
        clear_event(event);
      } else {
        BAD_BUTTON(o)->state = 0;
        BAD_BUTTON(o)->draw_state(BAD_BUTTON(o), false);
      };
    };
  };

};

void bad_button_draw_state(p_bad_button o, l_int astate)
{
  t_rect r = VIEW(o)->get_local_extent(VIEW(o));
  t_point p;

  BITMAP *out = VIEW(o)->begin_paint(VIEW(o), &p, r);

  if ( out ) {
    rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 0));
    switch (o->style) {
      case BBS_SEAL: {
        if (astate)
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, COLOR(0), COLOR(15));
        else
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, COLOR(15), COLOR(0));
        textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
      } break;
      case BBS_BADSEAL: {
        if (astate) {
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 2), VIEW(o)->get_color(VIEW(o), 1));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
        } else {
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 2), VIEW(o)->get_color(VIEW(o), 3));
          draw_bevel(out, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x-1, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 1), VIEW(o)->get_color(VIEW(o), 2));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
        };
      } break;
      case BBS_KDE: {
        if (astate) {
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 2), VIEW(o)->get_color(VIEW(o), 1));
          draw_bevel(out, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x-1, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 2), VIEW(o)->get_color(VIEW(o), 1));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x+1, r.b.y+p.y+1, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
        } else {
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 1), VIEW(o)->get_color(VIEW(o), 2));
          draw_bevel(out, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x-1, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 1), VIEW(o)->get_color(VIEW(o), 2));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
        };
      } break;
      case BBS_WIN95: {
        if (astate) {
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, COLOR(0), COLOR(0));
          draw_bevel(out, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x-1, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 2), VIEW(o)->get_color(VIEW(o), 2));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x+1, r.b.y+p.y+1, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
        } else {
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 1), COLOR(0));
          draw_bevel(out, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x-1, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 0), VIEW(o)->get_color(VIEW(o), 2));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
        };
      } break;
      case BBS_WIN31: {
        line(out, r.a.x+p.x, r.a.y+p.y+1, r.a.x+p.x, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 4));
        line(out, r.b.x+p.x, r.a.y+p.y+1, r.b.x+p.x, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 4));
        line(out, r.a.x+p.x+1, r.a.y+p.y, r.b.x+p.x-1, r.a.y+p.y, VIEW(o)->get_color(VIEW(o), 4));
        line(out, r.a.x+p.x+1, r.b.y+p.y, r.b.x+p.x-1, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 4));
        if (astate) {
          draw_bevel(out, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x-1, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 2), VIEW(o)->get_color(VIEW(o), 0));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x+1, r.b.y+p.y+1, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x+2, r.a.y+p.y+1, r.b.x+p.x+2, r.b.y+p.y+1, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
        } else {
          draw_bevel(out, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x-1, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 1), VIEW(o)->get_color(VIEW(o), 2));
          draw_bevel(out, r.a.x+p.x+2, r.a.y+p.y+2, r.b.x+p.x-2, r.b.y+p.y-2, VIEW(o)->get_color(VIEW(o), 1), VIEW(o)->get_color(VIEW(o), 2));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x+1, r.a.y+p.y, r.b.x+p.x+1, r.b.y+p.y, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
        };
      } break;
      case BBS_MODERN: {
        if (astate) {
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 2), VIEW(o)->get_color(VIEW(o), 2));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x+2, r.a.y+p.y+2, r.b.x+p.x+2, r.b.y+p.y+1, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 1), TX_NOCOLOR, 0);
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x+1, r.b.y+p.y+1, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 2), TX_NOCOLOR, 0);
        } else {
          draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 1), VIEW(o)->get_color(VIEW(o), 2));
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x+1, r.b.y+p.y+1, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 1), TX_NOCOLOR, 0);
          textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 2), TX_NOCOLOR, 0);
        };
      } break;
      case BBS_LIQUID: {
        if (astate) {
          stretch_sprite(out, Get_Bitmap(2), r.a.x+p.x, r.a.y+p.y, rect_sizex(r), rect_sizey(r));
        } else {
          stretch_sprite(out, Get_Bitmap(1), r.a.x+p.x, r.a.y+p.y, rect_sizex(r), rect_sizey(r));
        };
        textout_draw_rect(out, VIEW(o)->font, o->caption, -1, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, TX_ALIGN_CENTER, COLOR(0), TX_NOCOLOR, 0);
      } break;
    };
  };

  VIEW(o)->end_of_paint(VIEW(o), r);
};

void bad_button_change_state(p_bad_button o, l_int newstate)
{
  o->state = newstate;
  VIEW(o)->draw_view(VIEW(o));
};

void bad_button_change_style(p_bad_button o, l_int newstyle)
{
  o->style = newstyle;
  VIEW(o)->draw_view(VIEW(o));
};

void bad_button_change_caption(p_bad_button o, l_text newcap)
{
  afree(&(BAD_BUTTON(o)->caption));
  o->caption = (l_text)_strdup((char*)newcap);
  VIEW(o)->draw_view(VIEW(o));
}

void bad_button_draw(p_view o)
{
  BAD_BUTTON(o)->draw_state(BAD_BUTTON(o), false);
};

p_bad_button _bad_button_init(p_bad_button o, t_rect r, l_text cap, l_int style, l_dword msg)
{
  if (!o) return NULL;

  clear_type(o, sizeof(t_bad_button));

  view_init(&o->obclass, r);

  o->caption = (l_text)_strdup((char*)cap);
  o->message = msg;
  o->state = 0;
  o->style = style;

  OBJECT(o)->translate_event = &bad_button_translate_event;
  OBJECT(o)->set_state = &bad_button_set_state;
  OBJECT(o)->done = &bad_button_done;

  VIEW(o)->draw = &bad_button_draw;
  VIEW(o)->set_palette(VIEW(o), pal_bad_common);

  o->draw_state = &bad_button_draw_state;
  o->change_state = &bad_button_change_state;
  o->change_style = &bad_button_change_style;
  o->change_caption = &bad_button_change_caption;

  return o;
};

/* t_bad_vtrackbar */
void bad_vtrackbar_set_state(p_object o, l_dword st, l_bool set)
{
  view_set_state(o, st, set);

  if ( st & OB_SF_FOCUSED ) {

    VIEW(o)->draw_view(VIEW(o));

  };
};

void bad_vtrackbar_translate_event(p_object o, t_event *event)
{
  RETVIEW(o, event);

  view_translate_event(o, event);

  /* check if we're clicked by the mouse */
  if (event->type & EV_MOUSE && OBJECT(mouse)->state & MO_SF_MOUSELDOWN)
  {
    t_point p = VIEW(o)->get_global_point(VIEW(o), point_assign(0, 0));
    t_rect r = VIEW(o)->get_local_extent(VIEW(o));
    l_long my = (r.b.y-r.a.y-4);
    l_long first_pos = BAD_VTRACKBAR(o)->pos;
    l_bool in_view;

    VIEW(o)->draw_view(VIEW(o));

    while (OBJECT(mouse)->state & MO_SF_MOUSELPRESS)
    {
      l_bool old_in_view = in_view;
      l_long old_pos = BAD_VTRACKBAR(o)->pos;
      in_view = VIEW(o)->is_mouse_in_view(VIEW(o));

      if (in_view) BAD_VTRACKBAR(o)->pos = ((mouse->where.y)-p.y)*BAD_VTRACKBAR(o)->max/my;

      if (BAD_VTRACKBAR(o)->pos > BAD_VTRACKBAR(o)->max) BAD_VTRACKBAR(o)->pos = BAD_VTRACKBAR(o)->max;
      if (BAD_VTRACKBAR(o)->pos < BAD_VTRACKBAR(o)->min) BAD_VTRACKBAR(o)->pos = BAD_VTRACKBAR(o)->min;

      if (in_view != old_in_view)
      {
        if (in_view) BAD_VTRACKBAR(o)->pressed = true; else BAD_VTRACKBAR(o)->pressed = false;
        VIEW(o)->draw_view(VIEW(o));
      };

      if (BAD_VTRACKBAR(o)->pos != old_pos) VIEW(o)->draw_view(VIEW(o));

      o->get_event(o, event);
    };
    BAD_VTRACKBAR(o)->pressed = false;
    VIEW(o)->draw_view(VIEW(o));

    if (BAD_VTRACKBAR(o)->pos != first_pos)
    {
      VIEW(o)->draw_view(VIEW(o));

      set_event(event, EV_MESSAGE, BAD_VTRACKBAR(o)->message, o);
      o->put_event(o, event);
      clear_event(event);
    };
  };
};

void bad_vtrackbar_change_pos(p_bad_vtrackbar o, l_long pos)
{
  o->pos = pos;
  VIEW(o)->draw_view(VIEW(o));
};

void bad_vtrackbar_draw(p_view o)
{
  t_rect r = VIEW(o)->get_local_extent(VIEW(o));
  t_point p;
  l_long px, py, my;

  BITMAP *out = VIEW(o)->begin_paint(VIEW(o), &p, r);

  if ( out ) {
    px = r.a.x+p.x+((r.b.x-r.a.x)/2);
    my = (r.b.y-r.a.y-8);
    if (BAD_VTRACKBAR(o)->max == 0) py = r.a.y+p.y+4; else py = BAD_VTRACKBAR(o)->pos*my/BAD_VTRACKBAR(o)->max+r.a.y+p.y+4;
    rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 0));
    draw_bevel(out, px-1, r.a.y+p.y+4, px+1, r.b.y+p.y-4, VIEW(o)->get_color(VIEW(o), 2), VIEW(o)->get_color(VIEW(o), 1));
    vline(out, px, r.a.y+p.y+5, r.b.y+p.y-5, COLOR(0));
    rectfill(out, px-10, py-4, px+10, py+4, VIEW(o)->get_color(VIEW(o), 0));
    draw_bevel(out, px-10, py-4, px+10, py+4, VIEW(o)->get_color(VIEW(o), 1), VIEW(o)->get_color(VIEW(o), 3));
    draw_bevel(out, px-9, py-3, px+9, py+3, VIEW(o)->get_color(VIEW(o), 0), VIEW(o)->get_color(VIEW(o), 2));
    if (BAD_VTRACKBAR(o)->pressed) draw_bevel(out, px-7, py-1, px+7, py+1, VIEW(o)->get_color(VIEW(o), 2), VIEW(o)->get_color(VIEW(o), 1));
  };

  VIEW(o)->end_of_paint(VIEW(o), r);
};

p_bad_vtrackbar _bad_vtrackbar_init(p_bad_vtrackbar o, t_rect r, l_dword msg)
{
  if (!o) return NULL;

  clear_type(o, sizeof(t_bad_vtrackbar));

  view_init(&o->obclass, r);

  o->message = msg;
  o->min = 0;
  o->max = 100;
  o->pos = 0;
  o->pressed = false;

  OBJECT(o)->translate_event = &bad_vtrackbar_translate_event;
  OBJECT(o)->set_state = &bad_vtrackbar_set_state;

  VIEW(o)->draw = &bad_vtrackbar_draw;
  VIEW(o)->set_palette(VIEW(o), pal_bad_common);

  o->change_pos = &bad_vtrackbar_change_pos;

  return o;
};

/* t_bad_tabbook */
void bad_tabbook_set_state(p_object o, l_dword st, l_bool set)
{
  view_set_state(o, st, set);

  if ( st & OB_SF_FOCUSED ) {

    VIEW(o)->draw_view(VIEW(o));

  };
};

void bad_tabbook_translate_event(p_object o, t_event *event)
{
  t_rect r = VIEW(o)->get_local_extent(VIEW(o));
  t_point p = VIEW(o)->get_global_point(VIEW(o), point_assign(0, 0));
  l_int i;

  view_translate_event(o, event);

  RETVIEW(o, event);

  /* check if we're clicked by the mouse */
  if (event->type & EV_MOUSE && OBJECT(mouse)->state & MO_SF_MOUSELDOWN)
  {
    l_int tab_h = 21;
    l_int x;
    l_int old_active = BAD_TABBOOK(o)->active;

    x = r.a.x+p.x+1;
    for (i=0;i<BAD_TABBOOK(o)->tabs;i++) {
      l_int tab_width = FONT_GETSTRWIDTH(VIEW(o)->font, BAD_TABBOOK(o)->title[i], -1)+10;
      l_int y1 = r.a.y+p.y+1;
      t_rect rr;

      if (i == BAD_TABBOOK(o)->active) y1--;

      rr = rect_assign(x, y1, x+tab_width, r.a.y+p.y+tab_h);

      if (mouse->where.x >= rr.a.x && mouse->where.x <= rr.b.x && mouse->where.y >= rr.a.y && mouse->where.y <= rr.b.y ) {
//        BAD_TABBOOK(o)->active = i;
        if (i != old_active) {
          BAD_TABBOOK(o)->select_tab(BAD_TABBOOK(o), i);
        };
        while (OBJECT(mouse)->state & MO_SF_MOUSELPRESS)
        {
          o->get_event(o, event);
        };
        break;
      };

      x += tab_width+3;
    };
  };

  if (event->type != EV_NOTHING)
  for (i=0;i<BAD_TABBOOK(o)->tabs;i++)
    OBJECT(BAD_TABBOOK(o)->tab[i])->translate_event(OBJECT(BAD_TABBOOK(o)->tab[i]), event);
};

void bad_tabbook_tab_translate_event(p_object o, t_event *event)
{
  p_object oo, f = o->first_view(o);

  view_translate_event(o, event);

  if (VIEW(o) != BAD_TABBOOK(o->owner->owner)->tab[BAD_TABBOOK(o->owner->owner)->active]) return;

  oo = f;
  do {
    if (oo != o) oo->translate_event(oo, event);
    oo = oo->next_view(oo);
  } while (oo != f);
};

void bad_tabbook_draw_tab(p_view o)
{
  t_rect r = VIEW(o)->get_local_extent(VIEW(o));
  t_point p;

  BITMAP *out = o->begin_paint(o, &p, r);

  if ( out ) {
    rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, o->get_color(o, 0));
  };

  o->end_of_paint(o, r);
};

void bad_tabbook_draw(p_view o)
{
  t_rect r = VIEW(o)->get_local_extent(VIEW(o));
  t_point p;

  BITMAP *out = VIEW(o)->begin_paint(VIEW(o), &p, r);

  if ( out ) {
    l_int tab_h = 21;
    l_int i,x;

    rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.a.x+p.y+tab_h, VIEW(o)->get_color(VIEW(o), 0));
    draw_bevel(out, r.a.x+p.x, r.a.y+p.y+tab_h, r.b.x+p.x-1, r.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 1), VIEW(o)->get_color(VIEW(o), 2));
    draw_bevel(out, r.a.x+p.x, r.a.y+p.y+tab_h, r.b.x+p.x, r.b.y+p.y, VIEW(o)->get_color(VIEW(o), 1), VIEW(o)->get_color(VIEW(o), 3));

    x = r.a.x+p.x+1;
    for (i=0;i<BAD_TABBOOK(o)->tabs;i++) {
      l_int tab_width = FONT_GETSTRWIDTH(VIEW(o)->font, BAD_TABBOOK(o)->title[i], -1)+10;
      l_int y1 = r.a.y+p.y+1;
      l_int xx = 0;

      if (i == BAD_TABBOOK(o)->active) {
        y1--;
        xx = 1;
      };

      line(out, x-xx, y1+1, x-xx, r.a.y+p.y+tab_h-1, VIEW(o)->get_color(VIEW(o), 1));
      line(out, x-xx+1, y1, x+tab_width+xx-1, y1, VIEW(o)->get_color(VIEW(o), 1));
      line(out, x+tab_width+xx-1, y1+1, x+xx+tab_width-1, r.a.y+p.y+tab_h-1, VIEW(o)->get_color(VIEW(o), 2));
      line(out, x+tab_width+xx, y1+1, x+tab_width+xx, r.a.y+p.y+tab_h-1, VIEW(o)->get_color(VIEW(o), 3));

      if (i == BAD_TABBOOK(o)->active) {
        textout_draw_rect(out, VIEW(o)->font, BAD_TABBOOK(o)->title[i], -1, x+2, r.a.y+p.y, x+tab_width-2, r.a.y+p.y+tab_h-2, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
        line(out, x-xx+1, r.a.y+p.y+tab_h, x+tab_width+xx-1, r.a.y+p.y+tab_h, VIEW(o)->get_color(VIEW(o), 0));
      } else {
        light_image(out, x-xx+2, y1+1, x+tab_width+xx-1, r.a.y+p.y+tab_h-1, -10, -10, -10);
        textout_draw_rect(out, VIEW(o)->font, BAD_TABBOOK(o)->title[i], -1, x+2, r.a.y+p.y+1, x+tab_width-2, r.a.y+p.y+tab_h-1, TX_ALIGN_CENTER, VIEW(o)->get_color(VIEW(o), 5), TX_NOCOLOR, 0);
      };

      x += tab_width+3;
    };
  };

  VIEW(o)->end_of_paint(VIEW(o), r);
};

void bad_tabbook_select_tab(p_bad_tabbook o, l_int newtab)
{
  if (newtab != o->active) {
    t_rect r = rect_assign(-5, -5, -4, -4);
    BAD_TABBOOK(o)->tab[o->active]->change_bounds(BAD_TABBOOK(o)->tab[o->active], r);
    o->active = newtab;
    VIEW(o)->draw_view(VIEW(o));
    r = VIEW(o)->bounds;
    r = rect_assign(1, 22, rect_sizex(r)-2, rect_sizey(r)-2);
    BAD_TABBOOK(o)->tab[o->active]->change_bounds(BAD_TABBOOK(o)->tab[o->active], r);
//    BAD_TABBOOK(o)->tab[newtab]->show(BAD_TABBOOK(o)->tab[newtab]);
    if (o->on_changed) o->on_changed(o, o->active);
  };
};

void bad_tabbook_add_tab(p_bad_tabbook o, l_text title)
{
  t_rect r = VIEW(o)->bounds;
  if (o->tabs == 64) return;

//  r = rect_assign(1, 22, rect_sizex(r)-2, rect_sizey(r)-2);
  r = rect_assign(-5, -5, -4, -4);
  o->title[o->tabs] = strdup(title);
  o->tab[o->tabs] = view_init(malloc(sizeof(t_view)), r);
  o->tab[o->tabs]->set_palette(o->tab[o->tabs], pal_bad_common);
  o->tab[o->tabs]->draw = &bad_tabbook_draw_tab;
  OBJECT(o->tab[o->tabs])->translate_event = &bad_tabbook_tab_translate_event;
//  VIEW(o->tab[o->tabs])->hide(VIEW(o->tab[o->tabs]));
  OBJECT(o)->insert(OBJECT(o), OBJECT(o->tab[o->tabs]));
  o->tabs++;
  VIEW(o)->draw_view(VIEW(o));
  VIEW(o->tab[o->active])->draw_view(VIEW(o->tab[o->active]));
};

void bad_tabbook_remove_tab(p_bad_tabbook o, l_int tab)
{
  // do nothing
};

p_bad_tabbook _bad_tabbook_init(p_bad_tabbook o, t_rect r, l_text ftitle)
{
  t_rect rr;
  if (!o) return NULL;

  clear_type(o, sizeof(t_bad_tabbook));

  view_init(&o->obclass, r);

  rr = rect_assign(1, 22, rect_sizex(r)-2, rect_sizey(r)-2);

  o->title[0] = strdup(ftitle);
  o->tab[0] = view_init(malloc(sizeof(t_view)), rr);
  o->active = 0;
  o->tabs = 1;

  o->tab[0]->set_palette(o->tab[0], pal_bad_common);
  o->tab[0]->draw = &bad_tabbook_draw_tab;
  OBJECT(o->tab[0])->translate_event = &bad_tabbook_tab_translate_event;
  OBJECT(o)->insert(OBJECT(o), OBJECT(o->tab[0]));

  OBJECT(o)->translate_event = &bad_tabbook_translate_event;
  OBJECT(o)->set_state = &bad_tabbook_set_state;

  VIEW(o)->draw = &bad_tabbook_draw;
  VIEW(o)->set_palette(VIEW(o), pal_bad_common);

  o->select_tab = &bad_tabbook_select_tab;
  o->add_tab = &bad_tabbook_add_tab;
  o->on_changed = NULL;

  return o;
};

/* t_bad_iconbox */

void bad_iconbox_draw_icon(p_bad_iconbox o, BITMAP *out, t_point p, t_point xy, l_int item)
{
  p_bad_iconbox_item i = o->icons->at(o->icons, item);
  l_color c = VIEW(o)->get_color(VIEW(o), 10);
  l_color b = TX_NOCOLOR;
  l_int w, h;

  if (i)
  {
    if (i->selected)
    {
      c = VIEW(o)->get_color(VIEW(o), 9);
      b = VIEW(o)->get_color(VIEW(o), 8);
    };

    stretch_sprite(out, i->icon, xy.x+p.x, xy.y+p.y, 32, 32);

    get_size_of_ftext(i->caption, VIEW(o)->font, &w, &h);

    textout_draw_rect(out, VIEW(o)->font, i->caption, -1, xy.x+p.x+16-w/2, xy.y+p.y+38, xy.x+p.x+16+w/2, xy.y+p.y+38+h, TX_ALIGN_CENTER, c, b, 1);
  };
}

void bad_iconbox_draw(p_view o)
{
  p_bad_iconbox ib = ((p_bad_iconbox)(o));
  l_int i, x = ib->spacing.x/2, y = ib->spacing.y/2;
  t_rect r = o->get_local_extent(o);
  t_point p;
  BITMAP *out = o->begin_paint(VIEW(o), &p, r);

  if (out) {
    if (ib->flags & BIB_3D_BORDER) {
      x += 2;
      y += 2;
    };
    if (ib->scr) y -= ib->scr->pos;

    if (ib->flags & BIB_BACKGROUND_IMAGE) {
      if (ib->flags & BIB_DESKTOP_PLACEMENT)
        stretch_sprite(out, o->brush.background, 0, 0, desktop->bounds.b.x, desktop->bounds.b.y);
      else
        stretch_sprite(out, o->brush.background, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y);
    } else
      fade_rect(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, o->brush.color, o->brush.color2, FR_VER);

    if (ib->count > 0) {
      l_int rectsizey = rect_sizey(r);
      for (i=0;i<ib->count;i++) {
        if (y > -38-ib->spacing.y)
          bad_iconbox_draw_icon(ib, out, p, point_assign(x, y), i);

        if (ib->flags & BIB_PLACE_HORIZONTAL)
        {
          x += 38+ib->spacing.x;
          if (x+ib->spacing.x+32 > rect_sizex(r)) {
            y += 38+ib->spacing.y;
            x = ib->spacing.x/2;
          };
        } else {
          y += 38+ib->spacing.y;
          if (y+ib->spacing.y+32 > rect_sizey(r)) {
            x += 38+ib->spacing.x;
            y = ib->spacing.y/2;
          };
        };
        if (y > rectsizey) break;
      };
    };

    if (ib->selection.a.x > -1) {
      draw_bevel(out, ib->selection.a.x, ib->selection.a.y, ib->selection.b.x, ib->selection.b.y, o->get_color(o, 0), o->get_color(o, 2));
      draw_bevel(out, ib->selection.a.x+1, ib->selection.a.y+1, ib->selection.b.x-1, ib->selection.b.y-1, o->get_color(o, 0), o->get_color(o, 2));
      light_image(out, ib->selection.a.x+2, ib->selection.a.y+2, ib->selection.b.x-2, ib->selection.b.y-2, -25, -25, -25);
    };

    if (ib->flags & BIB_3D_BORDER) {
      draw_bevel(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, o->get_color(o, 2), o->get_color(o, 1));
      draw_bevel(out, r.a.x+p.x+1, r.a.y+p.y+1, r.b.x+p.x-1, r.b.y+p.y-1, o->get_color(o, 3), o->get_color(o, 0));
    };
  };

  r.b.x += 15;

  o->draw_sub_views(o, VIEW(OBJECT(o)->first_view(OBJECT(o))), VIEW(OBJECT(o)->first_view(OBJECT(o))));
  o->end_of_paint(o, r);
};

t_point bad_iconbox_icon_position(p_bad_iconbox o, l_int item)
{
  l_int x = o->spacing.x/2, y = o->spacing.y/2, i;
  t_rect r = VIEW(o)->get_local_extent(VIEW(o));
  if (o->flags & BIB_3D_BORDER) {
    x += 2;
    y += 2;
  };
  if (o->scr) y -= o->scr->pos;

  for (i=item;i;i--)
  if (o->flags & BIB_PLACE_HORIZONTAL)
  {
    x += 38+o->spacing.x;
    if (x+o->spacing.x+32 > rect_sizex(r)) {
      y += 38+o->spacing.y;
      x = o->spacing.x/2;
    };
  } else {
    y += 38+o->spacing.y;
    if (y+o->spacing.y+32 > rect_sizey(r)) {
      x += 38+o->spacing.x;
      y = o->spacing.y/2;
    };
  };

  return point_assign(x, y);
}

l_bool bad_iconbox_mouse_under_icon_at(p_bad_iconbox o, l_int item)
{
  t_point p = VIEW(o)->get_global_point(VIEW(o), point_assign(0, 0));
  t_point ip = bad_iconbox_icon_position(o, item);

  return (mouse->where.x >= ip.x+p.x && mouse->where.x <= ip.x+p.x+31 && mouse->where.y >= ip.y+p.y && mouse->where.y <= ip.y+p.y+31);
}

void bad_iconbox_add(p_bad_iconbox o, BITMAP *icon, l_text caption)
{
  p_bad_iconbox_item i = (t_bad_iconbox_item *) malloc(sizeof(t_bad_iconbox_item));
  t_point p = bad_iconbox_icon_position(o, o->count);

  i->icon = create_bitmap(icon->w, icon->h);
  blit(icon, i->icon, 0, 0, 0, 0, icon->w, icon->h);
  i->caption = strdup(caption);
  i->selected = FALSE;

  o->icons->insert(o->icons, i);

  if (o->scr) o->scr->change_max(o->scr, p.y+o->scr->pos);

  o->count += 1;

  if (p.y < -38+o->spacing.y || p.y > VIEW(o)->bounds.b.y-VIEW(o)->bounds.a.y)
    VIEW(o->scr)->draw_view(VIEW(o->scr));
  else
    VIEW(o)->draw_view(VIEW(o));
};

void bad_iconbox_clear(p_bad_iconbox o)
{
  o->icons->free_all(o->icons);
  o->count = 0;

  VIEW(o)->draw_view(VIEW(o));
};

void bad_iconbox_translate_event(p_object o, t_event *event)
{
  RETVIEW(o, event);
  view_translate_event(o, event);

  if ( o->phase == PH_PREPROCESS ) return;

  if (VIEW(o)->is_mouse_in_view(VIEW(o)))
  if ((event->type & EV_MOUSE) && ((OBJECT(mouse)->state & MO_SF_MOUSELDOWN) || (OBJECT(mouse)->state & MO_SF_MOUSELDOUBLE)))
  {
    p_bad_iconbox ib = ((p_bad_iconbox)(o));
    l_int i, idx;

    idx = -1;

    if (ib->count > 0) {
      l_int on_icon = FALSE;
      for (i=0;i<ib->count;i++) if (bad_iconbox_mouse_under_icon_at(ib, i)) {
        if (OBJECT(mouse)->state & MO_SF_MOUSELDOWN) { /* single click */
          p_bad_iconbox_item ii;
          idx = i;
          if (!(keyb->shifts & KB_CTRL_FLAG)) {
            int i;

            for (i=0;i<ib->count;i++) {
              ii = ib->icons->at(ib->icons, i);
              ii->selected = FALSE;
            };
          };
          ii = ib->icons->at(ib->icons, i);
          ii->selected = TRUE;
          ib->index = i;
          VIEW(o)->draw_view(VIEW(o));
          on_icon = TRUE;
        } else { /* double click */
          if (ib->on_dclick) ib->on_dclick(ib, i);
        };
        break;
      };

      if (!on_icon) {
        int i;
        p_bad_iconbox_item ii;
        t_point p = VIEW(o)->get_global_point(VIEW(o), point_assign(0, 0));

        ib->selection = rect_assign(mouse->where.x, mouse->where.y, mouse->where.x+5, mouse->where.y+5);
        VIEW(o)->draw_view(VIEW(o));

        for (i=0;i<ib->count;i++) {
          ii = ib->icons->at(ib->icons, i);
          ii->selected = FALSE;
        };
        do {
          if (OBJECT(mouse)->state & MO_SF_MOUSEMOVE) {
            ib->selection.b.x = mouse->where.x;
            ib->selection.b.y = mouse->where.y;
            if (ib->selection.b.x-ib->selection.a.x < 5) ib->selection.b.x = ib->selection.a.x+5;
            if (ib->selection.b.y-ib->selection.a.y < 5) ib->selection.b.y = ib->selection.a.y+5;

            for (i=0;i<ib->count;i++) {
              t_point pp = bad_iconbox_icon_position(ib, i);
              ii = ib->icons->at(ib->icons, i);
              pp.x += p.x;
              pp.y += p.y;
              ii->selected = (pp.x > ib->selection.a.x && pp.y > ib->selection.a.y && pp.x < ib->selection.b.x && pp.y < ib->selection.b.y);
            };

            VIEW(o)->draw_view(VIEW(o));
          };

          o->get_event(o, event);
        } while (OBJECT(mouse)->state & MO_SF_MOUSELPRESS);
        ib->selection = rect_assign(-1, -1, -1, -1);
        VIEW(o)->draw_view(VIEW(o));
      };
    };
    while (OBJECT(mouse)->state & MO_SF_MOUSELPRESS) o->get_event(o, event);
    clear_event(event);
  };
};

void bad_iconbox_on_scroll(p_bad_vscrollbar o, l_int newpos)
{
  if (OBJECT(o)->owner)
    VIEW(OBJECT(o)->owner)->draw_view(VIEW(OBJECT(o)->owner));
}

void bad_iconbox_change_bounds(p_view o, t_rect nr)
{
  p_bad_iconbox ib = ((p_bad_iconbox)(o));
  t_point p;
  view_change_bounds(o, nr);

  if (!ib->scr) return;
  p = bad_iconbox_icon_position(ib, ib->count);
  ib->scr->change_max(ib->scr, p.y+ib->scr->pos);
}

l_bool bad_iconbox_done(p_object o)
{
  p_bad_iconbox ib = BAD_ICONBOX(o);
  l_int i;
  if (!view_done(o)) return false;

  for (i=0;i<ib->count;i++) {
    p_bad_iconbox_item ii = ib->icons->at(ib->icons, i);
    destroy_bitmap(ii->icon);
    free(ii->caption);
  };

  dispose_list(&ib->icons, TRUE);

  return TRUE;
};

p_bad_iconbox _bad_iconbox_init(p_bad_iconbox o, t_rect r, l_big flags)
{
  if (!o) return NULL;

  clear_type(o, sizeof(t_bad_iconbox));

  view_init(&o->obclass, r);

  o->icons = list_init(malloc(sizeof(t_list)), &free, 0);
  o->caption_size = point_assign(64, 24);
  o->spacing = point_assign(43, 43);
  o->count = 0;
  o->index = -1;
  o->flags = flags;
  o->selection = rect_assign(-1, -1, -1, -1);

  o->on_dclick = NULL;
  o->add = &bad_iconbox_add;
  o->clear = &bad_iconbox_clear;

  VIEW(o)->draw = &bad_iconbox_draw;
  VIEW(o)->change_bounds = &bad_iconbox_change_bounds;
  VIEW(o)->set_palette(VIEW(o), pal_bad_common);
  VIEW(o)->brush.color = pal_bad_common[6];
  VIEW(o)->brush.color2 = pal_bad_common[6];
  VIEW(o)->brush.background = NULL;

  OBJECT(o)->translate_event = &bad_iconbox_translate_event;
  OBJECT(o)->done = &bad_iconbox_done;

  if (flags & BIB_VER_SCROLLBAR) {
    r = rect_assign(rect_sizex(r)-BAD_SCROLLBAR_WIDTH, 0, rect_sizex(r), rect_sizey(r));
    if (flags & BIB_3D_BORDER) {
      r.a.x -= 2;
      r.a.y += 2;
      r.b.x -= 2;
      r.b.y -= 2;
    };
    o->scr = bad_vscrollbar_init(malloc(sizeof(t_bad_vscrollbar)), r, 0);
    o->scr->max = 1;
    o->scr->pos = 0;
    o->scr->on_scroll = &bad_iconbox_on_scroll;
    VIEW(o->scr)->align |= TX_ALIGN_BOTTOM|TX_ALIGN_RIGHT|TX_ALIGN_FIXEDX;
    OBJECT(o)->insert(OBJECT(o), OBJECT(o->scr));
  };

  return o;
};

void bad_vscrollbar_draw(p_view o)
{
  p_bad_vscrollbar sb = BAD_VSCROLLBAR(o);
  t_rect r = o->get_local_extent(o);
  t_point p;
  BITMAP *out = o->begin_paint(VIEW(o), &p, r);

  if (out) {
    rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, o->get_color(o, 0));
    light_image ( out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, 20, 20, 20);
    if (!(sb->flags & BSB_NO_3D_BORDER))
      button(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, o->get_color(o, 2), o->get_color(o, 1));
#ifdef USE_SKIN_CODE
    if (!SKINFOR(o)) {
#endif
      rectfill(out, r.a.x+p.x+1, r.a.y+p.y+sb->bar_pos, r.b.x+p.x-1, r.a.y+p.y+sb->bar_pos+sb->delta-1, o->get_color(o, 0));

      draw_bevel(out, r.a.x+p.x, r.a.y+p.y+sb->bar_pos, r.b.x+p.x, r.a.y+p.y+sb->bar_pos+sb->delta-1, o->get_color(o, 0), o->get_color(o, 3));
      draw_bevel(out, r.a.x+p.x+1, r.a.y+p.y+sb->bar_pos+1, r.b.x+p.x-1, r.a.y+p.y+sb->bar_pos+sb->delta-2, o->get_color(o, 1), o->get_color(o, 2));
#ifdef USE_SKIN_CODE
    } else { /* skin */
      DRAW_SKIN_RECT(out, SKINFOR(o), "ScrollBar", rect_assign(r.a.x+p.x+1, r.a.y+p.y+sb->bar_pos, r.b.x+p.x-1, r.a.y+p.y+sb->bar_pos+sb->delta-1));
    };
#endif
  };

  r.b.x += 15;

  o->end_of_paint(o, r);
}

void bad_vscrollbar_change_pos(p_bad_vscrollbar o, l_int newpos)
{
  l_int old_bar_pos = o->bar_pos;
  o->pos = newpos;
  o->bar_pos = newpos*(rect_sizey(VIEW(o)->bounds)-o->delta)/lmax(o->max, 1);

  if (old_bar_pos != o->bar_pos) VIEW(o)->draw_view(VIEW(o));
}

void bad_vscrollbar_change_max(p_bad_vscrollbar o, l_int newmax)
{
  l_int old_bar_pos = o->bar_pos;
  l_int old_delta = o->delta;
  o->max = newmax;
  if (o->max <= rect_sizey(VIEW(o)->bounds))
    o->delta = rect_sizey(VIEW(o)->bounds);
  else
    o->delta = lmax(BAD_SCROLLBAR_MIN_SIZE, rect_sizey(VIEW(o)->bounds)*rect_sizey(VIEW(o)->bounds)/lmax(o->max, 1));
  o->bar_pos = o->pos*(rect_sizey(VIEW(o)->bounds)-o->delta)/lmax(o->max, 1);

  if (old_bar_pos != o->bar_pos || old_delta != o->delta) VIEW(o)->draw_view(VIEW(o));
}

void bad_vscrollbar_change_bounds(p_view o, t_rect nr)
{
  view_change_bounds(o, nr);

  if (BAD_VSCROLLBAR(o)->max <= rect_sizey(nr))
    BAD_VSCROLLBAR(o)->delta = rect_sizey(nr);
  else
    BAD_VSCROLLBAR(o)->delta = lmax(BAD_SCROLLBAR_MIN_SIZE, rect_sizey(o->bounds)*rect_sizey(nr)/lmax(BAD_VSCROLLBAR(o)->max, 1));
  BAD_VSCROLLBAR(o)->bar_pos = BAD_VSCROLLBAR(o)->pos*(rect_sizey(nr)-BAD_VSCROLLBAR(o)->delta)/lmax(BAD_VSCROLLBAR(o)->max, 1);

  VIEW(o)->draw_view(VIEW(o));
}

void bad_vscrollbar_translate_event(p_object o, t_event *event)
{
  p_bad_vscrollbar sb = BAD_VSCROLLBAR(o);
  RETVIEW(o, event);
  view_translate_event(o, event);

  if ((event->type & EV_MOUSE) && (OBJECT(mouse)->state & MO_SF_MOUSELDOWN))
  {
    t_point p = VIEW(o)->get_global_point(VIEW(o), point_assign(0, 0));
    t_rect r = VIEW(o)->get_local_extent(VIEW(o));
    l_long my = (r.b.y-r.a.y-sb->delta), sy = (mouse->where.y-p.y)-sb->bar_pos;
    l_long first_pos = sb->pos;
    l_bool in_view;

    if (mouse->where.y >= p.y+sb->bar_pos && mouse->where.y < p.y+sb->bar_pos+sb->delta && sb->max > rect_sizey(r))
    while (OBJECT(mouse)->state & MO_SF_MOUSELPRESS)
    {
      l_bool old_in_view = in_view;
      l_long old_pos = sb->pos;
      in_view = VIEW(o)->is_mouse_in_view(VIEW(o));

      sb->pos = ((mouse->where.y)-p.y-sy)*sb->max/my;
      if (sb->pos > sb->max) sb->pos = sb->max; else
      if (sb->pos < 0) sb->pos = 0;
      sb->bar_pos = sb->pos*(rect_sizey(r)-sb->delta)/lmax(sb->max, 1);

      if (sb->pos != old_pos) {
        VIEW(o)->draw_view(VIEW(o));
        if (sb->on_scroll) sb->on_scroll(sb, sb->pos);
      };

      o->get_event(o, event);
    };

    clear_event(event);
  };
}

p_bad_vscrollbar _bad_vscrollbar_init(p_bad_vscrollbar o, t_rect r, l_int flags)
{
  if (!o) return NULL;
  clear_type(o, sizeof(t_bad_vscrollbar));
  view_init(&o->obclass, r);

  o->max = 100;
  o->change = 5;

  o->delta = lmax(BAD_SCROLLBAR_MIN_SIZE, rect_sizey(r)*rect_sizey(r)/lmax(o->max, 1));
  o->bar_pos = o->pos*(rect_sizey(r)-o->delta)/lmax(o->max, 1);

  o->flags = flags;

  o->change_pos = &bad_vscrollbar_change_pos;
  o->change_max = &bad_vscrollbar_change_max;

  VIEW(o)->draw = &bad_vscrollbar_draw;
  VIEW(o)->change_bounds = &bad_vscrollbar_change_bounds;
  VIEW(o)->set_palette(VIEW(o), pal_bad_common);

  OBJECT(o)->translate_event = &bad_vscrollbar_translate_event;

  return o;
}

#include "badctrls.exp"

lib_begin(void)
{
  if (ap_process == AP_ALLOC)
  {
    AP_EXPORTLIB();
  } else
  if (ap_process == AP_INIT)
  {
    AP_SETNUMOFCALLS(1);
    dat = conv_to_skipcolor_data(DLXGetFileData(ap_id), CO_SKIP_RED, CO_SKIP_GREEN, CO_SKIP_BLUE);

    pal_bad_common[0] = registry_color("3d_face");
    pal_bad_common[1] = registry_color("3d_light");
    pal_bad_common[2] = registry_color("3d_shadow");
    pal_bad_common[3] = registry_color("3d_dark");
    pal_bad_common[4] = registry_color("3d_border");
    pal_bad_common[5] = registry_color("text");
    pal_bad_common[6] = registry_color("flat_background");
    pal_bad_common[7] = registry_color("flat_border");
    pal_bad_common[8] = registry_color("highlight");
    pal_bad_common[9] = registry_color("highlight_text");
    pal_bad_common[10] = registry_color("iconbox_text");
    pal_bad_common[11] = registry_color("filebox");
    pal_bad_common[12] = registry_color("filebox_text");
/*    pal_beditor[0] = registry_color("flat_background");
    pal_beditor[1] = registry_color("3d_light");
    pal_beditor[2] = registry_color("3d_shadow");
    pal_beditor[3] = registry_color("3d_dark");
    pal_beditor[4] = registry_color("3d_flat");
    pal_beditor[5] = registry_color("edit_text");*/
  };
} lib_end;

