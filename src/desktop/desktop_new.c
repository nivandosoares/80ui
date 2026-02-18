/*
   The Desktop (r1)
   Copyright (c) 2002 David Fredin. All Rights Reserved.

   Web:    http://software.xduffy.com
   E-mail: athanatos@home.se
*/

/******************************************************************
 * SEAL 2.0                                                       *
 * Copyright (c) 1999-2002 SEAL Developers. All Rights Reserved.  *
 *                                                                *
 * Web site: http://sealsystem.sourceforge.net/                   *
 * E-mail (current maintainer): orudge@users.sourceforge.net      *
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

/***********************************************************/
/*                    Copyright (C) 2001                   */
/*          by Kostas Michalopoulos aka Bad Sector         */
/***********************************************************/

/* Revision History (as of 10/04/2002):
 *
 * 10/04/2002: Updated for SEAL 2.00.11 (orudge)
 * 11/04/2002: Continued updating (orudge)
 * 15/05/2002: Icon loading re-done. Now loads from vfs. (xduffy)
 * 17/05/2002: The icons are now of the type t_dicon (xduffy)
 * 22/05/2002: Announced my modified Desktop as it's own project
               "The Desktop". (xduffy)
 */

#include <dos.h>
#include <allegro.h>
#include <seal.h>
#include <view.h>
#include <menus.h>
#include <button.h>
#include <app.h>
#include <unistd.h>
#include <dir.h>
#include <safmem.h>
#include <math.h>
#include <dataini.h>
#include <badctrls.h>
#include <lnk.h>
#include "registry.h"
#include "desktop.h"
#include "fileext.h"
#include "dicon.h"
#include <sys/ioctl.h>
#include "diconman.h"

extern void create_clock();

#ifndef SetInfoDescription   // Only exists in 2.00.11 and higher
   #define SetInfoDescription(x)  SetInfoDesciption(x)
#endif

/* indicates if it can be drawed under desktop line. 1 = it is, 0 = it isn't  */
l_int            draw_under_line = 1;

l_bool           taskbar_alone = 1;

BITMAP *image_menu;

/* list of minimalized objects */
static p_list           desktopline_minimalized = NULL;

/* items of program menu */
static p_menuitem       program_items = NULL;

/* items of settigns menu */
static p_menuitem       settings_items = NULL;

/* items of documents menu */
static p_menuitem       docs_items = NULL;

/* items of help menu */
static p_menuitem       help_items = NULL;

/* desktop line */
static p_desktopline  desktopline = NULL;


/* init function for desktop line */
p_desktopline  (*desktopline_init) ( p_desktopline o ) = &_desktopline_init;



/* menu icons */
static DATAFILE *datfile = NULL;
#define ICO_GET(n) (BITMAP*)GET_DATA(datfile, n)
#define ICO_xbtStart 0


/* desktop menu about texts */

#define TXT_ABOUTPROGRAMS      INI_TEXT("")
#define TXT_ABOUTSETTINGS      INI_TEXT("")
#define TXT_ABOUTCLOSEWIN      INI_TEXT("")
#define TXT_ABOUTQUIT          INI_TEXT("")
#define TXT_ABOUTRUNAPP        INI_TEXT("")

#define  DIR_PROGRAMS  "./PROGRAMS"
#define  DIR_SETTINGS  "./SETTINGS"
#define  DIR_DOCS      "./DOCS"
#define  DIR_HELP      "./HELP"

#define  DIR_IGNORE  '$'


#define  MSG_REFRESHMINIMIZED  100109
#define  MSG_RUNAPP            100110
#define  MSG_xAPPLICATION      100111
#define  MSG_xABOUT            100112

#define  MSG_xRESTART          100120
#define  MSG_xREFRESH          100121
#define  MSG_xSLEEP            100122


#define  MSG_ICON_SORT_V       100123
#define  MSG_ICON_SORT_H       100124
#define  MSG_ICON_SORT_G       100125
#define  MSG_ICON_OPEN         100126
#define  MSG_ICON_PREFS        100127

#define  ID_SIZE               23  /* desktop icon size */
                       
static p_menuitem dicons_menuitems()
{
   return new_menu_item_ex("Open", NULL, NULL, MSG_ICON_OPEN,
                              true, NULL, NULL, NULL, NULL, NULL, 0,
          new_menu_item_ex("Sort icons h", NULL, NULL, MSG_ICON_SORT_H,
                              true, NULL, NULL, NULL, NULL, NULL, 0,
          new_menu_item_ex("Sort icons v", NULL, NULL, MSG_ICON_SORT_V,
                              true, NULL, NULL, NULL, NULL, NULL, 0,
          new_menu_item_ex("Gridify icons", NULL, NULL, MSG_ICON_SORT_G,
                              true, NULL, NULL, NULL, NULL, NULL, 0,
          new_menu_item_ex("Preferences", NULL, NULL, MSG_ICON_PREFS,
                              true, NULL, NULL, NULL, NULL, NULL, 0, NULL)))));
}

