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

#ifndef CHIPW_BUFFERSCROLL_H_INCLUDED
#define CHIPW_BUFFERSCROLL_H_INCLUDED

#include <wx/scrolwin.h>
#include <wx/event.h>

namespace ChipW {

class BufferedScrolledWindow : public wxScrolledWindow {
public:
    BufferedScrolledWindow();
    BufferedScrolledWindow(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxHSCROLL | wxVSCROLL, const wxString& name = wxT("scrolledWindow"));
    bool Create(wxWindow* parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxHSCROLL | wxVSCROLL, const wxString& name = wxT("scrolledWindow"));
    // Invalidate the window and repaint it immediately.
    // With wxWidgets 3.2 on GTK3, a plain Refresh() on this kind of window may not
    // result in a repaint until something else (e.g. a resize) forces one, so we
    // follow it with Update() to process the pending repaint synchronously.
    // Must not be called from inside a paint handler.
    void RepaintNow() {Refresh(false); Update();}
private:
    void OnPaint(wxPaintEvent& event);
    DECLARE_EVENT_TABLE()
};

}

#endif // !CHIPW_BUFFERSCROLL_H_INCLUDED
