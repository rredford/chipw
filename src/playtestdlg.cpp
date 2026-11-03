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

#include "playtestdlg.h"
#include "valruleset.h"
#include "help.h"
#include "level.h"
#include "levelset.h"
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/valtext.h>
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>

namespace ChipW {

BEGIN_EVENT_TABLE(PlayTestDialog, wxDialog)
    EVT_BUTTON(-1, PlayTestDialog::OnCommand)
END_EVENT_TABLE()

enum {
    ID_BROWSE_PROGRAM = wxID_HIGHEST + 1,
    ID_BROWSE_WORKINGDIR,
    ID_DEFAULT_ARGS,
    ID_TEST_SETTINGS,
};

PlayTestDialog::PlayTestDialog(wxWindow* parent)
 : wxDialog(parent, -1, wxT("Playtesting configuration"), wxDefaultPosition, wxDefaultSize)
{
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* settingsizer = new wxFlexGridSizer(1);
//    settingsizer->Add(new wxStaticText(this, -1, wxT("Name:")), 0, wxALL | wxALIGN_RIGHT, 5);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Enter a name for this playtesting profile:")), 0, wxTOP | wxLEFT | wxRIGHT, 5);
    settingsizer->Add(new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0,
        wxTextValidator(wxFILTER_NONE, &type.name)), 0, wxALL | wxGROW, 5);
//    settingsizer->Add(new wxStaticText(this, -1, wxT("Program:")), 0, wxALL | wxALIGN_RIGHT, 5);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Select the program to run (only Tile World is supported):")), 0, wxTOP | wxLEFT | wxRIGHT, 5);
    wxBoxSizer* programsizer = new wxBoxSizer(wxHORIZONTAL);
    programsizer->Add(progbox = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0,
        wxTextValidator(wxFILTER_NONE, &type.program)), 1);
    programsizer->Add(new wxButton(this, ID_BROWSE_PROGRAM, wxT("Browse...")));
    settingsizer->Add(programsizer, 0, wxALL | wxGROW, 5);
//    settingsizer->Add(new wxStaticText(this, -1, wxT("Rule set:")), 0, wxALL | wxALIGN_RIGHT, 5);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Optionally, select the rule set to use:")), 0, wxTOP | wxLEFT | wxRIGHT, 5);
    RuleSetValidator valruleset(wxT("Level set default"), &type.ruleset);
    settingsizer->Add(new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, valruleset.GetChoices(), wxCB_DROPDOWN,
        valruleset), 0, wxALL | wxGROW, 5);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Advanced settings - these can usually be left alone:")), 0, wxTOP | wxLEFT | wxRIGHT, 5);
//    settingsizer->Add(new wxStaticText(this, -1, wxT("Arguments:")), 0, wxALL | wxALIGN_RIGHT, 5);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Command line arguments:")), 0, wxTOP | wxLEFT | wxRIGHT, 5);
    wxBoxSizer* argsizer = new wxBoxSizer(wxHORIZONTAL);
    argsizer->Add(argbox = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0,
        wxTextValidator(wxFILTER_NONE, &type.arguments)), 1);
    argsizer->Add(new wxButton(this, ID_DEFAULT_ARGS, wxT("Default")));
    settingsizer->Add(argsizer, 0, wxALL | wxGROW, 5);
//    settingsizer->Add(new wxStaticText(this, -1, wxT("Directory:")), 0, wxALL | wxALIGN_RIGHT, 5);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Working directory:")), 0, wxTOP | wxLEFT | wxRIGHT, 5);
    wxBoxSizer *workingdirsizer = new wxBoxSizer(wxHORIZONTAL);
    workingdirsizer->Add(dirbox = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, 0,
        wxTextValidator(wxFILTER_NONE, &type.workingdir)), 1);
    workingdirsizer->Add(new wxButton(this, ID_BROWSE_WORKINGDIR, wxT("Browse...")));
    settingsizer->Add(workingdirsizer, 0, wxALL | wxGROW, 5);
//    settingsizer->AddGrowableCol(1);
    settingsizer->AddGrowableCol(0);
    sizer->Add(settingsizer, 1, wxGROW);
    sizer->Add(new wxButton(this, ID_TEST_SETTINGS, wxT("Test settings...")), 0, wxALL, 5);
#if 0
    wxBoxSizer* buttonsizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okbutton = new wxButton(this, wxID_OK, wxT("&OK"));
    okbutton->SetDefault();
    buttonsizer->Add(okbutton, 0, wxALL, 5);
    buttonsizer->Add(new wxButton(this, wxID_CANCEL, wxT("&Cancel")), 0, wxALL, 5);