void popup_go_menu(p_dicon o)
{
  t_rect icon_space = VIEW(desktop)->get_limits(VIEW(desktop));
  t_rect r = rect_assign(mouse->where.x,mouse->where.y,mouse->where.x,mouse->where.y);
  t_event event;
  p_menu m = new_menu(dicons_menuitems());
  p_menuview menu;
  l_dword msg = NULL;

   if(m)
      menu = menuview_init(malloc(sizeof(t_menuview)), r, m);

   if(menu)
      msg = desktop->execute_view(desktop, VIEW(menu));

   if(msg == MSG_ICON_SORT_V || msg == MSG_ICON_SORT_H || msg == MSG_ICON_SORT_G)
   {
      VIEW(menu)->hide(VIEW(menu));
      if(msg == MSG_ICON_SORT_V)
         sort_dicons_in_object(OBJECT(desktop), true, icon_space);
      else if(msg == MSG_ICON_SORT_H)
         sort_dicons_in_object(OBJECT(desktop), false, icon_space);
      else
         gridify_dicons_in_object(OBJECT(desktop), icon_space);
   }
   else if(msg == MSG_ICON_OPEN)
   {
      run_file(o->filename);
   }
   else if(msg == MSG_ICON_PREFS)
   {
      init_preferences(o);
   }
}

/* t_taskbar structure */

/*
0 3d face
1 3d light
2 3d shadow
3 3d dark
4 task button face
5 task button light
6 task button shadow
7 task button dark
8 task button caption
9 selected task button caption
10 selected task button face
*/

l_color pal_taskbar[] = {CO_LIGHTGRAY, CO_WHITE, CO_DARKGRAY, CO_LIGHTGRAY, CO_WHITE, CO_DARKGRAY, CO_BLACK, CO_BLACK, CO_BLACK, CO_BLUE, CO_NOCOLOR};

typedef struct t_taskbar *p_taskbar;

typedef struct t_taskbar {

  struct t_view obclass;

  l_long old_windows;
  p_object old_first;

} t_taskbar;

l_long taskbar_windows()
{
  l_long r = 0;
  p_object o = OBJECT(desktop)->first_view(OBJECT(desktop));
  p_object f = o;
  do
  {
    if (o->tag & TAG_WINDOW) r++;
    o = OBJECT(OBJECT(o)->next_view(OBJECT(o)));
  } while ((o != f) || (r > 64));  //for safety (and because i don't think that a taskbar can contain 64 buttons

  return r;
};


void taskbar_callback(p_object o)
{
  l_long windows = taskbar_windows();
  if (windows != ((p_taskbar)(o))->old_windows)
  {
    ((p_taskbar)(o))->old_windows = windows;
    VIEW(o)->draw(VIEW(o));
  } else if (((p_taskbar)(o))->old_first != OBJECT(OBJECT(desktop)->first_view(OBJECT(desktop))))
  {
    ((p_taskbar)(o))->old_first = OBJECT(OBJECT(desktop)->first_view(OBJECT(desktop)));
    VIEW(o)->draw(VIEW(o));
  }
}

