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

#include "app.h"
#include "levelsetdoc.h"
#include "leveleditview.h"
#include "leveleditframe.h"
#include "leveleditids.h"
#include "playtestmgr.h"
#include "playtestdlg.h"
#include "help.h"
#include <wx/app.h>
#include <wx/docview.h>
#include <wx/string.h>
#include <wx/image.h>
//#include <wx/choicdlg.h>
#include <wx/cmdline.h>
#include <wx/confbase.h>
#include <wx/filefn.h>
#include <wx/msgdlg.h>
#include <cstdlib>
#include <ctime>

IMPLEMENT_APP(ChipW::App)

namespace ChipW {

long RefCounted::nextid = 0;

BEGIN_EVENT_TABLE(App, wxApp)
    EVT_MENU(-1, App::OnCommand)
    EVT_BUTTON(-1, App::OnCommand)
END_EVENT_TABLE()

App::App()
 : docmgr(NULL), autostart(false)
{
}

bool App::OnInit() {
    SetAppName(wxT("ChipW"));
    SetVendorName(wxT("Glarbex"));
    std::srand(std::time(NULL));
    wxInitAllImageHandlers();
    playtestmgr = new PlayTestManager;
    docmgr = new wxDocManager;
    new wxDocTemplate(docmgr, wxT("Level set"), wxT("*.ccl;*.dat"), wxT(""), wxT("ccl"), wxT("Level set"), wxT("Level editor"),
        CLASSINFO(LevelSetDoc), CLASSINFO(LevelEditView));
    if(!wxApp::OnInit())
        return false;
    if(!autostart) {
#if 0
        wxString choices[2];
        choices[0] = wxT("Create a new levelset");
        choices[1] = wxT("Open an existing levelset");
        int choice = wxGetSingleChoiceIndex(wxT("Welcome to Chip's Workshop! What would you like to do?"), wxT("Chip's Workshop"), 2, choices);
        switch(choice) {
        case 0:
            docmgr->CreateDocument(wxEmptyString, wxDOC_NEW);
            break;
        case 1:
            docmgr->CreateDocument(wxEmptyString, 0);
            break;
        }
#endif
        LevelEditFrame* frame = new LevelEditFrame(docmgr);
        frame->Show();
    }
//    return !docmgr->GetDocuments().empty();
    return true;
}

void App::OnInitCmdLine(wxCmdLineParser& parser) {
    wxApp::OnInitCmdLine(parser);
    parser.SetLogo(wxT("Chip's Workshop " VERSION " Copyright 2008-2011 Christopher Elsby\nCompiled with ") wxVERSION_STRING);
    parser.AddSwitch(wxT("v"), wxT("version"), wxT("show the version number and copyright notice"));
    parser.AddParam(wxT("file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE);
}

bool App::OnCmdLineParsed(wxCmdLineParser& parser) {
    if(!wxApp::OnCmdLineParsed(parser))
        return false;
    if(parser.Found(wxT("v")))
    {
        //wxFprintf(stderr, wxT("Chip's Workshop %s Copyright 2008-2011 Christopher Elsby\n"
        //    "Compiled with %s\n"), wxT(VERSION), wxVERSION_STRING);
        return false;
    }
    autostart = (parser.GetParamCount() > 0);
    if(docmgr != NULL) {
        for(size_t n = 0; n < parser.GetParamCount(); ++n)
            docmgr->CreateDocument(parser.GetParam(n), wxDOC_SILENT);
    }
    return true;
}

int App::OnExit() {
    // Delete temporary files.
    for(std::set<wxString>::const_iterator it = tmpfiles.begin(); it != tmpfiles.end(); ++it) {
        if(!it->empty())
            wxRemoveFile(*it);
    }
    tmpfiles.clear();
    delete docmgr;
    docmgr = NULL;
    delete playtestmgr;
    playtestmgr = NULL;
    delete wxConfigBase::Set(NULL);
    return 0;
}

void App::RemoveTmpFile(wxString filename) {
    std::set<wxString>::iterator it = tmpfiles.find(filename);
    if(it != tmpfiles.end()) {
        if(filename.empty() || wxRemoveFile(filename))
            tmpfiles.erase(it);
    }
}

void App::OnCommand(wxCommandEvent& event) {
    switch(event.GetId()) {
    case wxID_ABOUT:
        wxMessageBox(
            wxT("Chip's Workshop " VERSION "\nCopyright 2008-2011,2021 Christopher Elsby\nCompiled on " __DATE__ " " __TIME__ ".\n"
            "Compiled with ") wxVERSION_STRING wxT(".\n"
            "The included tile images are by Anders Kaseorg and Michael Hansen.\n"
            "This program is free software: you can redistribute it and/or modify it "
            "under the terms of version 3 of the GNU General Public License "
            "as published by the Free Software Foundation.\n"
            "This program is distributed in the hope that it will be useful, "
            "but WITHOUT ANY WARRANTY; without even the implied warranty of "
            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
	        "GNU General Public License for more details.\n"
	        "You should have received a copy of the GNU General Public License "
            "along with this program.  If not, see <https://www.gnu.org/licenses/>."
        ), wxT("About Chip's Workshop"), wxOK);
        break;
    case wxID_HELP_CONTENTS:
    case wxID_HELP:
        LaunchHelp();
        break;
    case ID_PLAYTEST_CONFIG:
        if(playtestmgr != NULL) {
            PlayTestListEditDialog dlg(NULL, playtestmgr->GetTypes(), playtestmgr->GetMaxLength());
            if(dlg.ShowModal() == wxID_OK)
                playtestmgr->SetTypes(dlg.GetTypes());
            break;
        }
    default:
        event.Skip();
    }
}

}

