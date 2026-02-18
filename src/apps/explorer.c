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

/* Revision History (as of 05/05/2002):
 *
 * 30/03/2002 - Added About dialog (orudge)
 * 05/05/2002 - Added menus + cleaned the code (bjhall)
 * 10/05/2002 - Small code cleanup changes (orudge)
 */

#include <seal.h>
#include <app.h>
#include <button.h>
#include <menus.h>
#include <treeview.h>
#include <dialogs.h>
#include <iodlg.h>



p_treeview tree = NULL;
p_filelistbox dir = NULL;
p_appwin win = NULL;


/*****************************************************************************/
/* FUNCTION:  trans_ev                                                       */
/*                                                                           */
/* PURPOSE:   Translate the events, and do what should be done               */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* - o        The object                                                     */
/* - event    The event                                                      */
/*****************************************************************************/

static void trans_ev (p_object o, p_event event)
{
   BITMAP *ico;

   if (o->process == PH_PREPROCESS && event->type & EV_MESSAGE) 
   {
      switch (event->message) 
      {
         case MSG_UPDATE: 
            if (tree && tree->selected)
            {
               l_text path = _strdup(tree->selected->name);
               dir->load_list(dir, path, dir->file);
               clear_event(event);
            }

            break;
         
         case MSG_ABOUT:
            ico = load_image("bmp/icons/dir.ico,32");
            seal2_app_about("Explorer", ico);
            destroy_bitmap(ico);

            clear_event(event);
            break;
         
         case MSG_HELP: 
            open_help("./help/explorer.hlp");
            clear_event(event);

            break;
      }
   }
}


/*****************************************************************************/
/* FUNCTION:  dcfilelistbox_translate_event                                  */
/*                                                                           */
/* PURPOSE:   Translate the events coming from the listbox                   */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* - o        The filelist object                                            */
/* - event    The event                                                      */
/*****************************************************************************/

void dcfilelistbox_translate_event (p_object o, p_event event)
{
   RETVIEW(o, event);
   filelistbox_translate_event(o, event);

   if (event->type & EV_MESSAGE) 
   {
      switch(event->message) 
      {
         case MSG_OK:  
         {
            l_text file = FILELISTBOX_ITEM_FILENAME(LISTBOX (o)->list, LISTBOX (o)->current);
            l_text path = FILELISTBOX_ITEM_PATH(LISTBOX (o)->list, LISTBOX (o)->current);

            l_text runfile = io_realpath(path, file);

            clear_event(event);

            if (!run_file(runfile)) 
            {
               seal_error(ERR_INFO, "%s\n\n%s", TXT_CANTRUNFILE, runfile);
            }

            _free(runfile);
         }

         break;
      }
   }
}


/*****************************************************************************/
/* FUNCTION:  create_menu                                                    */
/*                                                                           */
/* PURPOSE:   Creates the top-menus                                          */
/*                                                                           */
/* RETURNS:   A p_menu containing the menu created                           */
/*                                                                           */
/*****************************************************************************/

static p_menu create_menu (void) 
{
   p_menu menu;

   menu = new_menu (
             new_sub_menu ("File", new_menu (
                     new_menu_item (TXT_EXIT, "", 0, MSG_CLOSE, NULL, 
                     NULL)),
                    
             new_sub_menu (TXT_HELP, new_menu (
                     new_menu_item (TXT_HELP, "", 0, MSG_HELP, NULL,
                     new_menu_line (
                     new_menu_item (TXT_ABOUT, "", 0, MSG_ABOUT, NULL, NULL)
                     ))), NULL)));
   return(menu);
}


/*****************************************************************************/
/* FUNCTION:  init_window                                                    */
/*                                                                           */
/* PURPOSE:   Initializes the main window and all the objects in it.         */
/*                                                                           */
/*****************************************************************************/

void init_window (void)
{
   p_treeitem it;
   t_rect r = rect_assign (0, 0, 600, 300);
   p_menuview appmenu;

   win = appwin_init (_malloc (sizeof (t_appwin)), r, "SEAL Explorer", WF_MINIMIZE|WF_MAXIMIZE, ap_id, &trans_ev);
   win->menu = standard_system_menu_items (win, new_menu_line (new_menu_item ("About Explorer...", 0, 0, MSG_ABOUT, NULL, NULL)));

   if (win)
   {
      VIEW(win)->align |= TX_ALIGN_CENTER;
   }

   OBJECT(desktop)->insert(OBJECT (desktop), OBJECT (win));

   // create the treeview and the filelist box
   if (win) 
   {
      tree = treeview_init(_malloc(sizeof(t_treeview)), rect_assign(5, 48, (r.b.x / 2) - 20 , r.b.y - 5));
      VIEW(tree)->align |= TX_ALIGN_BOTTOM;

      OBJECT(win)->insert(OBJECT(win), OBJECT(tree));
 
      dir = filelistbox_init(_malloc(sizeof(t_filelistbox)), rect_assign(r.b.x / 2, 48, r.b.x - 5 , r.b.y - 20), 2, LF_SELECTABLE|FL_OPEN, _strdup("/"), _strdup("*.*"), FA_ALL, NULL);
      VIEW(dir)->align |= TX_ALIGN_BOTTOM|TX_ALIGN_RIGHT;

      OBJECT(win)->insert(OBJECT(win), OBJECT(dir));
      OBJECT(dir)->translate_event = &dcfilelistbox_translate_event;
 
      tree->add(tree, _strdup("root"), _strdup("/"), _strdup(TXT_COMPUTER), IMG_DIR16, true, NULL);
      tree->load_from_dir(tree, _strdup("/"), _strdup("/"));
   }

   // create the menus on top of the window
   if (win)
   {
      r = VIEW(win)->size_limits(VIEW(win));
      r = rect_assign(r.a.x, r.a.y + 1, r.a.x, r.a.y + 20);
      appmenu = hormenu_init(_malloc (sizeof (t_menuview)), r, create_menu());
      OBJECT(win)->insert(OBJECT(win), OBJECT(appmenu));
   }

}

SetInfoAppName("Explorer");
SetInfoDescription("SEAL Explorer application");
SetInfoCopyright ("Copyright (c) SEAL Developers 2001-2002. All Rights Reserved.");
SetInfoManufacturer ("SEAL Developers");

app_begin (void)
{
   if (ap_process == AP_INIT) /* When app start */
   {
      AP_SETNUMOFCALLS(1); /* Set max number of calls */
      init_window(); /* Run the initialisation function */
   }

   if (ap_process == AP_FREE)
   {
   }
  
   if (ap_process == AP_DONE) /* When app done */
   {
   }
} app_end;