void taskbar_translate_event(p_object o, t_event *event)
{
  RETVIEW(o, event);
  if ((event->type & EV_MOUSE) && (OBJECT(mouse)->state & MO_SF_MOUSELDOWN))
  {
    l_long windows = taskbar_windows();
    t_rect r = VIEW(o)->get_local_extent(VIEW(o));
    t_point p = VIEW(o)->get_global_point(VIEW(o), point_assign(0, 0));
    if (windows > 0)
    {

      l_int btn_size = ((r.b.x-r.a.x)-4)/windows;
      l_int btn_x = 2;
      p_object z = OBJECT(desktop)->first_view(OBJECT(desktop));
      p_object f = z;

      if (btn_size > 160) btn_size = 160;
      do {
        if (z->tag & TAG_WINDOW)
        {
          t_rect rr = rect_assign(btn_x+p.x, 2+p.y, btn_x+btn_size-4+p.x, r.b.y-2+p.y);

          if (mouse->where.x >= rr.a.x && mouse->where.x <= rr.b.x && mouse->where.y >= rr.a.y && mouse->where.y <= rr.b.y )
          {
            l_bool in_btn = true;
            BITMAP *out = VIEW(o)->begin_paint(VIEW(o), &p, r);
            button(out, rr.a.x+p.x, rr.a.y+p.y, rr.b.x+p.x, rr.b.y+p.y, VIEW(o)->get_color(VIEW(o), 6), VIEW(o)->get_color(VIEW(o), 5));
            rect(out, rr.a.x+p.x+1, rr.a.y+p.y+1, rr.b.x+p.x-1, rr.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 4));
            VIEW(o)->end_of_paint(VIEW(o), r);
            while (OBJECT(mouse)->state & MO_SF_MOUSELPRESS)
            {
              l_bool safe_in_btn = in_btn;
              in_btn = (mouse->where.x >= rr.a.x && mouse->where.x <= rr.b.x && mouse->where.y >= rr.a.y && mouse->where.y <= rr.b.y );

              if (in_btn != safe_in_btn)
              {
                out = VIEW(o)->begin_paint(VIEW(o), &p, r);
                if (in_btn) {
                  button(out, rr.a.x+p.x, rr.a.y+p.y, rr.b.x+p.x, rr.b.y+p.y, VIEW(o)->get_color(VIEW(o), 6), VIEW(o)->get_color(VIEW(o), 5));
                  rect(out, rr.a.x+p.x+1, rr.a.y+p.y+1, rr.b.x+p.x-1, rr.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 4));
                } else {
                  button(out, rr.a.x+p.x, rr.a.y+p.y, rr.b.x+p.x, rr.b.y+p.y, VIEW(o)->get_color(VIEW(o), 5), VIEW(o)->get_color(VIEW(o), 7));
                  button(out, rr.a.x+p.x+1, rr.a.y+p.y+1, rr.b.x+p.x-1, rr.b.y+p.y-1, VIEW(o)->get_color(VIEW(o), 4), VIEW(o)->get_color(VIEW(o), 6));
                };
                VIEW(o)->end_of_paint(VIEW(o), r);
              };

              o->get_event(o, event);
            };

            if (in_btn)
            {
              VIEW(z)->show(VIEW(z));
              z->select(z);
              WINDOW(z)->flags -= WF_MINSIZE;
              VIEW(o)->draw(VIEW(o));
            };

          };

          btn_x += btn_size+2;
        };
        z = OBJECT(OBJECT(z)->next_view(OBJECT(z)));
      } while (z != f);
    };
  };
  if ((event->type & EV_MOUSE) && (OBJECT(mouse)->state & MO_SF_MOUSERDOWN))
  {
    l_long windows = taskbar_windows();
    t_rect r = VIEW(o)->get_local_extent(VIEW(o));
    t_point p = VIEW(o)->get_global_point(VIEW(o), point_assign(0, 0));
    if (windows > 0)
    {

      l_int btn_size = ((r.b.x-r.a.x)-4)/windows;
      l_int btn_x = 2;
      p_object z = OBJECT(desktop)->first_view(OBJECT(desktop));
      p_object f = z;

      if (btn_size > 160) btn_size = 160;
      do {
        if (z->tag & TAG_WINDOW)
        {
          t_rect rr = rect_assign(btn_x+p.x, 2+p.y, btn_x+btn_size-4+p.x, r.b.y-2+p.y);

          if (mouse->where.x >= rr.a.x && mouse->where.x <= rr.b.x && mouse->where.y >= rr.a.y && mouse->where.y <= rr.b.y )
          {
            p_menu m = new_menu(APPWIN(z)->menu);
            p_menuview menu;
            t_rect mr = desktop->get_local_extent(VIEW(o));
            mr.a.x = mouse->where.x+1;
            mr.a.y = mouse->where.y+1;
            if (m) menu = menuview_init(malloc(sizeof(t_menuview)), mr, m);
            if (menu) {
              l_int msg = desktop->execute_view(desktop, VIEW(menu));
              set_event_info(event, EV_MESSAGE, msg, z, NULL);
              z->put_event(z, event);
              clear_event(event);
            };

            break;
          };

          btn_x += btn_size+2;
        };
        z = OBJECT(OBJECT(z)->next_view(OBJECT(z)));
      } while (z != f);
    };
  };
};

void taskbar_draw(p_view o)
{
  t_rect r = o->get_local_extent(o);
  t_point p;
  BITMAP *out = o->begin_paint(o, &p, r);

  if (out) {
    l_long windows = taskbar_windows();
    rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, o->get_color(o, 0));
    if (taskbar_alone) {
      button(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, o->get_color(o, 1), o->get_color(o, 2));
    };

    if (windows > 0)
    {
      l_int btn_size = ((r.b.x-r.a.x)-4)/windows;
      l_int btn_x = 2;
      p_object z = OBJECT(desktop)->first_view(OBJECT(desktop));
      p_object f = z;

      if (taskbar_alone) {
        if (btn_size > 160) btn_size = 160;
      } else {
        if (btn_size > 120) btn_size = 120;
      };


      do {
        if (z->tag & TAG_WINDOW)
        {
          t_rect rr = rect_assign(btn_x, 2, btn_x+btn_size-4, r.b.y-2);

          if (z->state & OB_SF_FOCUSED)
          {
            rectfill(out, rr.a.x+p.x, rr.a.y+p.y, rr.b.x+p.x, rr.b.y+p.y, o->get_color(o, 10));
            button(out, rr.a.x+p.x, rr.a.y+p.y, rr.b.x+p.x, rr.b.y+p.y, o->get_color(o, 7), o->get_color(o, 5));
            button(out, rr.a.x+p.x+1, rr.a.y+p.y+1, rr.b.x+p.x-1, rr.b.y+p.y-1, o->get_color(o, 6), o->get_color(o, 4));
            if (z->tag & TAG_APPWIN) draw_sprite(out, APPWIN(z)->icon16, rr.a.x+p.x+3, rr.a.y+p.y+2);
            textout_draw_rect(out, o->font, WINDOW(z)->caption, -1, rr.a.x+p.x+22, rr.a.y+p.y+4, rr.b.x+p.x-2, rr.b.y+p.y-2, TX_ALIGN_LEFT|TX_ALIGN_CENTERY, o->get_color(o, 9), TX_NOCOLOR, 1);
            textout_draw_rect(out, o->font, WINDOW(z)->caption, -1, rr.a.x+p.x+23, rr.a.y+p.y+4, rr.b.x+p.x-1, rr.b.y+p.y-2, TX_ALIGN_LEFT|TX_ALIGN_CENTERY, o->get_color(o, 9), TX_NOCOLOR, 1);
          } else {
            rectfill(out, rr.a.x+p.x, rr.a.y+p.y, rr.b.x+p.x, rr.b.y+p.y, o->get_color(o, 4));
            button(out, rr.a.x+p.x, rr.a.y+p.y, rr.b.x+p.x, rr.b.y+p.y, o->get_color(o, 5), o->get_color(o, 7));
            button(out, rr.a.x+p.x+1, rr.a.y+p.y+1, rr.b.x+p.x-1, rr.b.y+p.y-1, o->get_color(o, 4), o->get_color(o, 6));
            if (z->tag & TAG_APPWIN) draw_sprite(out, APPWIN(z)->icon16, rr.a.x+p.x+2, rr.a.y+p.y+1);
            textout_draw_rect(out, o->font, WINDOW(z)->caption, -1, rr.a.x+p.x+21, rr.a.y+p.y+3, rr.b.x+p.x-3, rr.b.y+p.y-3, TX_ALIGN_LEFT|TX_ALIGN_CENTERY, o->get_color(o, 8), TX_NOCOLOR, 1);
          };

          btn_x += btn_size+2;
        };
        z = OBJECT(OBJECT(z)->next_view(OBJECT(z)));
      } while (z != f);
    };
  };

  o->end_of_paint(o, r);
};

