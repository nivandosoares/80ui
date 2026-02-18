/***********************************************************/
/*                                                         */
/*            desktop.h - Bad Desktop for Bad Seal         */
/*                                                         */
/*                       version 0.53                      */
/*                                                         */
/*                    Copyright (C) 2001                   */
/*          by Kostas Michalopoulos aka Bad Sector         */
/*                                                         */
/* e-mail: michalopoylos@hotmail.com                       */
/*                                                         */
/***********************************************************/

#ifndef __DESKTOP_H_INCLUDED__
#define __DESKTOP_H_INCLUDED__


/* desktop menu texts */

#ifndef TXT_CANTRUNFILE
#define TXT_CANTRUNFILE        INI_TEXT("Unable to run file")
#endif
#ifndef TXT_PROGRAMS
#define TXT_PROGRAMS           INI_TEXT("Programs")
#endif
#ifndef TXT_SETTINGS
#define TXT_SETTINGS           INI_TEXT("Settings")
#endif
#ifndef TXT_CLOSEWIN
#define TXT_CLOSEWIN           INI_TEXT("Close Window")
#endif
#ifndef TXT_SHUTDOWN
#define TXT_SHUTDOWN           INI_TEXT("Shut Down")
#endif
#ifndef TXT_REFRESHDESKTOP
#define TXT_REFRESHDESKTOP     INI_TEXT("Refresh Desktop")
#endif
#ifndef TXT_RESTART
#define TXT_RESTART            INI_TEXT("Restart")
#endif
#ifndef TXT_AREYOUSUREYOUWANTTOQUITSEAL
#define TXT_AREYOUSUREYOUWANTTOQUITSEAL INI_TEXT("Are you sure you want to exit SEAL?")
#endif

/* t_desktopline */
typedef struct t_desktopline *p_desktopline;

typedef struct t_desktopline {

   /* inherited functions / variabes from t_view and t_object objects */
   struct t_view     obclass;

} t_desktopline;


#define DESKTOPLINE(o) ((p_desktopline)(o))


/* indicates if it can be drawed under desktop line. 1 = it is, 0 = it isn't  */
extern l_int            draw_under_line;

/* desktop line. it's used for inserting to desktopline of the Seal. Use this value
   for inserting to an object.
*/
extern p_desktopline  desktopline;

/* t_desktopline */

/*
 init standard desktop line. it's used for internal desktop.dlx using.
*/
p_desktopline   _desktopline_init ( p_desktopline o );

extern p_desktopline  (*desktopline_init) ( p_desktopline o );

/*
 an equivalent of "l_dword t_object.valid ( p_object o, l_dword msg )" function.
 it's called, whenever the message MSG_QUIT is called. control quiting of the Seal.
*/
l_dword   program_valid ( p_object o, l_dword msg );

/*
 an equivalent of "t_rect t_view.size_limits ( p_view o )" function. set limits of
 desktop line, where can be sub-objects inserted.
*/
t_rect    desktopline_size_limits ( p_view o );

/*
 an equivalent of "void t_view.draw ( p_view o )" function.
 draws desktopline.
*/
void      desktopline_draw ( p_view o );

/*
 load menu from the path. make new array of t_menuitem(s)
*/
p_menuitem  menu_load_from_dir ( l_text path, l_text filename );

/*
 ...used for internal desktop.dlx. translate event, whenever an item from
 "menu_load_from_dir" is pressed.
*/
void        filemenu_trans ( p_object o, p_event event );


/* userbox functions */

/* this function adds an object to userbox. object's height must be maximum to 23 */
void add_to_userbox(p_object o);

/* this function removes an object from userbox */
void remove_from_userbox(p_object o);

#endif

