/******************************************************************
 * SEAL 2.0                                                       *
 * Copyright (c) 1999-2002 SEAL Developers. All Rights Reserved.  *
 *                                                                *
 * Web site: http://sealsystem.sourceforge.net/                   *
 * E-mail (current maintainer): davidhimself@users.sourceforge.net*
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

/* Based on Absolute SEAL skin.c */

/* Revision History (as of 26/05/2002):
 *
 * 26/05/2002: Fixed colour loading problem that included a "duff" first value (orudge)
 * 02/06/2002: Fixed colour loading problem that caused SEAL to crash when run under DOS (orudge)
 */

#include <seal.h>
#include <dataini.h>

p_skin global_skin = NULL;
l_bool use_skins = true;

void initialize_skin_system()
{
   l_text skin_key = get_key("CURRENT/SEAL/SKIN/FILE");
   DEBUG_printf("    Initializing skin system...\n");

   if (!skin_key)
      DEBUG_printf_ex(ET_ERROR, "      Key 'CURRENT/SEAL/SKIN/FILE' not found - no skin used\n");
   else
   {
      global_skin = load_skin_from_ini(skin_key);

      if (!global_skin)
         DEBUG_printf_ex(ET_ERROR, "    Cannot load skin - no skin used\n");
      else
         DEBUG_printf_ex(ET_DONE, "      Skin loaded\n");

      free(skin_key);
   }
}

void shutdown_skin_system()
{
   DEBUG_printf(" - Shutting down skin system...");

   if (global_skin)
      free_skin(global_skin);

   DEBUG_printf("    OK\n");
}

