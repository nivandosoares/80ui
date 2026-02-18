/********************************************************************\
|     For licence and copyright information have a look to x.c       |
\********************************************************************/

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
