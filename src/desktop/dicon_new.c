/***********************************************************/
/*                                                         */
/*                 dicon.h - Icons for SEAL                */
/*                                                         */
/*                                                         */
/*                    Copyright (C) 2001, 2002             */
/*                     by Callum Lawson                    */
/*                    Copyright (C) 2002                   */
/*                     by David Fredin                     */
/*                                                         */
/*          e-mail: callum.lawson@laposte.net              */
/*          e-mail: athanatos@home.se                      */
/*                                                         */
/***********************************************************/
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

/* Revision History (as of 10/04/2002):
 * 15/05/2002 Ported the library to Seal2.0.11 and it's now a valid xdl (xduffy)
 * 15/05/2002 Renamed "it" to "icon" and minor syntax changing (xduffy)
 */


#include "dicon.h"

p_dicon (*dicon_init) (p_dicon o, t_rect r, l_text filename , l_text caption ,BITMAP *icon ) = &_dicon_init;

void dicon_draw(p_view o)
{
   t_rect  r = o->get_local_extent(o);
   t_point p;
   BITMAP *BMeter;

   BITMAP *out = o->begin_paint(o, &p, r);

   l_int x, y;

   if(!DICON(o)->cur_bg)
   {
      x = VIEW(o)->bounds.a.x;
      y = VIEW(o)->bounds.a.y;

//Adapted for the new icon size possibilities...
      DICON(o)->cur_bg = create_bitmap(DICON(o)->width, DICON(o)->height);
      blit(desktop->brush.background, DICON(o)->cur_bg, x, y, 0, 0, DICON(o)->width, DICON(o)->height);
   }   

   if(out)
   {
      //rectfill(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, COLOR(CO_NOCOLOR));

/*	Stupid stuff:

      if(DICON(o)->Meter && !strcmp(DICON(o)->caption, "a"))
      {
         _free(DICON(o)->Meter);
         DICON(o)->Meter = NULL;
         DICON(o)->Meter_on = false;
      }
      if(DICON(o)->Meter && DICON(o)->Meter_on == false)
      {
         BMeter = (DICON(o)->Meter)->getHDMeter(DICON(o)->Meter);
         blit(BMeter, DICON(o)->icon, 0, 0, DICON(o)->Meter->position.x, DICON(o)->Meter->position.y, DICON(o)->Meter->width, DICON(o)->Meter->height);
         DICON(o)->Meter_on = true;
      }
Should change into something better like:
*/
      if(DICON(o)->Meter_on && DICON(o)->Meter)
      {
         BMeter = (DICON(o)->Meter)->getHDMeter(DICON(o)->Meter);
         blit(BMeter, DICON(o)->icon, 0, 0, DICON(o)->Meter->position.x, DICON(o)->Meter->position.y, DICON(o)->Meter->width, DICON(o)->Meter->height);
         DICON(o)->Meter_on = true;
      }
      else if(DICON(o)->Meter)
      {
         _free(DICON(o)->Meter);
      }

      stretch_sprite(out, DICON(o)->icon, r.a.x+p.x+((r.b.x-r.a.x)/2)-16, r.a.y+p.y, 32 , 32);
      textout_draw_rect_ex(out, o->font, DICON(o)->caption, -1,
                                        p.x + r.a.x,
                                        p.y + r.a.x + 34,
                                        p.x + r.b.x,
                                        p.y + r.b.y,
                                        TX_ALIGN_CENTERX, color_desktop_text, color_desktop_face, 1,2);
   }
  o->end_of_paint(o, r);
}

void dicon_translate_event(p_object o, t_event *event)
{
   l_char *program_path;
   if (event->type & EV_MOUSE)
   {
      if (OBJECT(mouse)->state & MO_SF_MOUSELDOUBLE) 
      {
         clear_event(event);
//        program_path = get_key(set_format_text(NULL,"system/icons/%s/open", DICON(o)->caption));
         program_path = getini_fromfile("drives.ini", set_format_text(NULL,"%s", DICON(o)->caption), "open");
         if (program_path)
         {
            run_file_args(program_path, DICON(o)->filename);
         } 
         else 
         { 
            run_file(DICON(o)->filename);
         }
      }
      else if(OBJECT(mouse)->state & MO_SF_MOUSELDOWN)
      {
         l_word dm = (VIEW(o)->is_top_view(VIEW(o)) && VIEW(o)->drag_mode &DM_DRAGCONTEXT)?DM_DRAGCONTEXT:0;
         destroy_bitmap(DICON(o)->cur_bg);
         VIEW(o)->hide(VIEW(o));
         VIEW(o)->drag_view(VIEW(o), DM_DRAGMOVE+dm, event);
         VIEW(o)->show(VIEW(o));
         VIEW(o)->draw_me(VIEW(o));
//         dicon_save_pos(DICON(o), "drives.ini");
         clear_event(event);
      }
      else if(OBJECT(mouse)->state & MO_SF_MOUSERUP)
      {
         DICON(o)->run_pop_menu(DICON(o));
         clear_event(event);
      }
   } 
}