p_taskbar _taskbar_init(p_taskbar o, t_rect r)
{
  if (!o) return NULL;

  clear_type(o, sizeof(t_taskbar));

  view_init(&o->obclass, r);

  o->old_windows = 0;
  o->old_first = NULL;

  OBJECT(o)->func_callback = &taskbar_callback;
  OBJECT(o)->translate_event = &taskbar_translate_event;

  VIEW(o)->draw = &taskbar_draw;

  VIEW(o)->set_palette(VIEW(o), pal_taskbar);

  return o;
};

p_taskbar (*taskbar_init)(p_taskbar o, t_rect r) = &_taskbar_init;

p_taskbar taskbar = NULL;

/* t_userbox structure */

typedef struct t_userbox *p_userbox;

typedef struct t_userbox {

  struct t_view obclass;

} t_userbox;

void userbox_draw(p_view o)
{
  t_rect r = o->get_local_extent(o);
  t_point p;
  BITMAP *out = o->begin_paint(o, &p, r);

  if (out) {
    rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, o->get_color(o, 0));
    button(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, o->get_color(o, 2), o->get_color(o, 1));
  };

  o->end_of_paint(o, r);
};

p_userbox _userbox_init(p_userbox o, t_rect r)
{
  if (!o) return NULL;

  clear_type(o, sizeof(t_userbox));

  view_init(&o->obclass, r);

  VIEW(o)->draw = &userbox_draw;

  VIEW(o)->set_palette(VIEW(o), pal_taskbar); // bored to make new palette ;)

  return o;
};

p_userbox (*userbox_init)(p_userbox o, t_rect r) = &_userbox_init;

p_userbox userbox = NULL;

void refresh_userbox()
{
  p_object o = OBJECT(userbox)->first_view(OBJECT(userbox));
  p_object f = o;
  t_rect b = rect_assign(desktop->bounds.b.x-7, 2, desktop->bounds.b.x-4, 23);
  t_rect r;
  l_int x;
  l_int d;

  x = 2;

  if (f) o = OBJECT(OBJECT(o)->next_view(OBJECT(o)));

  if (o) do
  {
    r = VIEW(o)->bounds;
    d = VIEW(o)->bounds.b.y-VIEW(o)->bounds.a.y;
    r.a.y = (b.b.y-b.a.y)/2-d/2;
    r.b.y = r.a.y+d;
    d = VIEW(o)->bounds.b.x-VIEW(o)->bounds.a.x;
    r.a.x = x;
    r.b.x = x+d;
    VIEW(o)->change_bounds(VIEW(o), r);

    x += d+2;

    o = OBJECT(OBJECT(o)->next_view(OBJECT(o)));
  } while (o != f);

  if (f)
  {
    r = VIEW(o)->bounds;
    d = VIEW(o)->bounds.b.y-VIEW(o)->bounds.a.y;
    r.a.y = (b.b.y-b.a.y)/2-d/2;
    r.b.y = r.a.y+d;
    d = VIEW(o)->bounds.b.x-VIEW(o)->bounds.a.x;
    r.a.x = x;
    r.b.x = x+d;
    VIEW(o)->change_bounds(VIEW(o), r);

    x += d+2;
  };

  b = rect_assign(desktop->bounds.b.x-7-x, 2, desktop->bounds.b.x-4, 23);
  VIEW(userbox)->change_bounds(VIEW(userbox), b);
  if (!taskbar_alone) {
    b = VIEW(taskbar)->bounds;
    b.b.x = VIEW(userbox)->bounds.a.x-10;
    VIEW(taskbar)->change_bounds(VIEW(taskbar), b);
  };
};