p_skin load_skin_from_ini(l_text filename)
{
   l_text basedir = 0;
   p_skin skin = NULL;
   l_int items = 0, i;
   FILE *f;
   ini_data *inidata;

   if (get_depth(screen) < 16)
   {
      use_skins = false;
      return(NULL);
   }

   f = fopen(filename, "rt");

   if (!f)
      return NULL;

   fclose(f);

   DEBUG_printf("SKIN: getting skin version\n");

   if (getininum_fromfile(filename, "Information", "skinsystem") != 1)
      return NULL;

   DEBUG_printf("SKIN: counting ini sections\n");

   f = fopen(filename, "rt");

   while (!feof(f))
   {
      l_char s[256];
      fgets(s, 256, f);

      if (!s[0])
         continue;

      if (s[strlen(s)-1] < 32)
         s[strlen(s)-1] = 0;

      if (s[0] == '[' && s[strlen(s)-1] == ']' && stricmp(s+1, "Information]"))
         items++;
   }

   DEBUG_printf("SKIN: checking ini sections\n");

   if (!items)
   {
      fclose(f);
      return(NULL);
   }

   DEBUG_printf("SKIN: initializing 'skin' struct...\n");

   skin = (p_skin) malloc(sizeof(t_skin));
   skin->items = items;
   skin->item = (p_skin_item) malloc(sizeof(t_skin_item)*items);

   memset(skin->item, 0, sizeof(t_skin_item)*items);

   rewind(f);
   items = 0;

   DEBUG_printf("SKIN: loading skin sections\n");

   while (!feof(f))
   {
      l_char s[256];
      fgets(s, 256, f);

      if (!s[0])
         continue;

      if (s[strlen(s)-1] < 32)
         s[strlen(s)-1] = 0;

      if (s[0] == '[' && s[strlen(s)-1] == ']' && stricmp(s+1, "Information]") && stricmp(s+1, "Colours]"))
      {
         skin->item[items].name = strdup(s+1);
         skin->item[items].name[strlen(s+1)-1] = 0;
         DEBUG_printf("SKIN:    %i. %s\n", items, skin->item[items].name);
         items++;
      }
   }

   fclose(f);

   DEBUG_printf("SKIN: loading skin info\n");

   skin->name = getini_fromfile(filename, "Information", "name");
   skin->version = getininum_fromfile(filename, "Information", "version");

   DEBUG_printf("SKIN: calculating base dir\n");
   basedir = strdup(filename);

   do
   {
      basedir[strlen(basedir)-1] = 0;
   } while (basedir[0] && basedir[strlen(basedir)-1] != '/');

   DEBUG_printf("SKIN:   basedir = '%s'\n", basedir);

   for (i = 0; i < items; i++)
   {
      l_text section = skin->item[i].name;
      l_text bmpfile = malloc(1024);
      l_text skinfile = NULL;
      ini_rgb *rgb;

//    skin->item[i].name = getini_fromfile(filename, section, "name");

      DEBUG_printf("SKIN: loading skin item '%s'\n", skin->item[i].name);

      strcpy(bmpfile, basedir);
      skinfile = getini_fromfile(filename, section, "skin");

      if (skinfile)
      {
         strcat(bmpfile, skinfile);
         skin->item[i].skin = load_image(bmpfile);
         DEBUG_printf("SKIN: skinfile exists\n");
      }
      else
         skin->item[i].skin = NULL;

      DEBUG_printf("SKIN: Will look for alignment\n");
      skin->item[i].left = getininum_fromfile(filename, section, "left");
      skin->item[i].top = getininum_fromfile(filename, section, "top");
      skin->item[i].right = getininum_fromfile(filename, section, "right");
      skin->item[i].bottom = getininum_fromfile(filename, section, "bottom");

      rgb = getini_color(filename, section, "textcolor");
      DEBUG_printf("SKIN: Got textcolor.\n");
      if (rgb)
         skin->item[i].textcolor = makecol(rgb->r, rgb->g, rgb->b);
      skin->item[i].textalign = getininum_fromfile(filename, section, "textalign");
      skin->item[i].textshift = getininum_fromfile(filename, section, "textshift");
      if(bmpfile)
        free(bmpfile);
      if(rgb)
      {
        //free(rgb);
      }  
      DEBUG_printf("SKIN: skin item loaded\n");
//      free(section);
   }

   for (i = 0; i < items; i++)
   {
      if (!skin->item[i].skin)
      {
         l_int i;

         DEBUG_printf("SKIN: load aborted because a skin file wasn't loaded");

         for (i = 0; i < items; i++)
         {
            if (skin->item[i].name)
               free(skin->item[i].name);

            if (skin->item[i].skin)
               free(skin->item[i].skin);
         }

         if (skin->name)
            free(skin->name);

         free(skin->item);
         return(NULL);
      }
   }

   DEBUG_printf("SKIN: loading colours information\n");

   inidata = getinidata_fromfile(filename, "Colours");

   if (inidata)
   {
      l_int i, colors = getini_linenum(inidata);

      skin->color = malloc(sizeof(t_skin_color)*colors);
      skin->colors = 0;

      for (i = 0; i < colors; i++)
      {
         l_text colorname = NULL;
         l_text colorvalue = getini_line(&colorname, inidata, i);
         ini_rgb *rgb;

         if (!colorname || !colorvalue)
         {
            if (colorname)
               free(colorname);

            if (colorvalue)
               free(colorvalue);

            continue;
         }

         if (colorname[0] == '\n')
            continue;

         skin->color[skin->colors].name = colorname;

         rgb = getini_color(filename, "Colours", colorname);
         skin->color[skin->colors].color = makecol(rgb->r, rgb->g, rgb->b);
         DEBUG_printf("SKIN: colour loaded named '%s': R: %i G: %i B: %i\n", colorname, rgb->r, rgb->g, rgb->b);

         //free(rgb);
         free(colorvalue);
         skin->colors++;
      }

      DEBUG_printf("SKIN: Colours loaded\n");
      free(inidata);
   }

   DEBUG_printf("SKIN: done\n");
   return(skin);
}

void free_skin(p_skin skin)
{
   l_int i;

   for (i = 0; i < skin->items; i++)
   {
      if (skin->item[i].name)
         free(skin->item[i].name);

      if (skin->item[i].skin)
         free(skin->item[i].skin);
   }

   for (i = 0; i < skin->colors; i++)
   {
      if (skin->color[i].name)
         free(skin->color[i].name);
   }

   if (skin->name)
      free(skin->name);

   free(skin->item);
}

p_skin_item find_skin_item(p_skin skin, l_text name)
{
   p_skin_item item = NULL;
   l_int i;

   for (i = 0; i < skin->items; i++)
   {
      if (!stricmp(name, skin->item[i].name))
         return &(skin->item[i]);
   }

   return(NULL);
}

