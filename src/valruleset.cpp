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

#include "valruleset.h"
#include "ruleset.h"
#include <wx/arrstr.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/msgdlg.h>

namespace ChipW {

bool RuleSetValidator::Copy(const RuleSetValidator& newval) {
    if(!wxValidator::Copy(newval))
        return false;
    nonetext = newval.nonetext;
    allownone = newval.allownone;
    ptrhasruleset = newval.ptrhasruleset;
    ptrruleset = newval.ptrruleset;
    return true;
}

bool RuleSetValidator::TransferFromWindow() {
    wxString text;
    if(!GetWindowText(text))
        return false;
    RuleSetOrNone val;
    if(text != nonetext)
        val = GetRuleSetFromString(text);
    if(ptrhasruleset != NULL)
        *ptrhasruleset = val.hasruleset;
    if(ptrruleset != NULL)
        *ptrruleset = val.ruleset;
    return true;
}

bool RuleSetValidator::TransferToWindow() {
    if(ptrhasruleset != NULL && !*ptrhasruleset) {
        if(!SetWindowText(nonetext))
            return false;
    } else if(ptrruleset != NULL) {
        wxString text = GetRuleSetString(*ptrruleset);
        if(text == nonetext)
            return false;
        if(!SetWindowText(text))
            return false;
    }
    return true;
}

bool RuleSetValidator::Validate(wxWindow* parent) {
    wxWindow* win = GetWindow();
    if(win == NULL)
        return false;
    wxString text;
    if(!GetWindowText(text))
        return false;
    bool valid = ((text == nonetext) ? allownone : (bool) GetRuleSetFromString(text));
    if(!valid) {
        win->SetFocus();
        wxMessageBox(wxString::Format(wxT("'%s' is not a valid rule set."), text.c_str()),
            wxT("Validation conflict"), wxOK | wxICON_EXCLAMATION, parent);
    }
    return valid;
}

bool RuleSetValidator::GetWindowText(wxString& text) const {
    wxWindow* win = GetWindow();
    if(win != NULL) {
        if(win->IsKindOf(CLASSINFO(wxTextCtrl))) {
            text = ((wxTextCtrl*) win)->GetValue();
            return true;
        } else if(win->IsKindOf(CLASSINFO(wxComboBox))) {
            text = ((wxComboBox*) win)->GetValue();
            return true;
        }
    }
    return false;
}

bool RuleSetValidator::SetWindowText(const wxString& text) const {
    wxWindow* win = GetWindow();
    if(win != NULL) {
        if(win->IsKindOf(CLASSINFO(wxTextCtrl))) {
            ((wxTextCtrl*) win)->SetValue(text);
            return true;
        } else if(win->IsKindOf(CLASSINFO(wxComboBox))) {
            ((wxComboBox*) win)->SetValue(text);
            return true;
        }
    }
    return false;
}

wxArrayString RuleSetValidator::GetChoices() const {
    wxArrayString choices;
    choices.Alloc(SUPPORTED_RULESET_COUNT + 1);
    if(allownone && !nonetext.empty())
        choices.Add(nonetext);
    for(unsigned int i = 0; i < SUPPORTED_RULESET_COUNT; ++i) {
        wxString str = GetRuleSetString(SUPPORTED_RULESETS[i]);
        if(!str.empty() && str != nonetext)
            choices.Add(str);
    }
    return choices;
}

}