void add_to_userbox(p_object o)
{
  obj_insert(OBJECT(userbox), o);

  refresh_userbox();
};

void remove_from_userbox(p_object o)
{
  obj_remove(OBJECT(userbox), o);

  refresh_userbox();
};

/* t_minmenuitem structure */

typedef struct t_minmenuitem *p_minmenuitem;

typedef struct t_minmenuitem {

  struct t_menuitem  obclass;

  t_appinfo          appinfo;

} t_minmenuitem;


/* t_filemenuitem structure */

typedef struct t_filemenuitem *p_filemenuitem;

typedef struct t_filemenuitem {

  struct t_menuitem  obclass;

  l_text             filename;

} t_filemenuitem;


#define new_sub_filemenu(name,f,menu,icon,next) \
                                new_sub_filemenu_ex(name, true, NULL, NULL, icon, NULL, 0, f, menu, next)
#define new_filemenu_item(name,param,hk,msg,it,f,icon,next) \
                                new_filemenu_item_ex(name, param, hk, msg, true, it, 0, NULL, icon, NULL, 0, f, next)
#define new_filemenu_check_item(name,param,is_check,hk,msg,it,f,next) \
                                new_filemenu_item_ex(name, param, hk, msg, true, it, MIF_CHECK+(is_check*MIF_CHECKOK), NULL, NULL, NULL, 0, f, next)

static p_filemenuitem  new_filemenu_item_ex ( l_text name, l_text param, l_int hotkey, l_dword message,
                               l_bool enable, l_text info_text, l_int flags,
                               l_font *font, BITMAP *icon, l_font *font_symbol, l_byte chr,
                               l_text filename, p_menuitem next )
{

  p_filemenuitem fi = (p_filemenuitem)sf_malloc(sizeof(t_filemenuitem));

  p_menuitem f = new_menu_item_ex(name, param, hotkey, message, enable, info_text, flags, font, icon, font_symbol, chr, next);

  if ( f && fi ) {

     memcpy(fi, f, sizeof(t_menuitem));

     fi->filename = _strdup(filename);

  };

  sf_free(f);

  return fi;

};

static p_filemenuitem  new_sub_filemenu_ex ( l_text name, l_bool enable, l_text info_text,
                              l_font *font, BITMAP *icon, l_font *font_symbol, l_byte chr,
                              l_text filename, p_menu menu, p_menuitem next )
{

  p_filemenuitem fi = (p_filemenuitem)sf_malloc(sizeof(t_filemenuitem));

  p_menuitem f = new_sub_menu_ex(name, enable, info_text, font, icon, font_symbol, chr, menu, next);

  if ( f && fi ) {

     memcpy(fi, f, sizeof(t_menuitem));

     fi->filename = _strdup(filename);

  };

  sf_free(f);

  return fi;

};

p_menuitem  menu_load_from_dir ( l_text path, l_text filename )
{

  struct t_ffblk f;
  p_filemenuitem item = NULL;
  p_menuitem first_menu = NULL;
  l_int done = 0;

  l_text fi = io_realpath(path, "*.*");

  done = io_findfirst(fi, &f, FA_DIREC);

  while ( !done ) {

    if ( f.info.ff_attrib & FA_DIREC && io_isfilename(f.info.ff_name) &&
         f.info.ff_name[0] != DIR_IGNORE ) {

        l_text alld = io_realpath(path, f.info.ff_name);

        if ( io_isdir(alld) ) {

            l_text path2 = io_realpath(path, f.info.ff_name);

            p_menuitem it = path2?menu_load_from_dir(path2, filename):NULL;

            item = new_sub_filemenu(get_the_file_text(f.info.ff_name, path), alld, new_menu(it), get_the_file_ico(f.info.ff_name, path), NULL);

            first_menu = add_menu_item_to_menuitem(first_menu, (p_menuitem)item);

            sf_free(path2);

        };

        sf_free(alld);

    };

    done = io_findnext(&f);

  };

  sf_free(fi);

  fi = io_realpath(path, filename);

  done = io_findfirst(fi, &f, FA_ARCH);

  while ( !done ) {

        if ( f.info.ff_attrib & FA_ARCH ) {

          l_text allf = io_realpath(path, f.info.ff_name);

          if (get_the_file_visible(f.info.ff_name, path))
          if ( io_isfile(allf) ) { /* it's file */

            l_int mymem = 0;

            BITMAP *icon = get_the_file_ico(f.info.ff_name, path);

            item = new_filemenu_item_ex(get_the_file_text(f.info.ff_name, path), NULL, 0, MSG_RUNAPP,
                                    true, TXT_ABOUTRUNAPP, mymem?MIF_SELFICON:0, NULL, icon,
                                    NULL, 0, allf, NULL);

            first_menu = add_menu_item_to_menuitem(first_menu, (p_menuitem)item);

          };

          sf_free(allf);

        };

        done = io_findnext(&f);
  };

  sf_free(fi);

  return first_menu;

};


