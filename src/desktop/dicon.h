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
 * 15/05/2002: Ported the library to Seal2.0.11 (xduffy)
 * 15/05/2002: Renamed "it" to "icon" and minor syntax changing (xduffy)
 * 17/05/2002: Got the icons to render flicker-free. Added sort, hide,
 *             show and save icon functions (xduffy)
 * 18/05/2002: Added gridify icons function (xduffy)
 */

#ifndef __DICON_H_INCLUDED__
#define __DICON_H_INCLUDED__

#include <dos.h>
#include "allegro.h"
#include <seal.h>
#include <engine.h>
#include <view.h>
#include "fileext.h"
#include "compat.h"
#include "screen.h"
#include "lnk.h"
#include "vfile.h"
#include "free.h"

#ifdef __cplusplus
extern "C" {
#endif

extern l_int DEBUG_printf(l_text text, ...);


#define TAG_DICON            0x0000020
#define DIR_IGNORE          '$'


typedef struct t_HDMeter *p_HDMeter;

typedef struct t_HDMeter
{
   BITMAP *liquid;
   BITMAP *glass;
   BITMAP *shadow;

   double percent;
   l_int disk;

   l_int height;
   l_int width;

   t_point position;

   t_rect liquidflow;

   void (*set_graphics)(p_HDMeter Meter, BITMAP *liquid, BITMAP *glass, BITMAP *shadow);
   BITMAP *(*getHDMeter)(p_HDMeter Meter);
} t_HDMeter;

p_HDMeter _HDMeter_init(p_HDMeter Meter, t_point position, int whichdrive, int width, int height, t_rect liquidflow);
void      HDMeter_setgraphics(p_HDMeter Meter, BITMAP *liquid, BITMAP *glass, BITMAP *shadow);
BITMAP    *HDMeter_getHDMeter(p_HDMeter Meter);


/* The d stands for desktop */
typedef struct t_dicon *p_dicon;

typedef struct t_dicon
{
   struct t_view obclass;
   l_text filename;
   l_text caption;
   BITMAP *icon;
   // Current background.
   BITMAP *cur_bg;

   p_HDMeter Meter;
   l_bool Meter_on;

   /* ini_file have to be the local folder.ini !!! */
   /* If you don't make your totally own thing, of course :-) */
   void (*save_position)(p_dicon icon, l_text ini_file);
   void (*save_icon)(p_dicon icon, l_text ini_file, l_text icon_path);
   void (*hide)(p_dicon icon, l_text ini_file);

   /* run_pop_menu is a function you have to define yourself */
   void (*run_pop_menu)(p_dicon icon);
   void (*menu_translate_event)(p_dicon icon, l_dword message);

} t_dicon;



#define DICON(o)   ((p_dicon)(o))

void    dicon_draw(p_view o);
void    dicon_translate_event(p_object o, t_event *event);
void    unload_all_dicon_from_object(p_object object);
void    load_dicon_from_dir_in_object(p_object object, t_rect limits, l_text path);

p_dicon _dicon_init (p_dicon o, t_rect r, l_text filename , l_text caption ,BITMAP *icon);
extern  p_dicon (*dicon_init) ( p_dicon o, t_rect r, l_text filename , l_text caption ,BITMAP *icon);

void    dicon_save_pos(p_dicon icon, l_text ini_file);
void    sort_dicons_in_object(p_object object, l_bool vertical, t_rect limits);
void    gridify_dicons_in_object(p_object object, t_rect limits);
void    show_all_dicons(p_object object);
void    draw_all_dicons(p_object object);
void    hide_all_dicons(p_object object);

void    dicon_save_icon(p_dicon icon, l_text ini_file, l_text icon_path);
void    dicon_hide(p_dicon icon, l_text ini_file);

void    dicon_drag_view(p_view o, l_word mode, t_event *event);


#ifdef __cplusplus
}
#endif

#endif

