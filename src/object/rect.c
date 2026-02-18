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

/* Revision History (as of 29/04/2002):
 *
 * 29/04/2002: Cleaned up code (orudge)
 */

#include <seal.h>
#include <object.h>

/* Position and origin functions */

t_rect rect_empty = {{-0xFFFF, -0xFFFF},{ -0xFFFF, -0xFFFF}};

/*****************************************************************************/
/* FUNCTION:  point_assign                                                   */
/*                                                                           */
/* PURPOSE:   Returns a t_point structure referring to the specified points. */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* - x        The x (left) coordinate                                        */
/* - y        The y (top) coordinate                                         */
/*                                                                           */
/* RETURNS:   A t_point structure referring to the specified points.         */
/*****************************************************************************/

inline t_point point_assign (l_rect x, l_rect y)
{
   t_point tmp_point;

   tmp_point.x = x;
   tmp_point.y = y;

   return(tmp_point);
}

/*****************************************************************************/
/* FUNCTION:  rect_assign                                                    */
/*                                                                           */
/* PURPOSE:   Returns a t_rect structure referring to the specified          */
/*            co-ordinates.                                                  */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* - x1       The first x (left) coordinate                                  */
/* - y1       The first y (top) coordinate                                   */
/* - x2       The second x (right) coordinate                                */
/* - y2       The second y (bottom) coordinate                               */
/*                                                                           */
/* RETURNS:   A t_rect structure referring to the specified points.          */
/*****************************************************************************/

inline t_rect rect_assign (l_rect x1, l_rect y1, l_rect x2, l_rect y2)
{
  t_rect tmp_rect;

  tmp_rect.a.x = x1;
  tmp_rect.a.y = y1;
  tmp_rect.b.x = x2;
  tmp_rect.b.y = y2;

  return(tmp_rect);
}

/*****************************************************************************/
/* FUNCTION:  rect_move                                                      */
/*                                                                           */
/* PURPOSE:   Updates the co-ordinates in a t_rect structure by adding       */
/*            specified values to it.                                        */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* - rect     The t_rect structure to update                                 */
/* - x_inc    The number of pixels to add to the x co-ordinates              */
/* - y_inc    The number of pixels to add to the y co-ordinates              */
/*                                                                           */
/* RETURNS:   The t_rect structure passed to the function.                   */
/*****************************************************************************/

inline t_rect rect_move (t_rect rect, l_rect x_inc, l_rect y_inc)
{
   rect.a.x += x_inc;
   rect.a.y += y_inc;
   rect.b.x += x_inc;
   rect.b.y += y_inc;

   return(rect);
}

/*****************************************************************************/
/* FUNCTION:  rect_size                                                      */
/*                                                                           */
/* PURPOSE:   Calculates the width and height of the specified t_rect        */
/*            structure.                                                     */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* - rect     The t_rect structure to calculate the size of                  */
/*                                                                           */
/* RETURNS:   A t_point structure. The x member contains the width, and the  */
/*            y member contains the height.                                  */
/*****************************************************************************/

inline t_point rect_size (t_rect rect)
{
   t_point tmp_point;

   tmp_point.x = rect.a.x - rect.a.x;
   tmp_point.y = rect.b.y - rect.a.y;

   return(tmp_point);
}

/*****************************************************************************/
/* FUNCTION:  rect_overlay                                                   */
/*                                                                           */
/* PURPOSE:   Calculates if the first t_rect structure specified overlays    */
/*            the second.                                                    */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* - rect1    The t_rect structure which is checked to see if it fits on the */
/*            second.                                                        */
/*                                                                           */
/* - rect2    The t_rect structure which the first structure is checked      */
/*            against.                                                       */
/*                                                                           */
/* RETURNS:   true is returned if rect1 fits inside rect2, false otherwise.  */
/*****************************************************************************/

inline l_bool rect_overlay(t_rect rect1, t_rect rect2)
{
   if (rect1.a.x > rect2.b.x || rect1.a.y > rect2.b.y ||
       rect1.b.x < rect2.a.x || rect1.b.y < rect2.a.y )
      return(false);
   else
      return(true);
}

inline t_rect  rect_cliped ( t_rect r, t_rect d )
{
  if  ( !rect_overlay(r, d) ) return rect_empty;
  r.b.x = max(r.a.x, min(r.b.x, d.b.x));
  r.b.y = max(r.a.y, min(r.b.y, d.b.y));
  r.a.x = min(r.b.x, max(r.a.x, d.a.x));
  r.a.y = min(r.b.y, max(r.a.y, d.a.y));
  return r;
};



inline l_rect rect_sizex ( t_rect r ) {
  return r.b.x-r.a.x;
};


inline l_rect rect_sizey ( t_rect r ) {
  return r.b.y-r.a.y;
};


inline l_bool rect_check_empty ( t_rect r )
{
  if ( (r.a.x == rect_empty.a.x) && (r.b.x == rect_empty.b.x) &&
       (r.a.y == rect_empty.a.y) && (r.b.y == rect_empty.b.y) ) return true;
  return false;
};


inline l_bool rect_contains ( t_rect r, t_point p )
{
  if ( (r.a.x <= p.x) && (r.a.y <= p.y) && (r.b.x >= p.x) && (r.b.y >= p.y) )
    return true;
  return false;
};


inline l_bool  rect_equals ( t_rect r, t_rect nr )
{
  if ( (r.a.x != nr.a.x) || (r.a.y != nr.a.y) || (r.b.x != nr.b.x) ||
       (r.b.y != nr.b.y) ) return false;
  return true;
};


void  rect_double_overlay ( t_rect* fr, t_rect *lr )
{
  t_rect r   = *fr;
  t_rect xfr = *fr;
  t_rect xlr = *lr;
  if ( !rect_overlay(*fr, *lr) ) {
    *(fr) = rect_empty;
    *(lr) = rect_empty;
    return;
  };
  r = rect_cliped(r, *lr);
  if ( r.a.x > (*lr).a.x ) (*lr).b.x = r.a.x - 1;
  if ( r.b.x < (*lr).b.x ) (*lr).a.x = r.b.x + 1;
  if ( r.b.y < (*lr).b.y ) *fr = rect_assign(r.a.x, r.b.y + 1, r.b.x, (*lr).b.y);
  if ( r.a.y > (*lr).a.y ) *fr = rect_assign(r.a.x, (*lr).a.y, r.b.x, r.a.y - 1);
  if ( rect_equals(xfr, *fr) ) *fr = rect_empty;
  if ( rect_equals(xlr, *lr) ) *lr = rect_empty;
};
////////////////////////////////////////////////////////////////////////////////
