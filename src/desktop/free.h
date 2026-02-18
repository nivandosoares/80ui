#ifndef __DISKUSED_H__
#define __DISKUSED_H__

#include <stdio.h>
#include <dos.h>

typedef struct t_idisk
{
   long free;
   long total;
} t_idisk;

t_idisk getpercentused(int disk);

#endif