p_dicon _dicon_init(p_dicon o, t_rect r, l_text filename , l_text caption , BITMAP *icon)
{
   BITMAP *tmp = create_bitmap(icon->w, icon->h);
   if(!o)
      return NULL;

   clear_type(o, sizeof(t_dicon));

   view_init(VIEW(o), r);

   o->filename = filename;
   o->caption  = caption;
   blit(icon, tmp, 0, 0, 0, 0, icon->w, icon->h);
   o->icon    = tmp;

// Some kind of checkup on how the filename/caption will look like sizewise. fe:
// p_point name = getTextSize(caption);  //or maybe something like this should exist:
// p_point name = getTextOutSize(caption, LineWidth, LineSpacing);
// And then:
// o->width = icon->w + name.x;
// o->height = icon->h + name.y;
// Then we have the necessary stuff to use bigger (or smaller) icons in Seal....
// Now we just need to add the ability to use multilayeralphatransparent pngs too :-)


   OBJECT(o)->tag |= TAG_DICON;

   VIEW(o)->drag_mode |=  DM_DRAGMOVE+drag_context_ok*DM_DRAGCONTEXT;
   VIEW(o)->draw_mode &= ~(DWM_CANTACCELMOVE*accel_moving_ok);

   OBJECT(o)->translate_event = &dicon_translate_event;
   VIEW(o)->draw              = &dicon_draw;
   VIEW(o)->drag_view         = &dicon_drag_view;
   VIEW(o)->transparent       = true;
   o->save_position           = &dicon_save_pos;
   o->save_icon               = &dicon_save_icon;
   o->hide                    = &dicon_hide;
   o->run_pop_menu            = NULL;
   o->menu_translate_event    = NULL;
   o->Meter                   = NULL;
   o->Meter_on                = false;
   o->cur_bg                  = NULL;
   
   OBJECT(o)->set_options(OBJECT(o), OB_OF_SELECTABLE+OB_OF_ENABLE, false);
//   OBJECT(o)->set_options(OBJECT(o), OB_OF_STILLPROCESS, true);

   return o;
}

void load_dicon_from_dir_in_object(p_object object, t_rect limits, l_text path)
{
   struct t_ffblk file;
   l_text dir_path = GetFile(set_format_text(NULL, "%s", path));
   l_int x = limits.a.x;
   l_int y = limits.a.y + 55;
   l_int xdelta = 65, ydelta = 0;
   l_int done = 0;
   l_text find_this = io_realpath(dir_path, "*.*");
   l_text filename;
   l_text caption;
   p_dicon icon;
   BITMAP *icon_bitmap;
   t_rect rect;

   done = io_findfirst(find_this, &file, FA_ARCH+FA_DIREC);

   while(!done)
   {
      if(io_isfilename (file.ff_filename))
      {
         rect = rect_assign(x, y, x + 55, y + 45);

         filename = io_realpath(dir_path, file.ff_filename);
         caption = lnk_nice_link(file.ff_filename);
         icon_bitmap = get_the_file_ico32(file.ff_filename, dir_path);

         icon = dicon_init( _malloc(sizeof(t_dicon)), rect, filename, caption, icon_bitmap);
         object->insert(object,OBJECT(icon));

         VIEW(icon)->show(VIEW(icon));
         VIEW(icon)->draw_me(VIEW(icon));

         x += xdelta;
         y += ydelta;
         if(x + 55 > limits.b.x)
         {
            y += 55;
            x = 0;
         }
         if(y + 45 > limits.b.y)
            done = true;

      _free(caption);
      _free(filename);
      }
      done = io_findnext(&file);
   }
  _free(find_this);
}

void unload_all_dicon_from_object(p_object object)
{
   p_object p = OBJECT(OBJECT(object)->first_view(OBJECT(object)));
   p_object f = p;
   do
   {
      if (p->tag & TAG_DICON)
      {
         dispose(p);
      }
      p = OBJECT(OBJECT(p)->next_view(OBJECT(p))); // Next ...
   }
   while(f != p);
}

