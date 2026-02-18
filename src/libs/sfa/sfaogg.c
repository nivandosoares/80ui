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

/* Revision History:
 *
 * 14/08/2002: Created Ogg Vorbis driver for SFA (orudge)
 */

#include <seal.h>
#include <sound.h>
#include <dialogs.h>
#include <allegro.h>
#include <allegogg.h>

l_text filename;
OGGSTREAM *os;

l_bool ogg_init_driver (p_soundformat_item o)
{
   return(TRUE);
}

l_bool ogg_init_file (p_soundformat_item o, l_text file, l_bool play)
{
   if (filename)
      _free(filename);

   filename = _strdup(file);
         
   if (play == TRUE)
   {
      os = play_ogg_stream(filename, 16384, 16, 255, 127);

      if (!os)
      {
         seal_error(ERR_INFO, INI_TEXT("Unable to load file."));
         return(FALSE);
      }
   }

   return(TRUE);
}

l_bool ogg_play_file (p_soundformat_item o)
{
   os = play_ogg_stream(filename, 16384, 16, 255, 127);

   if (!os)
   {
      seal_error(ERR_INFO, INI_TEXT("Unable to load file."));
      return(FALSE);
   }

   return(TRUE);
}
         

l_bool ogg_poll_file(p_soundformat_item o)
{
   if (update_ogg_stream(os) == EOF)
      return(FALSE);
   else
      return(TRUE);
}

l_bool ogg_stop_file(p_soundformat_item o)
{
   stop_ogg_stream(os);
   return(TRUE);
}

l_bool ogg_pause_file(p_soundformat_item o)
{
   // Not implemented in driver yet
   return(0);
}

l_bool ogg_forward_file(p_soundformat_item o, l_int relpos)
{
   // Not implemented in driver yet
   return(TRUE);
}

l_bool ogg_rewind_file(p_soundformat_item o, l_int relpos)
{
   // Not implemented in driver yet
   return(TRUE);
}

l_bool ogg_set_pos(p_soundformat_item o, l_int pos)
{
   // Not implemented in driver yet
   return(TRUE);
}

l_int ogg_get_pos(p_soundformat_item o)
{
   // Not implemented in driver yet
   return(0);
}

l_int ogg_get_len_file(p_soundformat_item o)
{
   // Not implemented in driver yet
}

l_bool ogg_get_file_info(p_soundformat_item o, SFAINFO *inf)
{
   // Not implemented in driver yet
   return(TRUE);
}

l_text ogg_get_friendly_name(p_soundformat_item o)
{
   return(_strdup(filename));
}

l_bool ogg_config_driver(p_soundformat_item o)
{
   return(ogg_display_about(o));
}

l_bool ogg_display_about(p_soundformat_item o)
{
   msgbox(MW_INFO, MB_OK, "Ogg Vorbis Driver for SFA\nVersion 1.0\n\nCopyright \xA9 Owen Rudge 2002. All Rights Reserved.\nOgg Vorbis libraries: Copyright \xA9 2002 Xiph.org Foundation\n\nUses AllegOgg by Aleksi Nurmi");
   return(0);
}

l_bool ogg_close_file(p_soundformat_item o)
{
   return(TRUE);
}

l_bool ogg_terminate_driver(p_soundformat_item o)
{
   return(TRUE);
}

l_bool ogg_send_custom_message(p_soundformat_item o, l_int msg, void *extra)
{
   return(FALSE);
}

//lib_exportable;
#include "sfaogg.exp"

SetInfoAppName("Ogg Vorbis Driver");
SetInfoDesciption("Ogg Vorbis Driver for SFA");
SetInfoCopyright("Copyright \xA9 Owen Rudge 2002");
SetInfoManufacturer("Owen Rudge");

lib_begin (void)
{
   if (ap_process == AP_ALLOC)
      AP_EXPORTLIB();

   if (ap_process == AP_INIT)
   {
      p_soundformat_item k;

      AP_EXPORTLIB();

      k = new_soundformat_item(                      ("ogg"),
                                                     ("Ogg Vorbis files"),
                                                     ("Copyright \xA9 Owen Rudge 2002"),
                                                     &ogg_init_driver,
                                                     &ogg_init_file,
                                                     &ogg_play_file,
                                                     &ogg_poll_file,
                                                     &ogg_stop_file,
                                                     &ogg_pause_file,
                                                     &ogg_forward_file,
                                                     &ogg_rewind_file,
                                                     &ogg_set_pos,
                                                     &ogg_get_pos,
                                                     &ogg_get_len_file,
                                                     &ogg_get_file_info,
                                                     &ogg_get_friendly_name,
                                                     &ogg_config_driver,
                                                     &ogg_display_about,
                                                     &ogg_close_file,
                                                     &ogg_terminate_driver,
                                                     &ogg_send_custom_message
                                                     );

      sound_format_list->insert(sound_format_list, k);
   }

   if (ap_process == AP_DONE)
   {

   }

   if (ap_process == AP_FREE)
   {

   }
} lib_end;
