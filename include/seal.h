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

#ifndef _SEAL_H_
#define _SEAL_H_

#define SEAL_2
#define DEV_VERSION

#define SEAL_VER_MAJOR         2
#define SEAL_VER_MINOR         0
#define SEAL_VER_REVISION      12

#ifndef SEAL_VERSION_INFO_ONLY

#define text_loaded(x) ((x) && *(x))

#include "engine.h"
#include "object.h"
#include "view.h"
#include "keyboard.h"
#include "mouse.h"
#include "registry.h"
#include "fonts.h"
#include "window.h"
#include "txts.h"
#include "bmps.h"
#include "colors.h"
#include "vfile.h"
#include "files.h"
#include "skin.h"

#endif

#endif

