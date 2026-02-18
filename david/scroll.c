#include "scroll.h"



pScrollbutton (*ScrollbuttonInit) (pScrollbutton object, t_rect rectangle, l_bool up) = &_ScrollbuttonInit;
pScrollerbar (*ScrollerbarInit) (pScrollerbar object, t_rect rectangle, l_bool vertical) = &_ScrollerbarInit;
pScrollbar (*ScrollbarInit) (pScrollbar object, t_rect rectangle, l_bool vertical) = &_ScrollbarInit;

void initbuttons(pScrollbar object);

/*

      Here starts all the scrollbutton specific stuff...

*/

void ScrollbuttonTranslateEvent(p_object object, p_event event)
{
   if(event->type == EV_MESSAGE)
	{
      switch(event->message)
		{
/*         case MSG_SCROLL:
         {

            clear_event(event);
         } break;
*/         case MSG_UPDATE:
         {

            clear_event(event);
         } break;
      }
   }
}

void ScrollbuttonDraw(p_view view)
{

}

pScrollbutton _ScrollbuttonInit (pScrollbutton object, t_rect rectangle, l_bool up)
{

}

/*

      Here starts all the scrollerbar specific stuff...

*/

void ScrollerbarTranslateEvent(p_object object, p_event event)
{
   if(event->type == EV_MESSAGE)
	{
      switch(event->message)
		{
/*         case MSG_SCROLL:
         {

            clear_event(event);
         } break;
  */       case MSG_UPDATE:
         {

            clear_event(event);
         } break;
      }
   }
}

void ScrollerbarDraw(p_view view)
{

}

pScrollerbar _ScrollerbarInit (pScrollerbar object, t_rect rectangle, l_bool vertical)
{

}

/*

      Here starts all the scrollbar specific stuff...

*/

void ScrollbarTranslateEvent(p_object object, p_event event)
{
   if(event->type == EV_MESSAGE)
	{
      switch(event->message)
		{
/*         case MSG_SCROLL:
         {

            clear_event(event);
         } break;
  */       case MSG_UPDATE:
         {

            clear_event(event);
         } break;
      }
   }
}

void ScrollbarDraw(p_view view)
{
   t_rect rectangle = view->get_local_extent(view);
   t_point point;
   l_text SkinItem;

   BITMAP *out = view->begin_paint(view, &point, rectangle);

   if(out)
   {
      if(SKINFOR(view))
      {
         if(SCROLLBAR(view)->vertical)
            SkinItem = strdup("VScrollBarBackground");
         else
            SkinItem = strdup("HScrollBarBackground");
         DRAW_SKIN_RECT(out, SKINFOR(view), SkinItem, rect_move(rectangle, point.x, point.y));
         free(SkinItem);
      }
      else
      {
      }
   }
   view->end_of_paint(view, rectangle);
   if(!OBJECT(view)->last)
      initbuttons(SCROLLBAR(view));
}

