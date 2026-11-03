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

#ifndef CHIPW_SDIDOCFRAME_H_INCLUDED
#define CHIPW_SDIDOCFRAME_H_INCLUDED

#include <wx/defs.h>
#include <wx/gdicmn.h>
#include <wx/frame.h>

class wxDocManager;
class wxDocument;
class wxView;
class wxFrame;
class wxString;
class wxPoint;
class wxSize;

namespace ChipW {

// Proper SDI document frame for the wxWidgets document-view framework.
// This is based off a combination of the parent and child ones from wxWidgets.
class SDIDocFrame : public wxFrame {
public:
    SDIDocFrame();
    SDIDocFrame(wxDocManager* manager, wxFrame* parent, wxWindowID id = -1, const wxString& title = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxFrameNameStr);
    SDIDocFrame(wxView* newview, wxFrame* parent, wxWindowID id = -1, const wxString& title = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxFrameNameStr);
    bool Create(wxDocManager* manager, wxFrame* parent, wxWindowID id = -1, const wxString& title = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxFrameNameStr);
    bool Create(wxView* newview, wxFrame* parent, wxWindowID id = -1, const wxString& title = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxFrameNameStr);
    virtual bool Destroy();
    wxDocManager* GetDocumentManager() const {return mgr;}
    wxDocument* GetDocument() const;
    wxView* GetView() const {return view;}
    void SetView(wxView* newview) {view = newview;}
    virtual bool ProcessEvent(wxEvent& event);
    void OnMRUFile(wxCommandEvent& event);
    void OnActivate(wxActivateEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void OnExit(wxCommandEvent& event);
private:
    wxDocManager* mgr;
    wxView* view;
    DECLARE_CLASS(SDIDocFrame)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(SDIDocFrame)
};

}

#endif // !CHIPW_SDIDOCFRAME_H_INCLUDED