void draw_skin_ex(BITMAP *bmp, p_skin skin, l_text item, l_int x1, l_int y1, l_int x2, l_int y2, void *font, l_text text)
{
   p_skin_item si = find_skin_item(skin, item);
   l_int margin_left = 0;
   if(!strcmp(item, strdup("ActiveTitleBar")) || !strcmp(item, strdup("InactiveTitleBar")))
   {
      margin_left = (l_int)(*((long*)getini_fromfile(get_key("CURRENT/SEAL/SKIN/FILE"), strdup("ActiveTitleBar"), strdup("text-margin-left"))));
//      DEBUG_printf("text-margin-left: %i\n", margin_left);
   }
   if (!si)
      return;

   // OCR 12/05/2002: Used masked versions of blit

   masked_blit(si->skin, bmp, 0, 0, x1, y1, si->left, si->top);
   masked_blit(si->skin, bmp, 0, si->skin->h-si->bottom, x1, y2-si->bottom+1, si->left, si->bottom);
   masked_blit(si->skin, bmp, si->skin->w-si->right, 0, x2-si->right+1, y1, si->right, si->top);
   masked_blit(si->skin, bmp, si->skin->w-si->right, si->skin->h-si->bottom, x2-si->right+1, y2-si->bottom+1, si->right, si->bottom);

   masked_stretch_blit(si->skin, bmp, 0, si->top, si->left, si->skin->h-si->top-si->bottom, x1, y1+si->top, si->left, (y2-y1+1)-si->top-si->bottom);
   masked_stretch_blit(si->skin, bmp, si->skin->w-si->right, si->top, si->right, si->skin->h-si->top-si->bottom, x2-si->right+1, y1+si->top, si->right, (y2-y1+1)-si->top-si->bottom);

   masked_stretch_blit(si->skin, bmp, si->left, 0, si->skin->w-si->left-si->right, si->top, x1+si->left, y1, (x2-x1+1)-si->left-si->right, si->top);
   masked_stretch_blit(si->skin, bmp, si->left, si->skin->h-si->bottom, si->skin->w-si->left-si->right, si->bottom, x1+si->left, y2-si->bottom+1, (x2-x1+1)-si->left-si->right, si->bottom);

   masked_stretch_blit(si->skin, bmp, si->left, si->top, si->skin->w-si->left-si->right, si->skin->h-si->top-si->bottom, x1+si->left, y1+si->top, (x2-x1)-si->left-si->right+1, (y2-y1)-si->top-si->bottom+1);

   if (text)
   {
      switch (si->textalign)
      {
         case 0:
            textout_draw_rect(bmp, font, text, -1, x1+si->left+si->textshift+margin_left, y1+si->top, x2-si->right, y2-si->bottom, TX_ALIGN_LEFT, si->textcolor, TX_NOCOLOR, TRUE);
            break;

         case 1:
            textout_draw_rect(bmp, font, text, -1, x1+si->left+margin_left, y1+si->top, x2-si->right, y2-si->bottom, TX_ALIGN_CENTER, si->textcolor, TX_NOCOLOR, TRUE);
            break;

         case 2:
            textout_draw_rect(bmp, font, text, -1, x1+si->left+margin_left, y1+si->top, x2-si->right-si->textshift, y2-si->bottom, TX_ALIGN_RIGHT, si->textcolor, TX_NOCOLOR, TRUE);
            break;
      }
   }
/*   if (text)
   {
      switch (si->textalign)
      {
         case 0:
            textout_draw_rect(bmp, font, text, -1, x1+si->left+si->textshift, y1+si->top, x2-si->right, y2-si->bottom, TX_ALIGN_LEFT, si->textcolor, TX_NOCOLOR, TRUE);
            break;

         case 1:
            textout_draw_rect(bmp, font, text, -1, x1+si->left, y1+si->top, x2-si->right, y2-si->bottom, TX_ALIGN_CENTER, si->textcolor, TX_NOCOLOR, TRUE);
            break;
         case 2:
            textout_draw_rect(bmp, font, text, -1, x1+si->left, y1+si->top, x2-si->right-si->textshift, y2-si->bottom, TX_ALIGN_RIGHT, si->textcolor, TX_NOCOLOR, TRUE);
            break;
      }
   }*/
}

void draw_skin(BITMAP *bmp, p_skin skin, l_text item, l_int x1, l_int y1, l_int x2, l_int y2)
{
   draw_skin_ex(bmp, skin, item, x1, y1, x2, y2, NULL, NULL);
}

l_color get_skin_color(p_skin skin, l_text item)
{
   int i;

   for (i = 0; i < skin->colors; i++)
   {
      if (!stricmp(item, skin->color[i].name))
         return(skin->color[i].color);

      return(0);
   }
}
