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

/* Please note that these routines are for legacy compatibility only. You
   should use the new link routines - see lnk.h for details.
*/

/* Revision History:
 *
 * 26/05/2002: Updated bmp path (orudge)
 */

#include <seal.h>
#include <lnk.h>

#include "fileext.h"

l_bool fileext_islink (l_text file)
{
   if (lnk_is_link(file))
      return(true);
   else
      return ((!strcmp(_strdup(get_extension(file)), "LNK")) || (!strcmp(_strdup(get_extension(file)), "LDV")));
}

l_bool runitnow (l_text file)
{
   return(run_file(file));
}

l_text get_the_file_text (l_text file, l_text folder)
{
   l_text folderfile = io_realpath(folder, "desktop.inf");
   l_text final = file;
   l_text oldtxt = file;
   l_text realpath = io_realpath(folder, file);

   if (lnk_is_link(realpath))
      final = lnk_get_title(realpath);
   else
      final = getini_fromfile (folderfile, set_format_text(NULL, "%s", strupr(file)),"name" );

   if (final == NULL)
      return(oldtxt);
   else
      return(final);
}

l_text get_the_file_text2 (l_text file)
{
   return(get_the_file_text(io_filename(file), io_path(file)));
}

l_text get_the_file_des(l_text file, l_text folder)
{
   l_text folderfile = io_realpath(folder,"desktop.inf");
   l_text final;
   l_text realpath = io_realpath(folder, file);

   if (lnk_is_link(realpath))
      final = getini_fromfile(realpath, "definition", "description");
   else
      final = getini_fromfile(folderfile, set_format_text(NULL,"%s", strupr(file)),"des" );

   return(final);
}

l_text get_the_file_des2 (l_text file)
{
   return(get_the_file_des(io_filename(file), io_path(file)));
}

BITMAP *get_the_file_ico (l_text file, l_text folder)
{
   l_text folderfile = io_realpath(folder, "desktop.inf");
   l_text img;
   l_text realpath = io_realpath(folder, file);
   BITMAP *icon;

   if (lnk_is_link(realpath))
      icon = lnk_get_link_icon(realpath, 16, NULL);
   else
   {
      img = getini_fromfile (folderfile, set_format_text(NULL, "%s", strupr(file)), "img");

      if (img == NULL)
      {
         if (io_isdir(io_realpath(folder,file)))
            img = "/system/bmp/icons/dir.ico,16";
         else
            img = "/system/bmp/icons/file.ico,16";
      }

      icon = load_image(img);
   }

   return(icon);
}

BITMAP *get_the_file_ico2 (l_text file)
{
   return(get_the_file_ico(io_filename(file), io_path(file)));
}

BITMAP *get_the_file_ico32 (l_text file, l_text folder)
{
   l_text folderfile = io_realpath(folder, "desktop.inf");
   l_text img;
   l_text realpath = io_realpath(folder, file);
   BITMAP *icon;

   if (lnk_is_link(realpath))
      icon = lnk_get_link_icon(realpath, 32, NULL);
   else
   {
      img = getini_fromfile (folderfile, set_format_text(NULL, "%s", strupr(file)), "img32");

      if (img == NULL)
      {
         if (io_isdir(io_realpath(folder,file)))
            img = "/system/bmp/icons/dir.ico,32";
         else
            img = "/system/bmp/icons/file.ico,32";
      }

      icon = load_image(img);
   }

   return(icon);
}

BITMAP *get_the_file_ico322 (l_text file)
{
   return(get_the_file_ico32(io_filename(file), io_path(file)));
}

l_bool get_the_file_visible (l_text file, l_text folder)
{
   l_text folderfile = io_realpath(folder, "desktop.inf");
   l_text x = getini_fromfile (folderfile, set_format_text(NULL, "%s", strupr(file)), "hide");

   if (x == NULL)
      return true;
   else
      return false;
}

l_bool get_the_file_visible2 (l_text file)
{
   return(get_the_file_visible (io_filename(file), io_path (file)));
}

void set_the_file_ext (l_text file, l_text folder, l_text img, l_text img32, l_text name, l_text des, l_bool visible)
{
   l_text folderfile = io_realpath(folder, "desktop.inf");

   setini_tofile(folderfile, "desktop", "validate", "1", INI_STRING);
   setini_tofile(folderfile, strupr(file), "des", des, INI_STRING);
   setini_tofile(folderfile, strupr(file), "img", img, INI_STRING);
   setini_tofile(folderfile, strupr(file), "img32", img32, INI_STRING);
   setini_tofile(folderfile, strupr(file), "name", name, INI_STRING);

   if (visible == true)
      setini_tofile(folderfile, strupr(file), "hide", NULL, INI_STRING);
   else
      setini_tofile(folderfile, strupr(file), "hide", "1", INI_STRING);
}

void set_the_file_ext2 (l_text file, l_text img, l_text img32, l_text name, l_text des, l_bool visible)
{
   set_the_file_ext(io_filename(file), io_path (file), img, img32, name, des, visible);
}