#endif
    wxSizer* buttonsizer = CreateButtonSizer(wxOK | wxCANCEL /* | wxHELP */);
    if(buttonsizer != NULL)
        sizer->Add(buttonsizer, 0, wxGROW);
    SetSizerAndFit(sizer);
}

#if !wxCHECK_VERSION(2, 8, 0)
#define ChangeValue SetValue
#endif

#ifndef wxDD_DIR_MUST_EXIST
#define wxDD_DIR_MUST_EXIST 0
#endif

void PlayTestDialog::OnCommand(wxCommandEvent& event) {
    switch(event.GetId()) {
    case ID_BROWSE_PROGRAM:
        if(progbox != NULL) {
            wxString dirname;
            wxString filename = progbox->GetValue();
            if(filename.empty()) {
                if(dirbox != NULL)
                    dirname = dirbox->GetValue();
            } else {
                wxFileName filenamedetails(filename);
                dirname = filenamedetails.GetPath();
                filename = filenamedetails.GetFullName();
            }
            filename = wxFileSelector(wxT("Choose the program to run"), dirname, filename, wxT(""),
#ifdef __WXMSW__
                wxT("Executable files (*.exe)|*.exe|All files (*.*)|*.*")
#else
                wxT("*")
#endif
                , wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);
            if(!filename.empty()) {
                progbox->ChangeValue(filename);
                if(dirbox != NULL) {
                    wxFileName filenamedetails(filename);
                    dirbox->ChangeValue(filenamedetails.GetPath());
                }
            }
        }
        break;
    case ID_BROWSE_WORKINGDIR:
        if(dirbox != NULL) {
            wxString dir = wxDirSelector(wxDirSelectorPromptStr, dirbox->GetValue(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST, wxDefaultPosition, this);
            if(!dir.empty())
                dirbox->ChangeValue(dir);
        }
        break;
    case ID_DEFAULT_ARGS:
        if(argbox != NULL)
            argbox->ChangeValue(PlayTestType::GetDefaultArguments());
        break;
    case ID_TEST_SETTINGS:
        {
            // Validate and transfer the data from the controls.
            if(!Validate() || !TransferDataFromWindow())
                break;
            // Create a test level set with a very simple level.
            LevelSet testlevelset;
            CountedPtr<Level> testlevel = testlevelset.CreateLevel();
            if(testlevel != NULL) {
                testlevel->title = "TEST";
                testlevel->hint = "This is a test of playtesting.";
                testlevel->psw = "TEST";
                testlevel->PlaceTileOnly(1, 1, TILE_CHIP_S);
                testlevel->PlaceTileOnly(3, 1, TILE_HINT);
                testlevel->PlaceTileOnly(5, 1, TILE_EXIT);
            }
            // Try to playtest it.
            if(!type.Launch(testlevelset, testlevel->levelnumber))
                wxLogError(wxT("Playtesting failed."));
        }
        break;
    case wxID_HELP:
        LaunchHelp(wxT("playtest"));
        break;
    default:
        event.Skip();
    }
}





IMPLEMENT_DYNAMIC_CLASS(PlayTestListEditItem, ListEditItem)

wxString PlayTestListEditItem::GetSummaryText() const {
    return type.name.empty() ? wxT("(No name)") : type.name;
}

bool PlayTestListEditItem::Edit(wxWindow* parent) {
    PlayTestDialog dlg(parent);
    dlg.type = type;
    if(dlg.ShowModal() != wxID_OK)
        return false;
    type = dlg.type;
    return true;
}

ListEditItem* PlayTestListEditItem::CreateCopy() const {
    PlayTestListEditItem* newitem = new PlayTestListEditItem(*this);
    if(!newitem->type.name.empty())
        newitem->type.name.insert(0, wxT("Copy of "));
    return newitem;
}


PlayTestListEditDialog::PlayTestListEditDialog(wxWindow* parent, const std::vector<PlayTestType>& types, unsigned int maxlen)
 : ListEditDialog(parent, -1, wxT("Playtesting profiles"), CLASSINFO(PlayTestListEditItem), maxlen)
{
    for(std::vector<PlayTestType>::const_iterator it = types.begin(); it != types.end(); ++it)
        AddItem(new PlayTestListEditItem(*it));
}

std::vector<PlayTestType> PlayTestListEditDialog::GetTypes() const {
    size_t count = GetItemCount();
    std::vector<PlayTestType> types(count);
    for(size_t i = 0; i < count; ++i) {
        PlayTestListEditItem* item = wxDynamicCast(GetItem(i), PlayTestListEditItem);
        if(item != NULL)
            types[i] = item->type;
    }
    return types;
}


}

