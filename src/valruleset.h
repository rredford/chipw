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

#ifndef CHIPW_VALRULESET_H_INCLUDED
#define CHIPW_VALRULESET_H_INCLUDED

#include "ruleset.h"
#include <wx/defs.h>
#include <wx/string.h>
#include <wx/validate.h>
#include <wx/arrstr.h>

namespace ChipW {

class RuleSetValidator : public wxValidator {
public:
    RuleSetValidator(RuleSetId* newptrruleset) : allownone(false), ptrhasruleset(NULL), ptrruleset(newptrruleset) { }
    RuleSetValidator(const wxString& newnonetext = wxEmptyString, RuleSetOrNone* newptrval = NULL)
        : allownone(true), nonetext(newnonetext), ptrhasruleset(newptrval == NULL ? NULL : &newptrval->hasruleset),
          ptrruleset(newptrval == NULL ? NULL : &newptrval->ruleset) { }
    RuleSetValidator(const RuleSetValidator& newval) : allownone(false), ptrhasruleset(NULL), ptrruleset(NULL) {Copy(newval);}
    RuleSetValidator& operator=(const RuleSetValidator& newval) {Copy(newval); return *this;}
    bool Copy(const RuleSetValidator& newval);
    virtual wxObject* Clone() const {return new RuleSetValidator(*this);}
    virtual bool TransferFromWindow();
    virtual bool TransferToWindow();
    virtual bool Validate(wxWindow* parent);
    wxArrayString GetChoices() const;
private:
    bool GetWindowText(wxString& text) const;
    bool SetWindowText(const wxString& text) const;
    bool allownone;
    wxString nonetext;
    bool* ptrhasruleset;
    RuleSetId* ptrruleset;
};

}

#endif // !CHIPW_VALRULESET_H_INCLUDED
