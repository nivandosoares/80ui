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

#ifndef __FILEEXT_H_INCLUDED__
#define __FILEEXT_H_INCLUDED__

l_bool fileext_islink ( l_text file );

l_text get_the_file_text (l_text file, l_text folder);
l_text get_the_file_des (l_text file, l_text folder);
BITMAP *get_the_file_ico (l_text file, l_text folder);
BITMAP *get_the_file_ico32 (l_text file, l_text folder);
l_bool get_the_file_visible (l_text file, l_text folder);

l_text get_the_file_text2 (l_text file);
l_text get_the_file_des2 (l_text file);
BITMAP *get_the_file_ico2 (l_text file);
BITMAP *get_the_file_ico322 (l_text file);
l_bool get_the_file_visible2 (l_text file);

void set_the_file_ext (l_text file, l_text folder,l_text img ,l_text img32, l_text name, l_text des, l_bool visible);
void set_the_file_ext2 (l_text file ,l_text img ,l_text img32, l_text name, l_text des, l_bool visible);

l_bool runitnow (l_text file);

#endif