void ScrollButtoDrawState(p_button object, l_int press)
{
   t_rect  r = VIEW(object)->get_local_extent(VIEW(object));
   t_rect  safe = r;
   t_point p;
   p_object parent = OBJECT(object)->owner;

   BITMAP *out = VIEW(object)->begin_paint(VIEW(object), &p, safe);

   if(out && parent->is_state(parent, OB_SF_VISIBLE))
   {
      if (SKINFOR(object))
      {
         l_text skinitem = NULL;

         if(object->message == MSG_UP)
         {
            if (press)
               skinitem = strdup("ScrollButtonUp");
            else
            {
               skinitem = strdup("ScrollButtonUp");
            }
         }
         else if(object->message == MSG_DOWN)
            {
            if (press)
               skinitem = strdup("ScrollButtonDown");
            else
            {
               skinitem = strdup("ScrollButtonDown");
            }
         }
         else if(object->message == MSG_LEFT)
            {
            if (press)
               skinitem = strdup("ScrollButtonLeft");
            else
            {
               skinitem = strdup("ScrollButtonLeft");
            }
         }
         else if(object->message == MSG_RIGHT)
            {
            if (press)
               skinitem = strdup("ScrollButtonRight");
            else
            {
               skinitem = strdup("ScrollButtonRight");
            }
         }
         DRAW_SKIN_RECT_EX(out, SKINFOR(object), skinitem, rect_move(r, p.x, p.y), VIEW(object)->font, object->caption);
         free(skinitem);
      }
      else
      {
         l_bool idwn = (press == -1 && object->flags & BF_UNDERSEL) || press == 1;
         l_color fcolor = color_3d_text;

         if (!OBJECT(object)->is_options(OBJECT(object), OB_OF_ENABLE)) /* disable button */
            fcolor = color_3d_shadow;

         VIEW(object)->background(VIEW(object), out, rect_move(r, p.x, p.y));

         if (OBJECT(object)->is_state(OBJECT(object), OB_SF_FOCUSED))
         {
            rect(out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, color_3d_border);
            r = rect_assign(r.a.x+1, r.a.y+1, r.b.x-1, r.b.y-1);
         }

         textout_draw_rect(out, VIEW(object)->font, object->caption, -1, r.a.x+p.x+2+(idwn?1:0), r.a.y+p.y+2+(idwn?1:0),
                                r.b.x+p.x-2+(idwn?1:0), r.b.y+p.y-2+(idwn?1:0), TX_ALIGN_CENTER, fcolor, TX_NOCOLOR, 0);

         if (!press)
         {
            if (object->flags & BF_DARROUND)
               button3d(VIEW(object), out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, 0);
            else if (!(object->flags & BF_PUSH))
            {
               if (!(object->flags & BF_UNDERSEL) || object->flags & BF_OVER)
                  button3d(VIEW(object), out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, 0);
            }
            else if(idwn)
               button3d(VIEW(object), out, r.a.x+p.x, r.a.y+p.y, r.b.x+p.x, r.b.y+p.y, 1);
         }
      }
   }

   VIEW(object)->end_of_paint(VIEW(object), safe);
}

void initbuttons(pScrollbar object)
{
   t_rect r = rect_assign(1, 1, 1 + SCROLL_B_SIZE, SCROLL_B_SIZE);
   p_button button;

   if(object->vertical)
      button = button_init(_malloc(sizeof(t_button)), r, "", MSG_UP, BF_NORMAL);
   else
      button = button_init(_malloc(sizeof(t_button)), r, "", MSG_LEFT, BF_NORMAL);
   button->draw_state = &ScrollButtoDrawState;
   VIEW(button)->hide(VIEW(button));
   OBJECT(object)->insert(OBJECT(object), OBJECT(button));


   r = rect_assign(150, 1,  150 + SCROLL_B_SIZE, SCROLL_B_SIZE);
   if(object->vertical)
      button = button_init(_malloc(sizeof(t_button)), r, "", MSG_DOWN, BF_NORMAL);
   else
      button = button_init(_malloc(sizeof(t_button)), r, "", MSG_RIGHT, BF_NORMAL);
   button->draw_state = &ScrollButtoDrawState;
   VIEW(button)->hide(VIEW(button));
   OBJECT(object)->insert(OBJECT(object), OBJECT(button));
}

pScrollbar _ScrollbarInit(pScrollbar object, t_rect rectangle, l_bool vertical)
{
   t_point p = rect_size(rectangle);
   t_rect r = rect_assign(rectangle.a.x, rectangle.a.y,  rectangle.b.x, rectangle.b.y);
   p_button button;

   if(!object)
      return NULL;
   clear_type(object, sizeof(tScrollbar));

   view_init(VIEW(object), rectangle);

   OBJECT(object)->translate_event = &ScrollbarTranslateEvent;
   OBJECT(object)->set_state(OBJECT(object), OB_SF_VISIBLE, false);
   VIEW(object)->draw              = &ScrollbarDraw;
   object->vertical = vertical;

   return object;
}
