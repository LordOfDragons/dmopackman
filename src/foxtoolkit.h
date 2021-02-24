/* 
 * DMO Pack Manager
 *
 * Copyright (C) 2011, Plüss Roland ( roland@rptd.ch )
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either 
 * version 2 of the License, or (at your option) any later 
 * version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _FOXTOOLKIT_H_
#define _FOXTOOLKIT_H_

#include "dmopmconfig.h"

#ifdef HAVE_FOX_1_2_FX_H
#	error FOX 1.2 is not supported

#elif defined(HAVE_FOX_1_3_FX_H)
#	error FOX 1.3 is not supported

#elif defined(HAVE_FOX_1_4_FX_H)
#	include <fox-1.4/fx.h>
#	include <fox-1.4/fx3d.h>
#	include <fox-1.4/fxkeys.h>
#	include <fox-1.4/FXPNGIcon.h>
#	include <fox-1.4/FXUTF16Codec.h>
#	include <fox-1.4/FXUTF8Codec.h>

#elif defined(HAVE_FOX_1_6_FX_H)
#	include <fox-1.6/fx.h>
#	include <fox-1.6/fx3d.h>
#	include <fox-1.6/fxkeys.h>
#	include <fox-1.6/FXPNGIcon.h>
#	include <fox-1.6/FXUTF16Codec.h>
#	include <fox-1.6/FXUTF8Codec.h>
#endif

// anti-windows macros
#undef GetClassName
#undef GetObject
#undef LoadImage

#endif // _FOXTOOLKIT_H_
