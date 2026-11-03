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

#ifndef CHIPW_LEVELEDITFRAME_H_INCLUDED
#define CHIPW_LEVELEDITFRAME_H_INCLUDED

#include "sdidocframe.h"
#include <map>
#include <vector>

class wxDocManager;

namespace ChipW {

class LevelEditFrame : public SDIDocFrame {
public:
    LevelEditFrame() : playtestmenu(NULL) { }
    LevelEditFrame(wxDocManager* manager, wxFrame* parent = NULL, bool isspare = true) : playtestmenu(NULL) {Create(manager, parent, isspare);}
    LevelEditFrame(wxView* newview, wxFrame* parent = NULL) : playtestmenu(NULL) {Create(newview, parent);}
    virtual ~LevelEditFrame();
    bool Create(wxDocManager* manager, wxFrame* parent = NULL, bool isspare = true);
    bool Create(wxView* newview, wxFrame* parent = NULL);
    void OnExit(wxCommandEvent& event);
    static LevelEditFrame* GetSpareOrNew(wxView* view);
private:
    bool CreateMenuBar();
    void OnCommand(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    static std::map<wxDocManager*, std::vector<LevelEditFrame*> > spare;
    wxMenu* playtestmenu;
    DECLARE_CLASS(LevelEditFrame)
    DECLARE_EVENT_TABLE()
};

}

#endif // !CHIPW_LEVELEDITFRAME_H_INCLUDED
