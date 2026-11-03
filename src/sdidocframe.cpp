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

#include "global.h"

#include "sdidocframe.h"
#include <wx/docview.h>
#include <wx/file.h>
#include <wx/log.h>
#include <wx/intl.h>

namespace ChipW {

IMPLEMENT_CLASS(SDIDocFrame, wxFrame)

BEGIN_EVENT_TABLE(SDIDocFrame, wxFrame)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, SDIDocFrame::OnMRUFile)
    EVT_ACTIVATE(SDIDocFrame::OnActivate)
    EVT_CLOSE(SDIDocFrame::OnCloseWindow)
    EVT_MENU(wxID_EXIT, SDIDocFrame::OnExit)
END_EVENT_TABLE()

SDIDocFrame::SDIDocFrame()
 : mgr(NULL), view(NULL)
{
}

SDIDocFrame::SDIDocFrame(wxDocManager* manager, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name)
 : mgr(NULL), view(NULL)
{
    Create(manager, parent, id, title, pos, size, style, name);
}

SDIDocFrame::SDIDocFrame(wxView* newview, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name)
 : mgr(NULL), view(NULL)
{
    Create(newview, parent, id, title, pos, size, style, name);
}

bool SDIDocFrame::Create(wxDocManager* manager, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name)
{
    if(!wxFrame::Create(parent, id, title, pos, size, style, name))
        return false;
    mgr = manager;
    view = NULL;
    return true;
}

bool SDIDocFrame::Create(wxView* newview, wxFrame* parent, wxWindowID id, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style, const wxString& name)
{
    if(!wxFrame::Create(parent, id, title, pos, size, style, name))
        return false;
    view = newview;
    if(view != NULL) {
        mgr = view->GetDocumentManager();
        view->SetFrame(this);
    }
    return true;
}

bool SDIDocFrame::Destroy() {
    view = NULL;
    mgr = NULL;
    return wxFrame::Destroy();
}

wxDocument* SDIDocFrame::GetDocument() const {
    if(view == NULL)
        return NULL;
    return view->GetDocument();
}

bool SDIDocFrame::ProcessEvent(wxEvent& event) {
    if(view != NULL) {
        view->Activate(true);
        if(view->ProcessEvent(event))
            return true;
    }
    if(mgr != NULL && mgr->ProcessEvent(event))
        return true;
    return wxFrame::ProcessEvent(event);
}

void SDIDocFrame::OnMRUFile(wxCommandEvent& event) {
    if(mgr == NULL)
        return;
    int n = event.GetId() - wxID_FILE1;
    wxString filename(mgr->GetHistoryFile(n));
    if(filename.empty())
        return;
    if(wxFile::Exists(filename)) {
        if(!mgr->CreateDocument(filename, wxDOC_SILENT)) {
            mgr->RemoveFileFromHistory(n);
            wxLogError(_("The file '%s' couldn't be opened.\nIt has been removed from the most recently used files list."),
                filename.c_str());
        }
    } else {
        mgr->RemoveFileFromHistory(n);
        wxLogError(_("The file '%s' doesn't exist and couldn't be opened.\nIt has been removed from the most recently used files list."),
            filename.c_str());
    }
}

void SDIDocFrame::OnActivate(wxActivateEvent& event) {
    wxFrame::OnActivate(event);
    if(view != NULL)
        view->Activate(event.GetActive());
}

void SDIDocFrame::OnCloseWindow(wxCloseEvent& event) {
    if(view != NULL) {
        if(event.CanVeto() && !view->Close(false)) {
            event.Veto();
            return;
        }
        view->Activate(false);
        delete view;
        view = NULL;
    }
    mgr = NULL;
    Destroy();
}

void SDIDocFrame::OnExit(wxCommandEvent& event) {
    // To exit, we close all documents. This might be vetoed, if a document has unsaved changes and the user chooses Cancel.
    if(mgr != NULL && !mgr->CloseDocuments(false))
        return;
    // If this frame has no view, it will not be closed by the document manager, so we need to close it here.
    if(view == NULL)
        Close();
    // Clear pointers as the objects might become invalid.
    view = NULL;
    // Allow further exit handling elsewhere.
    event.Skip();
}

}

