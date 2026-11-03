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

#ifndef CHIPW_APP_H_INCLUDED
#define CHIPW_APP_H_INCLUDED

#include <wx/app.h>
#include <wx/event.h>
#include <set>

class wxDocManager;

namespace ChipW {

class PlayTestManager;

class App : public wxApp {
public:
    App();
    virtual bool Initialize(int& argc, wxChar** argv);
    virtual bool OnInit();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
    virtual int OnExit();
    // Get the playtest menu manager.
    PlayTestManager* GetPlayTestManager() const {return playtestmgr;}
    // Register a temporary file to be deleted on program exit.
    void RegisterTmpFile(wxString filename) {tmpfiles.insert(filename);}
    // Unregister a temporary file so that it will not be deleted on program exit.
    void UnregisterTmpFile(wxString filename) {tmpfiles.erase(filename);}
    // Deletes and unregisters the given temporary file if it is currently registered.
    void RemoveTmpFile(wxString filename);
private:
    void OnCommand(wxCommandEvent& event);
    wxDocManager* docmgr;
    PlayTestManager* playtestmgr;
    bool autostart;
    std::set<wxString> tmpfiles;
    DECLARE_EVENT_TABLE()
};

}

DECLARE_APP(ChipW::App)

#endif // !CHIPW_APP_H_INCLUDED
