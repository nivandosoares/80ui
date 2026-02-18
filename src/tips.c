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

/* Revision History (as of 04/05/2002):
 *
 * 08/01/2001: First created (Pablo Borobia)
 * 23/09/2001: Some changes (Julien Etelain)
 * 04/05/2002: Cleaned up code + small improvements (bjhall) 
 * 10/05/2002: Er, fixed dates of first two revision history entries and tiny code
 *             cleanup (orudge)
 */

#include <seal.h>


t_point tips_oldpos;
l_int   tips_count;

/*****************************************************************************/
/* FUNCTION:  tips_func_callback                                             */
/*                                                                           */
/* PURPOSE: the callback function when it has been called 5 times without    */
/*          the mouse being moved the tip is showed.                         */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* - o        p_object                                                       */
/*****************************************************************************/

void tips_func_callback (p_object o) 
{
   t_point m = VIEW(o)->get_global_point(VIEW(o), mouse->where);
   p_view tmp;

   // if mouse cursor is not moved increase tips_count by 1
   if (m.x == tips_oldpos.x && m.y == tips_oldpos.y)
      tips_count++; 
   else  // if mouse is moved reset tips_count to 0
   {
      tips_count = 0;
      tips_oldpos = m;
   }

   // below was 15, changed by bjhall
   if (tips_count == 5) // tips_count has reached 5 (1500 ticks) 
   {
      // get the view under the mouse
      tmp = desktop->get_view_under_mouse (desktop);

      if (tmp) 
      {
         while (tmp->get_view_under_mouse(tmp) != NULL) 
         {
            tmp = tmp->get_view_under_mouse(tmp);
         }

         // show the tip
         view_show_info_board (tmp);
      }
   }
}


/*****************************************************************************/
/* FUNCTION:  tips_init                                                      */
/*                                                                           */
/* PURPOSE:   Initializes a tip view.                                        */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* - o        The p_view that will become the tip                            */
/* - r        A rectangle for the tip                                        */
/*                                                                           */
/* RETURNS:   An p_view being the 'tip view'                                 */
/*                                                                           */
/*****************************************************************************/

p_view tips_init (p_view o, t_rect r) 
{
   if (!o)
      return NULL;
   
   view_init(o, r);
   o->transparent = FALSE;

   OBJECT(o)->process_tick = 300;  // was 100, changed by bjhall
   OBJECT(o)->func_callback = &tips_func_callback;
   OBJECT(o)->set_options(OBJECT(o), OB_OF_SELECTABLE + OB_OF_ENABLE, false);
   OBJECT(o)->set_options(OBJECT(o), OB_OF_STILLPROCESS, true);

   return(o);
}

/*****************************************************************************/
/* FUNCTION:  tips_system_init                                               */
/*                                                                           */
/* PURPOSE:   Initializes the tips-system.                                   */
/*                                                                           */
/*****************************************************************************/

void tips_system_init (void) 
{
   p_view o = tips_init(_malloc(sizeof(t_view)), rect_assign(0, 0, 0, 0));
   OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(o));
   o->hide(o);
}
