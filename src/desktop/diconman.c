/******************************************************************
 * SEAL 2.0                                                       *
 * Copyright (c) 2002-2003 SEAL Developers. All Rights Reserved.  *
 *                                                                *
 * Web site: http://sealsystem.sourceforge.net/                   *
 * For e-mail (current maintainer) look in readme.txt             *
 ******************************************************************/

 /* SEAL dicon icon-manager
  * (c) Copyright 2002 David Dredin.
  * All rights reserved.
  * web: http://software.xduffy.com
  * email: athanatos@home.se
  *
  * License: GPL
  */

#include "diconman.h"

t_appwin *ico_window;
t_dicon  *show_icon;
t_button *button_browse;
t_button *button_program;
t_button *button_save;
t_button *button_close;
t_stattext *about_program;
t_stattext *about_path;
t_textline *program_path;
t_textline *ico_path;
t_dicon *the_icon;
l_text s2a_path;
l_text file_path;
l_text about_text;

#define  MSG_BROWSE            100140
#define  MSG_SAVE              100141
#define  MSG_PROGRAM           100142

void stillicon_draw(p_view o);
void stillicon_translate_event(p_object o, t_event *event);

static p_list get_file_items(void)
{
                                            /* Free the items afterwards */
   p_list filestolookfor = list_init(malloc(sizeof(t_list)), &free_filehistory_item, 0);

   if(filestolookfor)
   {
      filestolookfor->insert(filestolookfor, new_filehistory_item("Windows bitmaps (*.bmp)", "*.bmp"));
      filestolookfor->insert(filestolookfor, new_filehistory_item("All files (*.*)", "*.*"));
   }
  return filestolookfor;
}

static p_list get_program_items(void)
{
                                            /* Free the items afterwards */
   p_list filestolookfor = list_init(malloc(sizeof(t_list)), &free_filehistory_item, 0);

   if(filestolookfor)
   {
      filestolookfor->insert(filestolookfor, new_filehistory_item("SEAL executables (*.s2a)", "*.s2a"));
      filestolookfor->insert(filestolookfor, new_filehistory_item("All files (*.*)", "*.*"));
   }
  return filestolookfor;
}

void window_translate_event(p_object o, p_event event)
{
   if(event->type == EV_MESSAGE)
	{
      switch(event->message)
		{
         case MSG_BROWSE:
         {
            file_path = open_dialog("/", "*.*", get_file_items());
            ico_path->set_text(ico_path, file_path);
            VIEW(show_icon)->hide(VIEW(show_icon));
            show_icon->icon = load_bitmap(GetFile(set_format_text(NULL,"%s", file_path)), NULL);
            VIEW(show_icon)->show(VIEW(show_icon));
            VIEW(show_icon)->draw_me(VIEW(show_icon));

            clear_event(event);
         } break;
         case MSG_PROGRAM:
         {
            s2a_path = open_dialog("/", "*.*", get_program_items());
            program_path->set_text(program_path, s2a_path);
            clear_event(event);
         } break;
         case MSG_SAVE:
         {
            file_path = _strdup(ico_path->text);

            setini_tofile("drives.ini", _strdup(the_icon->caption) , "icon32", GetFile(set_format_text(NULL,"%s", file_path)), INI_STRING);
            setini_tofile("drives.ini", _strdup(the_icon->caption) , "open", GetFile(set_format_text(NULL,"%s", s2a_path)), INI_STRING);
//            set_key(set_format_text(NULL,"system/icons/%s/icon32", _strdup(DICON(o)->caption)), _strdup(file_path));
            VIEW(the_icon)->hide(VIEW(the_icon));
            the_icon->icon = load_bitmap(GetFile(set_format_text(NULL,"%s", file_path)), NULL);
//            the_icon->Meter_on = false;
            VIEW(the_icon)->show(VIEW(the_icon));
            VIEW(the_icon)->draw_me(VIEW(the_icon));
            clear_event(event);
         } break;
      }
   }
}

