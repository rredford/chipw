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

#include "levelpropertiesdlg.h"
#include "level.h"
#include "valruleset.h"
#include <string>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valtext.h>
#include <wx/button.h>
#include <wx/combobox.h>

namespace ChipW {

BEGIN_EVENT_TABLE(LevelPropertiesDialog, wxDialog)
    EVT_BUTTON(-1, LevelPropertiesDialog::OnCommand)
END_EVENT_TABLE()

enum {
    ID_RANDPSW = wxID_HIGHEST + 1,
};

LevelPropertiesDialog::LevelPropertiesDialog(wxWindow* parent, bool readonly)
 : wxDialog(parent, -1, wxT("Level properties"), wxDefaultPosition, wxDefaultSize), pswbox(NULL)
{
    wxArrayString allowedChars;
    {
        allowedChars.Alloc(255);
        char str[2];
        str[1] = 0;
        for(str[0] = 1; str[0] != (char) 256; ++str[0])
            allowedChars.Add(wxString(str, wxConvISO8859_1));
#if 0 // Debug code to test safety of caller when faced with invalid characters.
        allowedChars.Add(wxString("\xE2\x82\xAC", wxConvUTF8)); // Euro sign, not in ISO-8859-1
#endif
    }
    long style = readonly ? wxTE_READONLY : 0;
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* settingsizer = new wxFlexGridSizer(2);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Title:")), 0, wxALL | wxALIGN_RIGHT, 5);
    {
        wxTextValidator val(wxFILTER_INCLUDE_CHAR_LIST, &title);
        val.SetIncludes(allowedChars);
        settingsizer->Add(new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, style,
            val), 0, wxALL | wxGROW, 5);
    }
    if(readonly) {
        settingsizer->Add(new wxStaticText(this, -1, wxT("Password:")), 0, wxALL | wxALIGN_RIGHT, 5);
    } else {
        settingsizer->Add(new wxButton(this, ID_RANDPSW, wxT("Password:"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
            0, wxALL | wxALIGN_RIGHT, 5);
    }
    {
        wxTextValidator val(wxFILTER_INCLUDE_CHAR_LIST, &psw);
        val.SetIncludes(allowedChars);
        settingsizer->Add(pswbox = new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, style,
            val), 0, wxALL | wxGROW, 5);
    }
    settingsizer->Add(new wxStaticText(this, -1, wxT("Chips required:")), 0, wxALL | wxALIGN_RIGHT, 5);
    settingsizer->Add(new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, style,
        wxTextValidator(wxFILTER_NUMERIC, &chips)), 0, wxALL | wxGROW, 5);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Chips in level:")), 0, wxALL | wxALIGN_RIGHT, 5);
    settingsizer->Add(new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY,
        wxTextValidator(wxFILTER_NUMERIC, &countedchips)), 0, wxALL | wxGROW, 5);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Time limit:")), 0, wxALL | wxALIGN_RIGHT, 5);
    settingsizer->Add(new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, style,
        wxTextValidator(wxFILTER_NUMERIC, &time)), 0, wxALL | wxGROW, 5);
    settingsizer->Add(new wxStaticText(this, -1, wxT("Hint:")), 0, wxALL | wxALIGN_RIGHT, 5);
    {
        wxTextValidator val(wxFILTER_INCLUDE_CHAR_LIST, &hint);
        val.SetIncludes(allowedChars);
        settingsizer->Add(new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, style,
            val), 0, wxALL | wxGROW, 5);
    }
    settingsizer->AddGrowableCol(1);
    sizer->Add(settingsizer, 0, wxGROW);
    sizer->Add(new wxStaticText(this, -1, wxT("Rule set (applies to the whole level set; choose MS to support both MS and Lynx):")),
        0, wxALL, 5);
    RuleSetValidator valruleset(&ruleset);
    if(readonly) {
        sizer->Add(new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, style,
            valruleset), 0, wxALL | wxGROW, 5);
    } else {
        sizer->Add(new wxComboBox(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, valruleset.GetChoices(), wxCB_DROPDOWN,
            valruleset), 0, wxALL | wxGROW, 5);
    }
    sizer->Add(new wxStaticText(this, -1, wxT("Internal level number - do not change unless you know what you are doing!")),
        0, wxALL, 5);
    sizer->Add(new wxTextCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, style,
        wxTextValidator(wxFILTER_NUMERIC, &levnum)), 0, wxALL | wxGROW, 5);
#if 0
    wxBoxSizer* buttonsizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okbutton = new wxButton(this, wxID_OK, wxT("&OK"));
    okbutton->SetDefault();
    buttonsizer->Add(okbutton, 0, wxALL, 5);
    buttonsizer->Add(new wxButton(this, wxID_CANCEL, wxT("&Cancel")), 0, wxALL, 5);
#endif
    wxSizer* buttonsizer = CreateButtonSizer(wxOK | wxCANCEL);
    if(buttonsizer != NULL)
        sizer->Add(buttonsizer, 0, wxGROW);
    SetSizerAndFit(sizer);
}

#if !wxCHECK_VERSION(2, 8, 0)
#define ChangeValue SetValue
#endif

void LevelPropertiesDialog::OnCommand(wxCommandEvent& event) {
    switch(event.GetId()) {
    case ID_RANDPSW:
        if(pswbox != NULL) {
            std::string randpsw = Level::RandPsw();
            pswbox->ChangeValue(wxString(randpsw.c_str(), wxConvISO8859_1));
        }
        break;
    default:
        event.Skip();
    }
}

}
