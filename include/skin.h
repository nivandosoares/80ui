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

#ifndef __SKIN_H_INCLUDED__
#define __SKIN_H_INCLUDED__

typedef struct t_skin_item *p_skin_item;
typedef struct t_skin_item
{
  l_text        name;
  BITMAP        *skin;
  short         left, top, right, bottom;
  l_color       textcolor;
  char          textalign; // 0 = left, 1 = center, 2 = right
  short         textshift;
} t_skin_item;

typedef struct t_skin_color *p_skin_color;
typedef struct t_skin_color
{
  l_text        name;
  l_color       color;
} t_skin_color;

typedef struct t_skin *p_skin;
typedef struct t_skin
{
  l_text        name;
  short         version;
  l_int         items;
  t_skin_item   *item;
  l_int         colors;
  t_skin_color  *color;
} t_skin;

extern p_skin   global_skin;
extern l_bool   use_skins;

/* this function initializes the skin system and loads global skin defined in
  registry key 'CURRENT/SEAL/SKIN' */
void initialize_skin_system();

/* this function shuts down the skin system and releases allocated memory for
  global skin (if was initialized) */
void shutdown_skin_system();

/* load_skin_from_ini loads skin information from an INI file and the bitmap
  files that this INI file describes. Returns NULL if the file wasn't found
  or a bitmap wasn't found or wasn't enough memory to store the bitmaps */
p_skin load_skin_from_ini(l_text filename);

/* releases allocated memory for given skin */
void free_skin(p_skin skin);

/* find_skin_item returns the skin item that has the given name */
p_skin_item find_skin_item(p_skin skin, l_text name);

/* draw_skin_ex draws the given item of the given skin to the given bitmap
  in given area (x1, y1)-(x2, y2) using the given text (if skin item wants
  text to be drawn) and given font */
void draw_skin_ex(BITMAP *bmp, p_skin skin, l_text item, l_int x1, l_int y1, l_int x2, l_int y2, void *font, l_text text);

/* like draw_skin_ex, but doesn't take the 'text' parameter */
void draw_skin(BITMAP *bmp, p_skin skin, l_text item, l_int x1, l_int y1, l_int x2, l_int y2);

/* get_skin color returns the specified color in the skin (this is actually
 used from registry_color, so it's recommended to use registry_color if the
 color you want to get is also specified in the registry) */
l_color get_skin_color(p_skin skin, l_text item);

/* two #defines to use with t_rect */
#define DRAW_SKIN_RECT_EX(bmp,s,i,r,f,t) draw_skin_ex(bmp, s, i, r.a.x, r.a.y, r.b.x, r.b.y, f, t)
#define DRAW_SKIN_RECT(bmp,s,i,r) draw_skin_ex(bmp, s, i, r.a.x, r.a.y, r.b.x, r.b.y, NULL, NULL)

#endif

