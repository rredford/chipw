// Chip's Workshop - a level editor for Chip's Challenge.
// Copyright 2008-2011,2021 Christopher Elsby <chrise@chrise.me.uk>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of version 3 of the GNU General Public License as
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Global includes.
// This file should be included once at the beginning of each source file, and never from headers.
// If precompiled header support is available, make this the precompiled header to speed
// up compiling.

// Include autoconf definitions.
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Include the wxWidgets precompiled header stuff.
// We do not then include wx.h if there is no PCH support, because that would greatly slow down
// compiling in this case, which happens to include my development environment.
// Instead, we include the wxWidgets headers we need when we need them.
#include <wx/wxprec.h>

// End the precompiled header.
#ifdef __BORLANDC__
#pragma hdrstop
#endif