static p_menuitem  get_quit_items ( p_menuitem next )
{

   p_menuitem p =
//                new_menu_item(TXT_REFRESHDESKTOP, 0, 0, MSG_xREFRESH, NULL,
//                new_menu_item(TXT_RESTART, 0, 0, MSG_xRESTART, NULL,
//                new_menu_line(
                new_menu_item(TXT_SHUTDOWN, 0, 0, MSG_QUIT, NULL,
   next);

   return p;

};


static p_menuitem  get_programs_items ( void )
{

   if ( !program_items ) program_items = menu_load_from_dir(DIR_PROGRAMS, "*.*");

   return program_items;

};


static p_menuitem  get_settings_items ( void )
{

   if ( !settings_items ) settings_items = menu_load_from_dir(DIR_SETTINGS, "*.*");

   return settings_items;

};


static p_menuitem  get_document_items ( void )
{

   if ( !docs_items ) docs_items = menu_load_from_dir(DIR_DOCS, "*.*");

   return docs_items;

};


static p_menuitem  get_help_items ( void )
{

   if ( !help_items ) help_items = menu_load_from_dir(DIR_HELP, "*.*");

   return help_items;

};

static p_menuitem get_startmenu_items(void)
{
  p_menuitem p = new_sub_menu("Programs", new_menu(get_programs_items()),
                 new_sub_menu("Settings", new_menu(get_settings_items()),
                 new_sub_menu("Documents", new_menu(get_document_items()),
                 new_sub_menu("Help", new_menu(get_help_items()),
                 new_menu_line(
                 new_menu_item("About The Desktop", 0, 0, MSG_xABOUT, NULL,
                 new_menu_line(
                 new_menu_item("Sleep", "Shift+F12", TO_SHIFT(KB_F12), MSG_xSLEEP, NULL,
                 new_menu_item("Shut down", 0, 0, MSG_QUIT, NULL,
  NULL)))))))));

  return p;
};

/*
  translate application menu, that return right application that was

  pushed in application menu.
*/

void   filemenu_trans ( p_object o, p_event event )
{
       l_dword msg = event->message;

       if ( msg != MSG_NOTHING && msg != MSG_CANCEL ) {

          if ( msg == MSG_RUNAPP ) { /* run application was pressed */

              p_filemenuitem i = (p_filemenuitem)menu_get_lastitem_called(MENUVIEW(o)->menu);

              clear_event(event);

              if ( i ) {

                  if ( !run_file(i->filename) )  /* run application */

                      seal_error(ERR_INFO, "%s \n\n%s", TXT_CANTRUNFILE, i->filename);  /* error in application running */

              };

          } else { /* other message */

/*               set_event(event, EV_MESSAGE, msg, o);

               OBJECT(desktop)->put_event(o, event);

               clear_event(event);*/

          };

       };
};




/* t_desktopline */

t_rect  desktop_size_limits ( p_view o ) /* !!! redefine desktop size_limits */
{

  return rect_assign(0, ID_SIZE+2, rect_sizex(o->bounds), rect_sizey(o->bounds));

};


t_rect  desktopline_size_limits ( p_view o )
{

  return rect_assign(1, 1, rect_sizex(o->bounds)-1, rect_sizey(o->bounds)-1);

};

void    desktopline_draw ( p_view o )
{
  t_rect  r = o->get_local_extent(o);
  t_point p;

  BITMAP *out = o->begin_paint(o, &p, r);

  if ( out ) {

    rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, pal_taskbar[0]);
//    o->background(o, out, rect_move(r, p.x, p.y));
    button(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, pal_taskbar[1], pal_taskbar[2]);

  };

  o->end_of_paint(o, r);
};

/* t_desktopline init */

p_desktopline  _desktopline_init ( p_desktopline o )
{
  t_rect r = desktop->size_limits(desktop);

  if ( !o ) return NULL;

  r.a.y = r.b.y-ID_SIZE-2;

  clear_type(o, sizeof(t_desktopline));

  view_init(VIEW(o), r);

  /* object's functions */

//  OBJECT(o)->remove = &desktopline_remove;
//  OBJECT(o)->insert = &desktopline_insert;


  /* view's functions */

  VIEW(o)->size_limits = &desktopline_size_limits;
  VIEW(o)->draw = &desktopline_draw;


  /* desktopline's functions */

//  o->refresh_place = &desktopline_refresh_place;

  /* desktop limits redeclaration */

  if ( draw_under_line )

      desktop->size_limits = &desktop_size_limits; /* redefine desktop size limits */

  OBJECT(o)->set_options(OBJECT(o), VW_OF_IGNORELIM, true);
  OBJECT(o)->set_options(OBJECT(o), OB_OF_SELECTABLE, false);

  return o;

}


