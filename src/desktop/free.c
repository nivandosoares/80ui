/******************************************************************
 * SEAL 2.0                                                       *
 * Copyright (c) 1999-2003 SEAL Developers. All Rights Reserved.  *
 *                                                                *
 * Web site: http://sealsystem.sourceforge.net/                   *
 * For e-mail (current maintainer) look in readme.txt             *
 ******************************************************************/

/*

      License: GPL
      Code originally written by David Fredin (xduffy)

*/
#include "free.h"

/*
 *  Function   :    getdiskfree
 *  Topics     :    Returns the number of bytes free of a disk.
 *  Parameters :    in int disk  0 = A:, 1 = B:
 *  Return code:    number of bytes free, -1 if invalid drive
 */

long getdiskfree(int disk)
{
   struct diskfree_t free;

   if(_dos_getdiskfree(disk + 1, &free))
      return -1;

   return (long) free.avail_clusters *
          (long) free.bytes_per_sector *
          (long) free.sectors_per_cluster;
}

long getdisktotal(int disk)
{
   struct diskfree_t total;

   if(_dos_getdiskfree(disk + 1, &total))
      return -1;

   return (long) total.total_clusters *
          (long) total.bytes_per_sector *
          (long) total.sectors_per_cluster;
}

t_idisk getpercentused(int disk)
{
   t_idisk driven;
   driven.free = getdiskfree(disk);
   driven.total = getdisktotal(disk);
   return driven;
}
