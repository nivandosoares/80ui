/*
*/

/* Revision history (as of 17/05/2002)
 *
 *
 */

#include "allegro.h"
#include "seal.h"
#include "menus.h"
#include "app.h"
#include "button.h"
#include "dialogs.h"
#include "iodlg.h"
#include "scroll.h"













void event_handler (p_object o, p_event event)
{
   t_rect r;
   if (event->type & EV_MESSAGE)
   {
       switch (event->message)
       {
           case MSG_RESIZE:
           {
             r = VIEW(o)->size_limits(VIEW(o));
             r = rect_assign(r.a.x+10, r.a.y+10, r.b.x-10, r.b.y-20);



           } break;
       }
   }
}

void init_splorer()
{
   t_rect r = rect_assign(0, 0, 300, 300);
   t_rect rectangle = rect_assign(50, 100, 250, 100 + SCROLL_SIZE);
   t_rect t = r;
   t_rect buf;

   /* make window */
   p_appwin o = appwin_init(_malloc(sizeof(t_appwin)),
                            r,
                            "New Scollbar test",
                            WF_MAXIMIZE|WF_MINIMIZE,
                            ap_id, /* application id */
                            &event_handler);

   pScrollbar scroll = ScrollbarInit(_malloc(sizeof(tScrollbar)), rectangle,
                                    false);

   OBJECT(o)->insert(OBJECT(o), OBJECT(scroll));

   /* set alignment */
   VIEW(o)->align |= TX_ALIGN_CENTER;
   /* insert it on the desktop */
   OBJECT(desktop)->insert(OBJECT(desktop), OBJECT(o));
}



app_begin (void)
{
  if (ap_process == AP_INIT)
  {
      if (ap_args)
      {

      }
      else
      {
      }

      init_splorer();

  }
  if (ap_process == AP_FREE)
  {
  }

} app_end;