void  desktopline_translate_event ( p_object o, p_event event )
{
  RETVIEW(o, event);

  view_translate_event(o, event); /* call old function */

/*  if ( event->type == EV_KEYBOARD ) {
    if (keyb->code == TO_SHIFT(KB_F12)) {
      union REGS r;
      r.w.ax = 0x04F10;
      r.h.bl = 0x01;
      r.h.bh = 0x04;
      int86(0x010, &r, &r);
      msgbox(MW_INFO, MB_OK, "Press enter to return");
      r.w.ax = 0x04F10;
      r.h.bl = 0x01;
      r.h.bh = 0x00;
      int86(0x010, &r, &r);
    };
  };*/

  if ( event->type == EV_INFO )

     switch ( event->message ) {

        case MSG_MINIMIZE : if ( event->info ) {

            p_object  m = ((p_appinfo)(event->info))->obj;

            clear_event(event);

            if ( m )
            if (WINDOW(m)->flags & WF_MINSIZE) {
              VIEW(m)->show(VIEW(m));
              OBJECT(m)->select(OBJECT(m));
              VIEW(m)->draw(VIEW(m));
              WINDOW(m)->flags -= WF_MINSIZE;
            } else {
              VIEW(m)->hide(VIEW(m));
              WINDOW(m)->flags |= WF_MINSIZE;
              VIEW(taskbar)->draw(VIEW(taskbar));
            };


        }; break;
     };

  if ( event->type == EV_MESSAGE )

     switch ( event->message ) {

        case MSG_xAPPLICATION : {

            t_rect r = VIEW(o)->bounds;
            l_dword msg;

            p_menu m = new_menu(get_startmenu_items());

            p_menuview menu;

            clear_event(event);

            r.a.y = r.a.y-161;

            if ( m )

               menu = menuview_init(malloc(sizeof(t_menuview)), r, m);

            if ( menu ) {
              msg = desktop->execute_view(desktop, VIEW(menu));

              switch (msg) {
                case MSG_xABOUT : {
                  seal2_app_about("The Desktop", load_image("/system/bmp/desktop.ico,32"));
//                  msgbox(MW_INFO, MB_OK, "A-B Desktop\nversion\n\n(c) 2001,2002 Kostas Michalopoulos aka BadSector");
                } break;
                case MSG_xSLEEP : {
                  union REGS r;
                  r.w.ax = 0x04F10;
                  r.h.bl = 0x01;
                  r.h.bh = 0x04;
                  int86(0x010, &r, &r);
                  msgbox(MW_INFO, MB_OK, "Press enter to return");
                  r.w.ax = 0x04F10;
                  r.h.bl = 0x01;
                  r.h.bh = 0x00;
                  int86(0x010, &r, &r);
                } break;
                case MSG_QUIT : {
                  set_event(event, EV_MESSAGE, msg, OBJECT(desktop));

                  OBJECT(desktop)->put_event(OBJECT(desktop), event);

                  clear_event(event);
                } break;
                default: {
                  event->message = msg;
                  filemenu_trans(OBJECT(menu), event);
                } break;
              };
            };

        }; break;

     };

};

p_object desktopline_genbutton ( l_int ico, l_text info, l_dword msg )
{
  t_rect   r = rect_assign(3, 3, 62, 22);

  /* init new button (OPEN) */
  p_object b = OBJECT(icobutton_init(
                            _malloc(sizeof(t_icobutton)), /* memory for class t_button */
                            r, /* button area */
                            image_menu,
                            info,
                            msg,    /* message of the button */
                            BF_PUSH)     /* flags of the button */
               );

  if ( b ) {

     VIEW(b)->brush.color = COLOR(CO_BLACK);

  };

  return b;
};

/*
   if program is quit, it calls this routine. It check for validity and

   return ZERO if it's able to quit Seal, otherwise it return NON-ZERO.
*/

l_dword    program_valid ( p_object o, l_dword msg )
{

   if ( msg == MSG_QUIT ) {
     if ( msgbox(MW_QUESTION, MB_YES|MB_NO, "%s ?", TXT_AREYOUSUREYOUWANTTOQUITSEAL) != MSG_YES )
     return !0;

     return 0;

   }

   return 1;
};


static void  desktopline_standard_insertions ( void )
{
  program.valid = &program_valid;

  OBJECT(desktopline)->translate_event = &desktopline_translate_event;

  obj_insert(OBJECT(desktopline), desktopline_genbutton(ICO_xbtStart, "How do you want to spend your day today?", MSG_xAPPLICATION));

};

// desktopbox_items