void dicon_save_pos(p_dicon icon, l_text ini_file)
{
 
   DEBUG_printf("save_pos called for %s\n", _strdup(icon->caption));
 

/*
   l_text name = _strdup(icon->caption);
   l_int x, y;

   x = VIEW(icon)->bounds.a.x;
   y = VIEW(icon)->bounds.a.y;

   if(!io_isfile(ini_file))
      return;

   setini_tofile(ini_file,  name, "x", &x, INI_DECANUM);
   setini_tofile(ini_file,  name, "y", &y, INI_DECANUM);

   _free(name);
*/
}

void gridify_dicons_in_object(p_object object, t_rect limits)
{
 
//   DEBUG_printf("gridify called\n");
 

   p_object f = (OBJECT(OBJECT(object)->first_view(OBJECT(object))))->next;
//   f = OBJECT(OBJECT(f)->next_view(OBJECT(f)));
   p_object p = f;
   t_rect rect;
   l_int x, y;
   l_int x_times = 0, y_times = 0;
   do
   {
      if (p->tag & TAG_DICON)
      {
         VIEW(p)->hide(VIEW(p));
         x = VIEW(p)->bounds.a.x;
         y = VIEW(p)->bounds.a.y;
         x_times = (int) ((x + 32.5) / 65);
         y_times = (int) ((y + 27.5) / 55);
         if((float)((int)x/65) == (float)x / (float)65)
         {
         }
         else
         {
/*            while(x_times * 65 + 55 > limits.b.x)
            {
               --x_times;
            }
            while(x_times * 65 < limits.a.x)
            {
               ++x_times;
            }
            while(y_times * 55 + 45 > limits.b.y)
            {
               --y_times;
            }
            while(y_times * 55 + 45 < limits.a.y)
            {
               ++y_times;
            }                   */
            rect = rect_assign(x_times * 65, y_times * 55, x_times * 65 + 55, y_times * 55 + 45);
            VIEW(p)->change_bounds(VIEW(p), rect);
         }
         VIEW(p)->show(VIEW(p));
      }
//      VIEW(p)->draw_me(VIEW(p));
      p = OBJECT(p)->next; //_view(OBJECT(p))); // Next ...
   }
   while(p != f);
   draw_all_dicons(OBJECT(desktop));
//   DEBUG_printf("Gridified done gonna call draw sub views.\n");
//   desktop->draw_sub_views(VIEW(desktop), VIEW(OBJECT(desktop)->last->next), VIEW(OBJECT(desktop)->last));
 
   DEBUG_printf("Gridify icons done.\n");
 

}

void sort_dicons_in_object(p_object object, l_bool vertical, t_rect limits)
{
   p_object f = (OBJECT(OBJECT(object)->first_view(OBJECT(object))))->next;
   p_object p = f;
   BITMAP *buf;
   t_rect rect;
   l_int x = limits.a.x + 10, y = limits.a.y + 10;
   l_int xdelta = 0, ydelta = 0;
   if(vertical)
   {
      ydelta = 55;
   }
   else
   {
      xdelta = 65;
   }
   do
   {
      if (p->tag & TAG_DICON)
      {
         rect = rect_assign(x, y, x + 55, y + 45);
         x += xdelta;
         y += ydelta;
         if(vertical)
         {
            if(y + 45 > limits.b.y)
            {
               x += 65;
               y = 0;
            }
            if(x + 55 > limits.b.x)
               return;
         }
         else
         {
            if(x + 55 > limits.b.x)
            {
               y += 55;
               x = 0;
            }
            if(y + 45 > limits.b.y)
               return;
         }
         VIEW(p)->change_bounds(VIEW(p), rect);
      }
      p = OBJECT(p)->next;
   }
   while(p != f);
   draw_all_dicons(OBJECT(desktop));
}


void dicon_save_icon(p_dicon icon, l_text ini_file, l_text icon_path)
{
/*
   if(icon && io_isfile(ini_file) && io_isfile(icon_path))
      setini_tofile(_strdup(ini_file), _strdup(icon->caption), "icon32", _strdup(icon_path), INI_STRING);
*/
   return;
}

void dicon_hide(p_dicon icon, l_text ini_file)
{
   int show = false;
   setini_tofile(_strdup(ini_file), _strdup(icon->caption), "show", &show, INI_DECANUM);
}

