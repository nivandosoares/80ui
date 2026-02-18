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
 * 29/04/2002: Cleaned up some code (xduffy)
 *
 * 09/05/2002: Code cleaning finished, all objects functions are now documented.
 *             Feel free to update/clearify them, but don't forget to add that
 *             to the revision history. (xduffy)
 *
 * 10/05/2002: Made a few modifications to code layout, etc (orudge)
 *
 */

#include <seal.h>
#include <object.h>

/* FIFO = First In First Out */
#ifndef FIFO_BUFFER_SIZE
   #define FIFO_BUFFER_SIZE   512
#endif

/**********************************************/
/* FUNCTION: _afree                           */
/*                                            */
/* PURPOSE: Frees the memory location the     */
/*          pointer points to.                */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p - The pointer that we're using.        */
/**********************************************/
void _afree(void **p)
{
   if (p)
   {
      if (*p)
         _free(*p);

      *p = NULL;
   }
}

/* Time functions */

/*static*/ l_big atimer = 0;

void aclock(void)
{
   atimer += 20;
}

l_big time_get_mili(void)
{
   return(_time_get_mili());
}

l_big time_diff_mili(l_big mili)
{
   return(_time_diff_mili(mili));
}

// first in first out //////////////////////////////////////////////////////////

static char global_buf[FIFO_BUFFER_SIZE];

l_ptr fifo_add(l_ptr list, l_ptr src, l_int size, l_int where)
{
   where = max(where, 0);

   if (size <= 0)
      return(NULL);
   
   if (!list)
   {
      list = _malloc(size);
      where = 0;
   }
   else
      list = _realloc(list, size*(where+1));

   if (list)
      memcpy((l_ptr)((l_long)list+(where*size)), src, size);

   return(list);
}

l_ptr fifo_get(l_ptr *list, l_int size, l_int where)
{
   if (!list || (size <= 0))
      return(NULL);

   where = max(where, 0);

   if (*list)
   {
      if (size > FIFO_BUFFER_SIZE)
      {
         DEBUG_printf("fifo buffer overflowed\n");
         exit(1);
      }

      memcpy(global_buf, (l_ptr)((l_long)(*list)+(where*size)), size);

      if(where > 0)
         *list = _realloc(*list, size*where);
      else
      {
         _free(*list);
         *list = NULL;
      }

      return((l_ptr)(global_buf));
   }

   return(NULL);
}

/* object's functions (in C++ this would be the class's methods)*/

/**********************************************/
/* FUNCTION: is_my_object                     */
/*                                            */
/* PURPOSE: Compare two objects and return    */
/*          TRUE if they are the same.        */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The first object to compare  */
/* - p_object2 - The second object to compare */
/**********************************************/
l_bool is_my_object(t_object *p_object1, t_object *p_object2)
{
   t_object *buffer = NULL;

   if (p_object2)
   {
      if (p_object2 == p_object1)
         return(true);
      else
      {
         /* pObject->last refers to the last child of pObject1 */
         p_object1 = p_object1->last;

         buffer = p_object1;

         if (p_object1)
         {
            /* pObject->prefere->prev = pObject->last got it?
               They're all linked together */
            do
            {
               if (is_my_object(p_object1, p_object2))
                  return(true);

               p_object1 = p_object1->next;
            } while (p_object1 != buffer);
         }
      }
   }

   return(false);
}

/**********************************************/
/* FUNCTION: is_active                        */
/*                                            */
/* PURPOSE: Tests if object is active, return */
/*          TRUE if so                        */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to test           */
/**********************************************/
l_bool is_active(t_object *p_object1)
{
   while (p_object1)
   {
      if (IS_ACTIVE_PROCESS(p_object1) && !p_object1->is_options(p_object1, OB_OF_NOTACTIVATE))
         return(true);

      p_object1 = p_object1->owner;
   }

   return(false);
}

/* object functions */

/**********************************************/
/* FUNCTION: obj_done                         */
/*                                            */
/* PURPOSE: Waste's the object and eventuall  */
/*          child-objects                     */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to waste          */
/**********************************************/
l_bool obj_done(p_object p_object1)
{
   t_object *p_last = NULL;
   t_object *p_next = NULL;
   t_object *owner = NULL;

   if (obj_exist(p_object1) > 0)
   {
      p_last = p_object1->last;

      done_stillprocess(p_object1); /* get out from list of still-processing objects */
                            /* it's also in obj_remove, but... */

      if (p_last) /* if there is sub object */
      {
         do
         {
            p_next = p_last->next;
            dispose(p_object1->last);
            p_last = p_next;
         } while (p_object1->last);
      }

      if (p_object1->owner)
      {
         owner = p_object1->owner;
         owner->remove(owner, p_object1);
         owner->reset_prefer(owner);
      }

      p_object1->tag = TAG_DISPOSE;

      return(true);
   }

   return(false);
}