void add_icons()
{
   int a = 0, x = 0, y = 0;

   BITMAP *icon = NULL;
   l_char *bmp_path = NULL;
   l_int xpos, ypos;
   l_int drive;
   p_dicon dicon = NULL;
   t_vdrive *p_drivers;
   t_rect r = VIEW(desktop)->size_limits(VIEW(desktop));
   r.b.y = r.b.y - 25;
   p_drivers = (t_vdrive *) VirtualDrives->at(VirtualDrives, a);


	while(p_drivers != NULL)
   {
//      bmp_path = get_key(set_format_text(NULL,"system/icons/%s/icon32", _strdup(p_drivers->drive)));
      bmp_path = getini_fromfile("drives.ini", set_format_text(NULL,"%s", _strdup(p_drivers->drive)), "icon32");

      if(strlen(p_drivers->drive) == 1)
      {
         drive = (int)(p_drivers->drive[0] - 'a');
         DEBUG_printf("drive with number %i found.\n", drive);
      }
      else
         drive = -1;

      icon = load_bitmap(set_format_text(NULL,"%s", bmp_path), NULL);

      r = rect_assign(x, y, x+ 55, y + 45);
      x += 65;
      if(icon)
      {
         dicon = dicon_init(sf_malloc(sizeof(t_dicon)), r, _strdup(p_drivers->drv->data), _strdup(p_drivers->drive), icon);
      }
      else
      {
         dicon = dicon_init(sf_malloc(sizeof(t_dicon)), r, _strdup(p_drivers->drv->data), _strdup(p_drivers->drive), IMG_DIR32);
      }

      if(drive > -1)
      {
         dicon->Meter = _HDMeter_init(sf_malloc(sizeof(t_HDMeter)), point_assign(24, 2), drive, 8, 28, rect_assign(1, 1, 5, 25));
         dicon->Meter->set_graphics(dicon->Meter, load_bitmap("./bmp/liquid.bmp", NULL), load_bitmap("./bmp/glass.bmp", NULL), load_bitmap("./bmp/shadow.bmp", NULL));
         DEBUG_printf("Loading of drive %i done.\n", drive);
      }
      dicon->run_pop_menu = &popup_go_menu;
      OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(dicon));

      DEBUG_printf("           - Icon in list, %s ...\n", _strdup(p_drivers->drive));
      p_drivers = (t_vdrive *) VirtualDrives->at(VirtualDrives, ++a);
      dicon = NULL;

      r = VIEW(desktop)->size_limits(VIEW(desktop));
      r.b.y -= 25;
      load_dicon_from_dir_in_object(OBJECT(desktop), r, "/desktop/");
	}
}

#include "desktop.exp"

//lib_exportable;


lib_begin ( void ) {

  if ( ap_process == AP_ALLOC ) {

          AP_EXPORTLIB(); /* export table defined by lib_exportable you find in seal.h */

  } else

  if ( ap_process == AP_INIT ) {
          t_rect r = rect_assign(0, 0, desktop->bounds.b.x, 25); /*taskbar position*/

          AP_SETNUMOFCALLS(1); /* set max num of calls */

          pal_taskbar[0] = registry_color("3d_face");
          pal_taskbar[1] = registry_color("3d_light");
          pal_taskbar[2] = registry_color("3d_shadow");
          pal_taskbar[3] = registry_color("3d_dark");
          pal_taskbar[4] = registry_color("taskbar_button_face");
          pal_taskbar[5] = registry_color("taskbar_button_light");
          pal_taskbar[6] = registry_color("taskbar_button_shadow");
          pal_taskbar[7] = registry_color("taskbar_button_dark");
          pal_taskbar[8] = registry_color("taskbar_button_caption");
          pal_taskbar[9] = registry_color("taskbar_selected_caption");
          pal_taskbar[10] = registry_color("taskbar_selected_face");

          draw_under_line = 0;//getininum_fromfile(INI_MAINFILE, INI_DESKTOP, "draw_under_line");

          image_menu = load_image(get_key("current/seal/images/badsealmenu_image"));

          datfile = conv_to_skipcolor_data(DLXGetFileData(ap_id), CO_SKIP_RED, CO_SKIP_GREEN, CO_SKIP_BLUE);

          get_programs_items();
          get_settings_items();
          get_document_items();
          get_help_items();

//          desktopline_minimalized = list_init(sf_malloc(sizeof(t_list)), ((void (*)(void*))&free_menu_item), DAT_TEXT);
          desktopline = desktopline_init(sf_malloc(sizeof(t_desktopline)));

          OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(desktopline));

          desktopline_standard_insertions();

          /* initialize task bar */

          taskbar_alone = get_key_integer("current/seal/optical/taskbar_alone");

          if(taskbar_alone) // Taskbar alone at the top of screen
          {
            taskbar = taskbar_init(malloc(sizeof(t_taskbar)), r);
            OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(taskbar));
          } else {               // Taskbar near the start menu
            r = rect_assign(70, 2, desktop->bounds.b.x-17, 24);
            taskbar = taskbar_init(malloc(sizeof(t_taskbar)), r);
            OBJECT(desktopline)->insert(OBJECT(desktopline), OBJECT(taskbar));
          }

          init_stillprocess(OBJECT(taskbar), 50);

          /* initialize user box */

          r = rect_assign(desktop->bounds.b.x-7, 2, desktop->bounds.b.x-4, 23);
          userbox = userbox_init(malloc(sizeof(t_userbox)), r);

          obj_insert(OBJECT(desktopline), OBJECT(userbox));

          add_icons();

          // Insert the desktop clock  - OCR: 11/04/2002
          create_clock();
  } else

  if(ap_process == AP_FREE) // Time to die
  {
      unload_datafile(datfile);
      datfile = NULL;
      dispose_list(&desktopline_minimalized, true);
  }


} lib_end;

SetInfoAppName("The Desktop");
SetInfoDescription("The Desktop for SEAL 2.0");
SetInfoCopyright("Copyright (c) David Fredin 2002, Kostas Michalopoulos 2000-2002, Owen Rudge 2002");
SetInfoManufacturer("David Fredin, Kostas Michalopoulos and Owen Rudge");
