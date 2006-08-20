/*
 * GNUitar
 * Utility functions
 * Copyright (C) 2000,2001,2003 Max Rudensky         <fonin@ziet.zhitomir.ua>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

#ifndef _UTILS_H_
#define _UTILS_H_ 1

#include <math.h>
#ifdef _WIN32
#   ifndef M_PI
#       define M_PI 3.1415926535897932384626433832795
#   endif
#   ifndef  R_OK
#       define  R_OK 04
#   endif
#   ifndef  X_OK
#       define  X_OK 01
#   endif
#   ifndef  F_OK
#       define  F_OK 00
#   endif
#   define FILESEP "\\"
#   ifndef isnan
#       define isnan _isnan
#   endif

#   include <windows.h>
typedef HANDLE  my_mutex;

#else
#   define FILESEP "/"

#   include <glib.h>
typedef GMutex* my_mutex;

#endif

extern void     my_create_mutex(my_mutex*);
extern void     my_lock_mutex(my_mutex);
extern void     my_unlock_mutex(my_mutex);
extern void     my_close_mutex(my_mutex);

#endif

