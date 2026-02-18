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

/* SEAL Printing API
   Version 0.2

   Programmed by Leon Pegg (aka Iota). LPT information by "Romance".
   SEAL interface by Owen Rudge, with parts by Leon Pegg.

   OCR: This needs improving A LOT! There should be a system like SFA
   OCR: for printers, which distributes printing requests to the appropriate
   OCR: low-level driver, which can be selected from a configuration dialog
   OCR: or something.
*/

#include <seal.h>
#include <stdio.h>

char port[PATH_MAX]; // could possibly be a file in future versions

l_bool print_text_from_buffer(l_text buf)
{
   FILE *print;

   print = fopen(port, "w");
   fprintf(print, "%s\n", buf);
   fclose(print);

   return(0);
}

l_bool print_text_from_file(l_text file)
{
   FILE *print, *f;
   int c;
   long loc = 0;
   int len = 0; 
   int wid = 0; 
   char buf[79];
   char *b = buf;

   print = fopen(port, "w");
   f = fopen(file, "r");
    
   if (!f)
      return NULL;

   while (!feof(f))
   {    
      c = fgetc(f);                 

      if (!wid)                 
      {    
         sprintf(buf,"%lx: ",loc);

         while (*b++)
         { 
            wid++;
         }

         b--;
      }

      *b++ = c;                
      len++; 
      wid++;            

      if (wid >= 79)
      { 
         *b = '\0';          
         fprintf(print, "%s\n",buf); 
         b = buf;            
         wid = 0;
      }

      loc += 1;                     
   }

   if(len > 4 && wid)              
   {    
      *b = '\0';                    
      fprintf(print, "%s\n",buf);   
   }
    
   fclose(print);
   return 0;
}

#include "print.exp"

lib_begin (void)
{
   switch (ap_process)
   {
      case AP_ALLOC:
         AP_EXPORTLIB();
         break;

      case AP_INIT:
         AP_EXPORTLIB();

         strcpy(port, "LPT1");
         break;

      case AP_DONE:
         break;

      case AP_FREE:
         break;
   }
} lib_end;

SetInfoAppName("SEAL Printing API");
SetInfoDesciption("SEAL Printing API");
SetInfoCopyright("Copyright (c) Leon Pegg, Owen Rudge 2002");
SetInfoManufacturer("Leon Pegg and Owen Rudge");

/*
	The meaning of life and the bits returned in the status byte
	NOTE:  Important - the sense of all bits are flipped such that
	if the bit is set, the condition is asserted.

Bits----------------------------
   7   6   5   4   3   2   1   0
   |   |   |   |   |   |   |   +-- unused
   |   |   |   |   |   |   +------ unused
   |   |   |   |   |   +---------- unused
   |   |   |   |   +-------------- 1 = i/o error
   |   |   |   +------------------ 1 = selected
   |   |   +---------------------- 1 = out of paper
   |   +-------------------------- 1 = acknowledge
   +------------------------------ 1 = not busy

 Port architecture:

Each LPT port starts at a base address as defined below.  Status and 
control ports are defined off that base.

		          write									read
=============================================================================
Base	data to the printer is latched.			Read latched data

base+1	not defined                             Read printer status lines
												Bits are as follows:
												bit 7	busy		0x80
												bit 6 	ack			0x40
												bit 5	paper out	0x20
												bit 4	select in	0x10
												bit 3	error		0x08
												2,1,0	Not used

base+2	write control bits						read the same control bits
		Bits are defined as follows:			Normally reads the latched
		bit 0	*strobe			0x01			bits written to same port
		bit 1	*auto feed		0x02
		bit 2	init printer	0x04
		bit 3	*select out		0x08
		bit 4	turn on irq7 on ACK hi-2-lo toggle	0x10
		5,6,7	not used
*/