/**********************************************/
/* FUNCTION: obj_index_of                     */
/*                                            */
/* PURPOSE: Returns which index a child object*/
/*          have.                             */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_parent - The parent object             */
/* - p_child  - The child object which index  */
/*              we want                       */
/**********************************************/
l_long obj_index_of(t_object *p_parent, t_object *p_child)
{
   /* Getting the first child-object */
   t_object *first = p_parent->first(p_parent);
   t_object *buffer = first;
   l_long i = 0;
   
   if (!buffer || !p_child)
      return 0;

   do
   {
      if (buffer == p_child)
         return(i);

      buffer = buffer->next;
      i++;
   } while(buffer != first);

   return(0);
}

/**********************************************/
/* FUNCTION: obj_at                           */
/*                                            */
/* PURPOSE: Returns which index a child object*/
/*          have.                             */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_parent - The parent object             */
/* - p_child  - The child object which index  */
/*              we want                       */
/**********************************************/
t_object *obj_at(t_object *p_object1, l_long index)
{
   t_object *first = p_object1->first(p_object1);
   t_object *buffer = first;
   l_long i = 0;

   if (!buffer || index < 0)
      return(NULL);

   do
   {
      if (i == index)
         return buffer;

      i++;

      buffer = buffer->next;
   } while ((buffer != first) && (i <= index));

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_find_match                   */
/*                                            */
/* PURPOSE: Searches and returns an object    */
/*          that corresponds to the given     */
/*          state and options.                */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to start from     */
/* - state     - The required state           */
/* - options   - The required options         */
/* - search_forward - If this one is true then*/
/*                    search using next       */
/*                    otherwise use prev      */
/**********************************************/
t_object *obj_find_match(t_object *p_object1, l_dword state, l_dword options, l_bool search_forward)
{
   t_object *buffer = p_object1;

   if (search_forward)
   {
      /* search forward */
      do
      {
         buffer = buffer->next;

         /* Checks that buffer have the correct values and if so return it */
         if (buffer->is_options(buffer, OB_OF_ENABLE) && (!state || buffer->is_state(buffer, state)) &&
            (!options || buffer->is_options(buffer, options)))
            return(buffer);
      } while (buffer != p_object1);
   }

   /* if not forward then backward :-) Almost BASIC :-P */
   do
   {
      buffer = buffer->prev;

      /* Checks that buffer have the correct values and if so return it */
      if (buffer->is_options(buffer, OB_OF_ENABLE) && (!state || buffer->is_state(buffer, state)) &&
         (!options || buffer->is_options(buffer, options)))
         return(buffer);
   } while(buffer != p_object1);

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_find_match_view              */
/*                                            */
/* PURPOSE: Searches and returns a view that  */
/*          corresponds to the given state    */
/*          and options.                      */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_view  - The object to start from       */
/* - state   - The required state             */
/* - options - The required options           */
/* - search_forward - If this one is true then*/
/*                    search using next       */
/*                    otherwise use prev      */
/**********************************************/
t_object *obj_find_match_view(p_object p_view, l_dword state, l_dword options, l_bool search_forward)
{
   p_object buffer = p_view;

   if (search_forward)
   {
      /* search forward */
      do
      {
         buffer = buffer->next_view(buffer);

      /* Checks that buffer have the correct values and if so return it */
         if ( buffer->is_options(buffer, OB_OF_ENABLE) && (!state || buffer->is_state(buffer, state)) &&
            (!options || buffer->is_options(buffer, options)) )
            return buffer;
      } while (buffer && buffer != p_view);
   }

   /* if not forward then backwards :-) Almost BASIC :-P */
   do
   {
      buffer = buffer->prev_view(buffer);

      /* Checks that buffer have the correct values and if so return it */
      if ( buffer->is_options(buffer, OB_OF_ENABLE) && (!state || buffer->is_state(buffer, state)) &&
         (!options || buffer->is_options(buffer, options)) )
         return(buffer);
   } while (buffer && buffer != p_view);

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_call_trans_events            */
/*                                            */
/* PURPOSE: At the moment this function       */
/*          doesn't do anything at all.       */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to start from     */
/* - state     - The required state           */
/* - options   - The required options         */
/**********************************************/
t_object *obj_call_trans_events(t_object *p_object1, l_dword state, l_dword options)
{

}

/**********************************************/
/* FUNCTION: obj_owner_view                   */
/*                                            */
/* PURPOSE: Returns the nearest parent view   */
/*          which is a view                   */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_view - The view to use                 */
/**********************************************/
t_object *obj_owner_view(t_object *p_view)
{
   if (p_view->owner)
   {
      if (p_view->owner->tag & TAG_VIEW)
         return(p_view->owner);
      else
         return((p_view->owner)->owner_view(p_view->owner));
   }

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_next_view                    */
/*                                            */
/* PURPOSE: Returns the next view that is     */
/*          a view                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_view - The view to use                 */
/**********************************************/
p_object obj_next_view(t_object *p_view)
{
   t_object *buffer = p_view;

   do
   {
      buffer = buffer->next;

      if (buffer->tag & TAG_VIEW)
         return(buffer);
   } while(buffer != p_view);

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_prev_view                    */
/*                                            */
/* PURPOSE: Returns the previous view that is */
/*          a view                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_view - The view to use                 */
/**********************************************/
t_object *obj_prev_view(t_object *p_view)
{
   t_object *buffer = p_view;

   do
   {
      buffer = buffer->prev;

      if (buffer->tag & TAG_VIEW)
         return(buffer);
   } while(buffer != p_view);

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_last_view                    */
/*                                            */
/* PURPOSE: Returns the last child-view that  */
/*          is a view                         */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_view - The view to use                 */
/**********************************************/
t_object *obj_last_view(t_object *p_view)
{
   t_object *p_child_next;

   if (!p_view->last)
      return(NULL);

   if (p_view->last->tag & TAG_VIEW)
      return(p_view->last);

   /* Gets another child-view when the last object wasn't a view */
   p_child_next = p_view->last->next_view(p_view->last);

   /* Check if there is a child-view left */
   if (p_child_next)
      return(p_child_next);

   /* Since there is child-view to p_view lets look one level deeper */
   return(p_view->last->last_view(p_view->last));
}

/**********************************************/
/* FUNCTION: obj_first_view                   */
/*                                            */
/* PURPOSE: Returns the first child-view that */
/*          is a view                         */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_view - The view to use                 */
/**********************************************/
t_object *obj_first_view(t_object *p_view)
{
   t_object *p_first;

   if (!p_view->first(p_view))
      return(NULL);

   if (p_view->first(p_view)->tag & TAG_VIEW)
      return(p_view->first(p_view));

   /* Gets another child-view when the first object wasn't a view */
   p_first = p_view->first(p_view)->prev_view(p_view->first(p_view));

   /* Check if there is a child-view left */
   if (p_first)
      return(p_first);

   /* Since there is child-view to p_view lets look one level deeper */
   return((p_view->first(p_view))->first_view(p_view->first(p_view)));
}

/**********************************************/
/* FUNCTION: obj_prev_view_to_first           */
/*                                            */
/* PURPOSE: Returns the previous view that is */
/*          a view. If p_view is already the  */
/*          first view then it returns NULL.  */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_view - The view to use                 */
/**********************************************/
t_object *obj_prev_view_to_first(t_object *p_view)
{
   if (p_view->owner)
   {
      if (p_view != p_view->owner->first_view(p_view->owner))
         return(p_view->prev_view(p_view));
   }

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_next_view_to_last            */
/*                                            */
/* PURPOSE: Returns the next view that is a   */
/*          view. If p_view is already the    */
/*          last view then it returns NULL.   */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_view - The view to use                 */
/**********************************************/
t_object *obj_next_view_to_last(t_object *p_view)
{
   if (p_view->owner)
   {
      if(p_view != (p_view->owner)->last_view(p_view->owner))
         return(p_view->next_view(p_view));
   }

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_prev_to_first                */
/*                                            */
/* PURPOSE: Returns the previous object, if   */
/*          not p_object1 is already the first*/
/*          view then it returns NULL.        */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to use            */
/**********************************************/
p_object obj_prev_to_first(t_object *p_object1)
{
   if (p_object1->owner)
   {
      if (p_object1 != p_object1->owner->first(p_object1->owner))
         return(p_object1->prev);
   }

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_next_to_last                 */
/*                                            */
/* PURPOSE: Returns the next object, if not   */
/*          p_object1 already is the last one,*/
/*          then it returns NULL.             */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to use            */
/**********************************************/
t_object *obj_next_to_last(t_object *p_object1)
{
   if (p_object1->owner)
   {
      if (p_object1 != p_object1->owner->last)
         return(p_object1->next);
   }

   return NULL;
}

/**********************************************/
/* FUNCTION: obj_setup                        */
/*                                            */
/* PURPOSE: Resets the owners prefer to the   */
/*          first of it's child-objects       */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to use            */
/**********************************************/
void obj_setup(t_object *p_object1)
{
   if (p_object1->owner)
      p_object1->owner->reset_prefer(p_object1->owner);
}

/**********************************************/
/* FUNCTION: obj_after_init                   */
/*                                            */
/* PURPOSE: Apparantly there is nothing to do */
/*          after the initialization          */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object that isn't used :)*/
/**********************************************/
void obj_after_init(t_object *p_object1)
{

}

/**********************************************/
/* FUNCTION: obj_select                       */
/*                                            */
/* PURPOSE: Selects the object, which means   */
/*          that it will be first in the      */
/*          linked list                       */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to select         */
/**********************************************/
l_bool obj_select(t_object *p_object1)
{
   /* The object must be selectable */
   if (!p_object1->is_options(p_object1, OB_OF_ENABLE) ||
       !p_object1->is_state(p_object1, OB_SF_VISIBLE) ||
       !p_object1->is_options(p_object1, OB_OF_SELECTABLE))
       return false;

   if (p_object1->is_options(p_object1, OB_OF_TOPSELECT) && p_object1->owner)
      /* Now putting p_object1 before the first object */
      p_object1->put_in_front_of(p_object1, p_object1->owner->first_view(p_object1->owner));
   else
      if (p_object1->owner) /* Makes p_object1 the prefered object */        
         p_object1->owner->set_prefer(p_object1->owner, p_object1);

   return(true);
}

/**********************************************/
/* FUNCTION: obj_put_into_stillprocess        */
/*                                            */
/* PURPOSE:                                   */
/*                                            */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 -                              */
/**********************************************/
int obj_put_into_stillprocess(p_object o, p_object s)
{
   if (o->owner)
      o = o->owner;

   o->put_into_stillprocess(o, s);
}

/**********************************************/
/* FUNCTION: obj_clear_from_stillprocess      */
/*                                            */
/* PURPOSE:                                   */
/*                                            */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* -                                          */
/**********************************************/
int obj_clear_from_stillprocess ( p_object o, p_object s )
{
   if (o->owner)
      o = o->owner;

   o->clear_from_stillprocess(o, s);
}

/**********************************************/
/* FUNCTION: obj_insert                       */
/*                                            */
/* PURPOSE: Inserting a child-object into a   */
/*          parent-object.                    */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_parent - The parent-object             */
/* - p_child - The child object to be inserted*/
/**********************************************/
t_object *obj_insert(t_object *p_parent, t_object *p_child)
{
   /* This doesn't do anything now, since after_init is empty */
   if (p_child)
      p_child->after_init(p_child);

   /* Here we check if child can be selected on top eg a window */
   if (p_parent->last && p_child && p_child->is_options(p_child, OB_OF_TOPSELECT))
      p_parent->insert_before(p_parent, p_child, p_parent->first(p_parent));
   else
      p_parent->insert_before(p_parent, p_child, NULL);

   if (p_child)
   {
      if (p_child->is_options(p_child, OB_OF_STILLPROCESS))
         init_stillprocess(p_child, p_child->process_tick);

      p_child->setup(p_child);
   }

   return(p_child);
}

/**********************************************/
/* FUNCTION: obj_insert_before                */
/*                                            */
/* PURPOSE: Inserts p_insert into p_parent    */
/*          as a child before the p_reference */
/*          object.                           */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_parent - The parent to the objects:    */
/* - p_insert - Object to insert              */
/* - p_reference - Inserted object.           */
/**********************************************/
t_object *obj_insert_before(t_object *p_parent, t_object *p_insert, t_object *p_reference)
{
   t_object *buffer = p_reference;
   
   /* Obvious */
   if (!p_insert)
      return NULL;

   p_insert->owner = p_parent;
   
   /* Here we make the right linkings, should be easy to follow */
   if (buffer)
   {
      buffer = p_reference->prev;
      p_insert->next = p_reference;
      buffer->next = p_insert;
      p_reference->prev = p_insert;
      p_insert->prev = buffer;
   }
   else
   {
      /* If p_insert is the only child-object to p_parent */
      if (!p_parent->last)
      {
         p_insert->next = p_insert;
         p_insert->prev = p_insert;
      }
      else
      {
         /* Making p_insert the first object, that's what
            p_parent->last->next is all about! */
         buffer = p_parent->last->next;
         p_insert->prev = p_parent->last;
         p_insert->next = buffer;
         buffer->prev = p_insert;
         p_parent->last->next = p_insert;
      }

      p_parent->last = p_insert;
   }

   return(p_insert);
}

/**********************************************/
/* FUNCTION: obj_put_in_front_of              */
/*                                            */
/* PURPOSE: Same as insert_before, but uses   */
/*          the references owner/parent,      */
/*          instead of asking for it.         */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_insert - Object to insert.             */
/* - p_reference - Inserted object.           */
/**********************************************/
void obj_put_in_front_of(t_object *p_insert, t_object *p_reference)
{
   t_object *parent = p_reference->owner;

   if (parent)
   {
      parent->set_prefer(parent, p_insert);
      parent->remove(parent, p_insert);
      parent->insert_before(parent, p_insert, p_reference);
   }
}

/**********************************************/
/* FUNCTION: obj_first                        */
/*                                            */
/* PURPOSE: Returns the objects first child.  */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_parent - The childs parent.            */
/**********************************************/
t_object *obj_first(t_object *p_parent)
{
   if (p_parent->last)
      return(p_parent->last->next);

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_set_state                    */
/*                                            */
/* PURPOSE: Sets the state of an object.      */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to modify.        */
/* - state - The state to set or to unset.    */
/* - set - Set the state or not.              */
/**********************************************/
void obj_set_state(t_object *p_object1, l_dword state, l_bool set)
{
   t_object *buffer = p_object1->first(p_object1);

   if (set)
      p_object1->state |= state;
   else
      p_object1->state &= ~state;

   if (state & OB_SF_FOCUSED) /* focus flag for sub selected object */
   {
      if(buffer)
      {
         buffer = buffer->find_match(buffer, set?OB_SF_SELECTED:OB_SF_FOCUSED, 0, true);
         /* Then this. NOTE that it's not the same buffer-object now */
         if (buffer)
            buffer->set_state(buffer, OB_SF_FOCUSED, set);
      }
   }
}

/**********************************************/
/* FUNCTION: obj_is_state                     */
/*                                            */
/* PURPOSE: Checks if the provided state is   */
/*          the same as the objects. Returns  */
/*          TRUE if so and FALSE if not.      */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to check.         */
/**********************************************/
l_bool obj_is_state(t_object *p_object1, l_dword state)
{
   return((l_bool)(p_object1->state & state));
}

/**********************************************/
/* FUNCTION: obj_set_options                  */
/*                                            */
/* PURPOSE:                                   */
/*                                            */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to modify.        */
/* - options - The options to apply.          */
/* - set - Apply options, TRUE or FALSE.      */
/**********************************************/
void obj_set_options(p_object p_object1, l_dword options, l_bool set)
{
   if (set)
      p_object1->options |= options;
   else
      p_object1->options &= ~options;
}

/**********************************************/
/* FUNCTION: obj_is_options                   */
/*                                            */
/* PURPOSE: Checks if certain options are     */
/*          on in the object.                 */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to check.         */
/* - options - The options to check.          */
/**********************************************/
l_bool obj_is_options(t_object *p_object1, l_dword options)
{
   return((l_bool)(p_object1->options & options));
}

/**********************************************/
/* FUNCTION: obj_remove                       */
/*                                            */
/* PURPOSE: Removes the child from the parents*/
/*          list. So references will be there.*/
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_parent - The parent-object.            */
/* - p_child - The child object to remove.    */
/**********************************************/
void obj_remove(t_object *p_parent, t_object *p_child)
{
   t_object* buffer = NULL;

   if (!p_parent->last || !p_child)
      return;

   if (p_child->next != p_child)
   {
      done_stillprocess(p_child);

      buffer = p_child->prev;
      buffer->next = p_child->next;
      buffer->next->prev = buffer;

      if (p_child == p_parent->last)
         p_parent->last = buffer;

      if (p_child == p_parent->prefer)
         p_parent->prefer = NULL;
   }
   else
   {
      done_stillprocess(p_child);
      p_parent->last = NULL;
      p_parent->prefer = NULL;
   }

   p_child->owner = NULL;
}

/**********************************************/
/* FUNCTION: obj_valid                        */
/*                                            */
/* PURPOSE: I don't know what this is for.    */
/*          Since it always will return zero. */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to start from     */
/* - msg - A message.                         */
/**********************************************/
l_dword obj_valid(t_object *p_object1, l_dword msg)
{
   t_object *first_child = p_object1->first(p_object1);
   t_object *buffer = first_child;

   if (first_child)
   {
      do
      {
         l_dword msg = buffer->valid(buffer, msg);

         if (msg)
            return(msg);
      } while (first_child != buffer);
   }

   return(0);
}

/**********************************************/
/* FUNCTION: obj_get_event                    */
/*                                            */
/* PURPOSE: Makes the top-object taking care  */
/*          of the event.                     */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to start from.    */
/* - event - The event.                       */
/**********************************************/
void obj_get_event(t_object *p_object1, t_event *event)
{
   if (p_object1->owner)
      p_object1 = p_object1->owner;

   p_object1->get_event(p_object1, event);
}

/**********************************************/
/* FUNCTION: obj_put_event                    */
/*                                            */
/* PURPOSE: Puts the event to the top-object. */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to start from.    */
/* - event - The event to send.               */
/**********************************************/
void obj_put_event(t_object *p_object1, t_event *event)
{
   if (p_object1->owner)
      p_object1 = p_object1->owner;

   p_object1->put_event(p_object1, event);
}

/**********************************************/
/* FUNCTION: obj_execute                      */
/*                                            */
/* PURPOSE: Executes an object.               */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to execute.       */
/**********************************************/
l_dword obj_execute(t_object *p_object1)
{
/*  t_event event;*/
   clear_event(&event_main);

   do
   {
      p_object1->end_state = 0;

      do
      {
         p_object1->get_event(p_object1, &event_main);
         INTMAIN(&event_main);
         PLAY_PROCESS(p_object1, &event_main);
      } while (!p_object1->end_state);
   } while (p_object1->valid(p_object1, p_object1->end_state) != 0);

   return(p_object1->end_state);
}

/**********************************************/
/* FUNCTION: obj_reset_prefer                 */
/*                                            */
/* PURPOSE: Resets the preferd child-object   */
/*          to the first one.                 */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object - The object which pointer will */
/*              will be reset.                */
/**********************************************/
void obj_reset_prefer(t_object *p_object1)
{
   /* Getting the first child object and set it as prefer.*/
   p_object1->set_prefer(p_object1, p_object1->first(p_object1));
}

/**********************************************/
/* FUNCTION: obj_set_prefer                   */
/*                                            */
/* PURPOSE: Sets the object to be prefere.    */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The parent object.           */
/* - prefer - The child object to become      */
/*            prefere.                        */
/**********************************************/
void obj_set_prefer(t_object *p_object1, t_object *prefer)
{
   #define defSelect(_p, _enable ) \
      if ( _p && (!_enable||(_p)->is_options(_p, OB_OF_SELECTABLE))    \
          && (_p)->is_options(_p, OB_OF_ENABLE))                       \
            (_p)->set_state(_p, OB_SF_SELECTED, _enable);
   #define  defFocus(_p, _enable ) \
      if ( _p && p_object1->is_state(p_object1, OB_SF_FOCUSED) && (!_enable||(_p)->is_options(_p, OB_OF_SELECTABLE)) \
           && (_p)->is_options(_p, OB_OF_ENABLE) ) \
        (_p)->set_state(_p, OB_SF_FOCUSED, _enable);

   if (p_object1->prefer != prefer)
   {
      defSelect(prefer, TRUE);
      defSelect(p_object1->prefer, FALSE);
      defFocus(prefer, TRUE);
      defFocus(p_object1->prefer, FALSE);
      p_object1->prefer = prefer;
   }
}

/**********************************************/
/* FUNCTION: obj_for_each_event               */
/*                                            */
/* PURPOSE: Sends the event to each child-obj.*/
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The parent object.           */
/* - event - The event to send.               */
/**********************************************/
void obj_for_each_event(t_object *p_object1, t_event *event)
{
   t_object *first_child = p_object1->first(p_object1);
   t_object *buffer = first_child;

   if (first_child)
   {
      do
      {
         if (IS_OKTOSUBPROCESS(buffer) && buffer != p_object1->prefer)
            PLAY_PROCESS(buffer, event);

         buffer = buffer->next;
      } while(buffer != first_child);
   }
}

/**********************************************/
/* FUNCTION: obj_play_process                 */
/*                                            */
/* PURPOSE: Plays a process with an event.    */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to play.          */
/* - event - The event to give the object.    */
/**********************************************/
void obj_play_process(t_object *p_object1, t_event *event)
{
   if (!p_object1)
      return;

   if (!IS_ACTIVE_PROCESS(p_object1) || p_object1->is_options(p_object1, OB_OF_NOTACTIVATE) || EV_IGNORE_ACTIVE_PROCESS(event->type))
   {
      ACTIVE_PROCESS(p_object1);
      event_stop = p_object1;
      p_object1->translate_event(p_object1, event);
      PASSIVE_PROCESS(p_object1);
   }
}

/**********************************************/
/* FUNCTION: obj_translate_event              */
/*                                            */
/* PURPOSE: Translates an event into action.  */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object we use.           */
/* - event - The event to translate.          */
/**********************************************/
void obj_translate_event(t_object *p_object1, t_event *event)
{
   if (p_object1->prefer && IS_OKTOSUBPROCESS(p_object1->prefer))
      PLAY_PROCESS(p_object1->prefer, event);

   p_object1->for_each_event(p_object1, event);

   /* For some reason the cut and paste code is here... */
   if (event->type & EV_MESSAGE)
   {
      switch (event->message)
      {
         case MSG_PASTE:
            if (p_object1 == event->obj)   /* paste data */
            {
               p_object1->set_data(p_object1, &clipboard); /* from clipboard */
               clear_event(event);
            }

            break;

         case MSG_COPY:
            if (p_object1 == event->obj)   /* copy data */
            {
               clipboard.style = DS_WHATEVER; /* get what object want to sent */
               p_object1->get_data(p_object1, &clipboard); /* to clipboard */
               clear_event(event);
            }

            break;

         case MSG_CUT:             /* cut data */
            if (p_object1 == event->obj)
            { 
               clipboard.style = DS_WHATEVER;
               p_object1->get_data(p_object1, &clipboard);
            }

            // no break - pass through to the delete routine

         case MSG_DELETE:
            if (p_object1 == event->obj)
            {
               t_data delclip;

               clear_type(&delclip, sizeof(t_data)); /* from object */
               delclip.style = DS_DELETE|DS_WHATEVER; /* delete what object want to */
               l_tag_cpy(delclip.id, p_object1->data_type); /* id of clipboard = o->data_type */
               p_object1->set_data(p_object1, &delclip);
               clear_event(event);
            }

            break;
      }
   }
}


/**********************************************/
/* FUNCTION: obj_get_data                     */
/*                                            */
/* PURPOSE: Currently this function does      */
/*          nothing. Translate_event takes    */
/*          care of cutting and pasting...    */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object...                */
/* - rec - The data to get...                 */
/**********************************************/
l_bool obj_get_data(t_object *p_object1, t_data *rec)
{
   return(false);
}

/**********************************************/
/* FUNCTION: obj_set_data                     */
/*                                            */
/* PURPOSE: Currently this function does      */
/*          nothing. Translate_event takes    */
/*          care of cutting and pasting...    */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object...                */
/* - rec - The data to set...                 */
/**********************************************/
l_bool obj_set_data(t_object *p_object1, t_data *rec)
{
   return(false);
}

/**********************************************/
/* FUNCTION: obj_select_data                  */
/*                                            */
/* PURPOSE: Used to select data in an object. */
/*          Now, it's here for compatability. */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object...                */
/* - data_style - As it says..                */
/* - set - To select or not select...         */
/**********************************************/
l_bool obj_select_data(t_object *p_object1, l_int data_style, l_bool set)
{
   return(false);
}

/**********************************************/
/* FUNCTION: dispose                          */
/*                                            */
/* PURPOSE: To dispose objects when they are  */
/*          done.                             */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to dispose.       */
/**********************************************/
t_object *dispose(t_object *p_object1)
{
   if (p_object1 && p_object1->done(p_object1))
      _free(p_object1);

   return(NULL);
}

/**********************************************/
/* FUNCTION: dispose_all                      */
/*                                            */
/* PURPOSE: Disposes the object and all of its*/
/*          children.                         */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to dispose.       */
/**********************************************/
void dispose_all(t_object *p_object1)
{
   t_object *last_child = OBJECT(p_object1)->last;
   t_object *buffer1 = last_child;
   t_object *buffer2 = NULL;

   if (obj_exist(p_object1) > 0)
   {
      if (last_child) /* if there is sub object */
      {
         do
         {
            buffer2 = buffer1->next;
            buffer1->done(buffer1);
            buffer1 = buffer2;
         } while(p_object1->last);
      
         p_object1->last = NULL;
         p_object1->tag = TAG_DISPOSE;
      }
   }
}

/**********************************************/
/* FUNCTION: init_stillprocess                */
/*                                            */
/* PURPOSE: If milliseconds is 0 or below the */
/*          the object will taken out of the  */
/*          list of stilled processes. If     */
/*          milliseconds is more than 0 the   */
/*          object will be initalized into    */
/*          stillprocess's                    */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to initalize      */
/* - l_int milliseconds - The number of       */
/*                        milliseconds to run */
/**********************************************/
void init_stillprocess(t_object *p_object1, l_int milliseconds)
{
   if (!p_object1)
      return;

   if (milliseconds > 0)
   {
      p_object1->process_tick = milliseconds;
      p_object1->set_options(p_object1, OB_OF_STILLPROCESS, true);
      p_object1->put_into_stillprocess(p_object1, p_object1);
   }
   else
   {
      if (p_object1->options & OB_OF_STILLPROCESS)
      {
         p_object1->process_tick = 0;
         p_object1->clear_from_stillprocess(p_object1, p_object1);
         p_object1->set_options(p_object1, OB_OF_STILLPROCESS, false);
      }
   }
}

/**********************************************/
/* FUNCTION: message_info                     */
/*                                            */
/* PURPOSE: Sets up and event and sends it to */
/*          the object.                       */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to play event on. */
/* - type - Type of event.                    */
/* - message - The events message.            */
/* - p_message_object - Originator.           */
/* - info - Additional information.           */
/**********************************************/
void message_info(t_object *p_object1, l_dword type, l_dword message, t_object *p_message_object, void *info)
{
   t_event event;

   if (p_object1)
   {
      set_event_info(&event, type, message, p_message_object, info);
      PLAY_PROCESS(p_object1, &event);
   }
}

/**********************************************/
/* FUNCTION: message_all_info                 */
/*                                            */
/* PURPOSE: Sends and event to all objects.   */
/*                                            */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - &program - SEAL's main program.          */
/* For the rest look at the function above... */
/**********************************************/
void message_all_info(l_dword type, l_dword message, t_object *p_message_object, void *info)
{
   message_info(&program, type, message, p_message_object, info);
}

/**********************************************/
/* FUNCTION: set_event                        */
/*                                            */
/* PURPOSE: Initializes an new event, and     */
/*          sets the values.                  */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - event - The events pointer.              */
/* - type - Type of event.                    */
/* - message - The events message.            */
/* - p_event_object - Originator.             */
/**********************************************/
void set_event(t_event *event, l_dword type, l_dword message, p_object p_event_object)
{
   t_event new_event;

   clear_event(&new_event);

   new_event.type = type;
   new_event.message = message;
   new_event.obj = p_event_object;

   (*event) = new_event;
}

/**********************************************/
/* FUNCTION: set_event_info                   */
/*                                            */
/* PURPOSE: Initializes an new event, and     */
/*          sets all the values.              */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - event - The events pointer.              */
/* - type - Type of event.                    */
/* - message - The events message.            */
/* - p_event_object - Originator.             */
/* - info - Additional information.           */
/**********************************************/
void set_event_info(t_event *event, l_dword type, l_dword message, p_object p_event_object, void *info)
{
   set_event(event, type, message, p_event_object);
   event->info = info;
}

/**********************************************/
/* FUNCTION: copy_type                        */
/*                                            */
/* PURPOSE: Copies a variable into a new one  */
/*          of the same kind.                 */
/*                                            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - to_copy - The variable to copy.          */
/* - size - The size of the variable.         */
/**********************************************/
l_ptr copy_type(l_ptr to_copy, l_long size)
{
   void *ptr = NULL;
 
   if (to_copy && size > 0)
   {   
      ptr = _malloc(size);

      if(ptr)
      {
         memcpy(ptr, to_copy, size);
         return(ptr);
      }
   }

   return(NULL);
}

/**********************************************/
/* FUNCTION: obj_func_nothing                 */
/*                                            */
/* PURPOSE: None.                             */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object                   */
/**********************************************/
static void obj_func_nothing(t_object *p_object1)
{

}

/**********************************************/
/* FUNCTION: _obj_init                        */
/*                                            */
/* PURPOSE: Initializes an object.            */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to initialize.    */
/**********************************************/
t_object *_obj_init(t_object *p_object1)
{
   if (!p_object1)
      return(NULL);

   memset(p_object1, 0, sizeof(t_object));

   p_object1->options = OB_OF_ENABLE;
   p_object1->tag = TAG_OBJECT;
   p_object1->process_tick = 20;

   p_object1->owner = NULL;

   p_object1->find_match_view = &obj_find_match_view;
   p_object1->find_match = &obj_find_match;

   p_object1->owner_view = &obj_owner_view;
   p_object1->put_in_front_of = &obj_put_in_front_of;

   p_object1->func_callback = &obj_func_nothing;
   p_object1->next_view = &obj_next_view;
   p_object1->prev_view = &obj_prev_view;
   p_object1->last_view = &obj_last_view;
   p_object1->first_view = &obj_first_view;
   p_object1->prev_to_first = &obj_prev_to_first;
   p_object1->next_to_last = &obj_next_to_last;
   p_object1->prev_view_to_first = &obj_prev_view_to_first;
   p_object1->next_view_to_last = &obj_next_view_to_last;

   p_object1->set_prefer = &obj_set_prefer;
   p_object1->reset_prefer = &obj_reset_prefer;

   p_object1->select = &obj_select;
   p_object1->set_state = &obj_set_state;
   p_object1->is_state = &obj_is_state;
   p_object1->set_options = &obj_set_options;
   p_object1->is_options = &obj_is_options;
   p_object1->get_data = &obj_get_data;
   p_object1->set_data = &obj_set_data;
   p_object1->select_data = &obj_select_data;

   p_object1->at = &obj_at;
   p_object1->index_of = &obj_index_of;

   p_object1->play_process = &obj_play_process;
   p_object1->translate_event = &obj_translate_event;
   p_object1->for_each_event = &obj_for_each_event;
   p_object1->first = &obj_first;
   p_object1->after_init = &obj_after_init;
   p_object1->setup = &obj_setup;
   p_object1->insert_before = &obj_insert_before;
   p_object1->insert = &obj_insert;
   p_object1->remove = &obj_remove;
   p_object1->done = &obj_done;
   p_object1->valid = &obj_valid;

   p_object1->put_into_stillprocess = &obj_put_into_stillprocess;
   p_object1->clear_from_stillprocess = &obj_clear_from_stillprocess;

   p_object1->get_event = &obj_get_event;
   p_object1->put_event = &obj_put_event;
   p_object1->execute = &obj_execute;

   return(p_object1);
}

/**********************************************/
/* FUNCTION: obj_init (variable)              */
/*                                            */
/* PURPOSE: A pointer to the real initializer */
/*                                            */
/* PARAMETERS:                                */
/*                                            */
/* - p_object1 - The object to initialize.    */
/**********************************************/
t_object *(*obj_init) ( t_object *p_object1 ) = &_obj_init;
