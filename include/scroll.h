#ifndef __SCROLL_H_INCLUDED__
#define __SCROLL_H_INCLUDED__

#include "seal.h"
#include "dialogs.h"
#include "skin.h"

#define  MSG_UP                100150
#define  MSG_DOWN              100151
#define  MSG_LEFT              100152
#define  MSG_RIGHT             100153
#define  MSG_UPDATE            100154

#define SCROLL_SIZE            18
#define SCROLL_B_SIZE          16


#ifdef __cplusplus
extern "C" {
#endif

typedef struct tScrollbutton *pScrollbutton;

typedef struct tScrollbutton
{
   struct t_view obclass;

   l_bool up;
   l_int delta;

} tScrollbutton;

void    ScrollbuttonDraw(p_view view);
pScrollbutton _ScrollbuttonInit (pScrollbutton object, t_rect rectangle, l_bool up);
extern  pScrollbutton (*ScrollbuttonInit) (pScrollbutton object, t_rect rectangle, l_bool up);

typedef struct tScrollerbar *pScrollerbar;

typedef struct tScrollerbar
{
   struct t_view obclass;

   l_bool vertical;
} tScrollerbar;

void    ScrollerbarDraw(p_view view);
pScrollerbar _ScrollerbarInit (pScrollerbar object, t_rect rectangle, l_bool vertical);
extern  pScrollerbar (*ScrollerbarInit) (pScrollerbar object, t_rect rectangle, l_bool vertical);

typedef struct tScrollbar *pScrollbar;

typedef struct tScrollbar
{
   struct t_view obclass;
   l_bool vertical;

   l_int clientSize;

   /* This is the size of the area that you want
      to scroll, it is not the scrollers own size.
   */
   void (*setClientSize)(l_int size);

   /* Use this function for callback to your control.
      When the scrollbar is moved this function will
      be called with the new position. */
   void (*scrollCallback)(l_int position);

} tScrollbar;
#define SCROLLBAR(object)   ((pScrollbar)(object))

void     ScrollbarSetClientSize(l_int size);

void    ScrollbarDraw(p_view view);
pScrollbar _ScrollbarInit (pScrollbar object, t_rect rectangle, l_bool vertical);
extern  pScrollbar (*ScrollbarInit) (pScrollbar object, t_rect rectangle, l_bool vertical);

#ifdef __cplusplus
}
#endif

#endif
