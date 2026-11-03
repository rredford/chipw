// Chip's Workshop - a level editor for Chip's Challenge.
// Copyright 2008-2011,2021 Christopher Elsby <chrise@chrise.me.uk>
// Modified 2026 by the Chip's Workshop fork contributors (see README.md for a list of changes).
// This is an unofficial modified version; it is not endorsed by the original author.
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

#include "global.h"

#include "bufferscroll.h"
#include <wx/dcbuffer.h>

namespace ChipW {

BEGIN_EVENT_TABLE(BufferedScrolledWindow, wxScrolledWindow)
    EVT_PAINT(BufferedScrolledWindow::OnPaint)
END_EVENT_TABLE()

BufferedScrolledWindow::BufferedScrolledWindow() {
}

BufferedScrolledWindow::BufferedScrolledWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
long style, const wxString& name) {
    Create(parent, id, pos, size, style, name);
}

bool BufferedScrolledWindow::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
long style, const wxString& name) {
    if(!wxScrolledWindow::Create(parent, id, pos, size, style, name))
        return false;
    // We paint the whole window ourselves in OnPaint, so tell the toolkit not
    // to erase the background first (avoids flicker).
#if wxCHECK_VERSION(2, 9, 1)
    SetBackgroundStyle(wxBG_STYLE_PAINT);
#else
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
#endif
    return true;
}

void BufferedScrolledWindow::OnPaint(wxPaintEvent& event) {
    wxBufferedPaintDC dc(this);
    dc.SetBackground(wxBrush(GetBackgroundColour()));
    dc.Clear();
    DoPrepareDC(dc);
    OnDraw(dc);
}

}