void init_preferences(p_dicon dicon)
{
   the_icon = dicon;
   file_path = getini_fromfile("drives.ini", set_format_text(NULL,"%s", _strdup(the_icon->caption)), "icon32");
   s2a_path = getini_fromfile("drives.ini", set_format_text(NULL,"%s", _strdup(the_icon->caption)), "open");
//   file_path = get_key(set_format_text(NULL,"system/icons/%s/icon32", _strdup(dicon->caption)));
   ico_window = appwin_init(_malloc(sizeof(t_appwin)),
               rect_assign(0, 0, 220, 300), "Change icon",
               0|WF_MINIMIZE|WF_SYSMENU, ap_id, &window_translate_event);
   VIEW(ico_window)->align |= TX_ALIGN_CENTER;
   OBJECT(desktop)->insert(OBJECT(desktop),OBJECT(ico_window));

   show_icon = dicon_init(sf_malloc(sizeof(t_dicon)),
                     rect_assign(94, 30, 126, 62),
                     " ", " ", load_bitmap(file_path, NULL));

   OBJECT(show_icon)->translate_event = &stillicon_translate_event;
   VIEW(show_icon)->draw = &stillicon_draw;
   OBJECT(desktop)->insert(OBJECT(ico_window),OBJECT(show_icon));

   about_path = stattext_init(_malloc(sizeof(t_stattext)),
                      rect_assign(15, 95, 100, 115),
                      TX_ALIGN_CENTERY, "Icon Path:");
   OBJECT(ico_window)->insert(OBJECT(ico_window), OBJECT(about_path));

   ico_path = textline_init( _malloc(sizeof(t_textline)),
                         rect_assign(15, 120, 205, 140),
                         255, 0);
   ico_path->set_text(ico_path, file_path);
   OBJECT(ico_window)->insert(OBJECT(ico_window), OBJECT(ico_path));

   button_browse = button_init(_malloc(sizeof(t_button)),
                       rect_assign(135, 150, 195, 170),
                       "Browse...", MSG_BROWSE, BF_DEFAULT);
   OBJECT(ico_window)->insert(OBJECT(ico_window), OBJECT(button_browse));

   about_program = stattext_init(_malloc(sizeof(t_stattext)),
                      rect_assign(15, 180, 100, 200),
                      TX_ALIGN_CENTERY, "Open with:");
   OBJECT(ico_window)->insert(OBJECT(ico_window), OBJECT(about_program));

   program_path = textline_init( _malloc(sizeof(t_textline)),
                         rect_assign(15, 205, 205, 225),
                         255, 0);
   program_path->set_text(program_path, s2a_path);
   OBJECT(ico_window)->insert(OBJECT(ico_window), OBJECT(program_path));

   button_program = button_init(_malloc(sizeof(t_button)),
                       rect_assign(135, 235, 195, 255),
                       "Browse...", MSG_PROGRAM, BF_DEFAULT);
   OBJECT(ico_window)->insert(OBJECT(ico_window), OBJECT(button_program));

   button_save = button_init(_malloc(sizeof(t_button)),
                      rect_assign(50, 270, 110, 290),
                      "Save", MSG_SAVE, BF_DEFAULT);
   OBJECT(ico_window)->insert(OBJECT(ico_window), OBJECT(button_save));

   button_close = button_init(_malloc(sizeof(t_button)),
                       rect_assign(120, 270, 180, 290),
                       "Close", MSG_CLOSE, BF_DEFAULT);
   OBJECT(ico_window)->insert(OBJECT(ico_window), OBJECT(button_close));
}

void stillicon_draw(p_view o)
{
   t_rect  r = o->get_local_extent(o);
   t_point p;

   BITMAP *out = o->begin_paint(o, &p, r);

   if(out)
   {
      stretch_sprite(out, DICON(o)->icon, r.a.x+p.x+((r.b.x-r.a.x)/2)-16, r.a.y+p.y, 32, 32);
   }
  o->end_of_paint(o, r);
}

void stillicon_translate_event(p_object o, t_event *event)
{
}