void draw_all_dicons(p_object object)
{
   p_object f = OBJECT(OBJECT(object)->first_view(OBJECT(object)));
   p_object p = f;
   do
   {
      if (p->tag & TAG_DICON)
      {
         VIEW(p)->draw_me(VIEW(p));
      }
      p = OBJECT(OBJECT(p)->next_view(OBJECT(p))); // Next ...
   }
   while(p != f);
}

void hide_all_dicons(p_object object)
{
   p_object f = OBJECT(OBJECT(object)->first_view(OBJECT(object)));
   p_object p = f;
   do
   {
      if (p->tag & TAG_DICON)
      {
         VIEW(p)->hide(VIEW(p));
      }
      p = OBJECT(OBJECT(p)->next_view(OBJECT(p))); // Next ...
   }
   while(p != f);
}

void show_all_dicons(p_object object)
{
   p_object f = OBJECT(OBJECT(object)->first_view(OBJECT(object)));
   p_object p = f;
   do
   {
      if (p->tag & TAG_DICON)
      {
         VIEW(p)->show(VIEW(p));
      }
      p = OBJECT(OBJECT(p)->next_view(OBJECT(p))); // Next ...
   }
   while(p != f);
}

void dicon_drag_view(p_view o, l_word mode, t_event *event)
{
   t_rect safebounds = o->bounds;
   t_rect r = safebounds;
   t_cursor icon_cursor;

   icon_cursor.bmp = create_bitmap(32, 32);

   /* 63519 is the colour that get's invicible when you move the mouse */
   clear_to_color(icon_cursor.bmp, 63519);

   rect(icon_cursor.bmp, 0, 0, 31, 31, makecol32(0, 0, 0));
   icon_cursor.focus.x = 16;
   icon_cursor.focus.y = 16;

   DEBUG_printf("drag_view called for\n");

   if(o->drag_mode & mode)
   {
      STOP_PROCESS();

      if(mode & DM_DRAGMOVE)
      {
         mouse_set_cursor(MOUSE, &icon_cursor);
      }
      o->hide(o);
      DEBUG_printf("Gonna drag\n");
      r = o->drag(o, mode, event);
      DEBUG_printf("Dragged, change bounds and all that\n");
      START_PROCESS();
      o->change_bounds(o, r);
      DEBUG_printf("Dragging done... show icon again\n");
      o->show(o);
      DEBUG_printf("Dragging done... gonna draw sub views\n");
      draw_all_dicons(OBJECT(desktop));
//      desktop->draw_sub_views(VIEW(desktop), VIEW(OBJECT(desktop)->last->next), VIEW(OBJECT(desktop)->last));
   }
 
   DEBUG_printf("drag_view done for %s\n", _strdup(DICON(o)->caption));
 

}

p_HDMeter _HDMeter_init(p_HDMeter Meter, t_point position, int whichdrive, int width, int height, t_rect liquidflow)
{
   if(!Meter)
      return NULL;
   clear_type(Meter, sizeof(t_HDMeter));

   Meter->disk = whichdrive;
   Meter->width = width;
   Meter->height = height;
   Meter->position = position;
   Meter->liquidflow = liquidflow;

   Meter->getHDMeter = &HDMeter_getHDMeter;
   Meter->set_graphics = &HDMeter_setgraphics;

   return Meter;
}

void HDMeter_setgraphics(p_HDMeter Meter, BITMAP *liquid, BITMAP *glass, BITMAP *shadow)
{
   Meter->liquid = liquid;
   Meter->glass = glass;
   Meter->shadow = shadow;
}

BITMAP *HDMeter_getHDMeter(p_HDMeter Meter)
{
   BITMAP *Bowl = create_bitmap(Meter->shadow->w, Meter->shadow->h);
   t_idisk driven;
   blit(Meter->shadow, Bowl, 0, 0, 0, 0, Meter->shadow->w, Meter->shadow->h);
   draw_sprite(Bowl, Meter->glass, 0, 0);
   driven = getpercentused(Meter->disk);
   DEBUG_printf("Percent of disk free: %i\n", (((double)driven.free / (double)driven.total)*100));
   Meter->percent = (((double)driven.free / (double)driven.total)) * 25;//(Meter->liquidflow.b.y - Meter->liquidflow.a.y + 1);
   blit(Meter->liquid, Bowl, 0, 0, 1, Meter->percent + 1, 5, 25 - Meter->percent + 1);
   

   return Bowl;
}
