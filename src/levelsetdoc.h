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

#ifndef CHIPW_LEVELSETDOC_H_INCLUDED
#define CHIPW_LEVELSETDOC_H_INCLUDED

#include <wx/docview.h>
#include <ostream>
#include <istream>
#include <vector>
#include "level.h"
#include "levelset.h"
#include "levelchange.h"

namespace ChipW {

class LevelSetDoc : public wxDocument, public LevelMonitor {
public:
    LevelSetDoc(wxDocument* parent = NULL);
    virtual ~LevelSetDoc();
    const LevelSet* GetLevelSet() const {return &levelset;}
    LevelSet* GetLevelSet() {return &levelset;}
    virtual wxCommandProcessor* OnCreateCommandProcessor();
    virtual void OnLevelChange(CountedPtr<Level> level, LevelChange* change, bool isnew = true, bool reverse = false);
    void FinishLevelChangeCommand();
    bool CancelLevelChangeCommand();
    bool InsertLevel(CountedPtr<Level> level, wxUint16 index = 65535);
    bool InsertLevels(const std::vector<CountedPtr<Level> >& levels, wxUint16 index = 65535);
    template<class InputIterator> bool InsertLevels(InputIterator begin, InputIterator end, wxUint16 index = 65535)
        {return InsertLevels(std::vector<CountedPtr<Level> >(begin, end), index);}
    bool RemoveLevel(CountedPtr<Level> level);
    bool MoveLevel(CountedPtr<Level> level, wxUint16 newindex);
    bool SetLevelMonsters(CountedPtr<Level> level, const std::list<MonsterRecord>& newmonsters);
    bool SetLevelProperties(CountedPtr<Level> level, std::string title, std::string psw, wxUint16 chips, wxUint16 time, std::string hint, wxUint16 levelnumber, wxUint32 ruleset);
    virtual std::ostream& SaveObject(std::ostream& stream);
    virtual std::istream& LoadObject(std::istream& stream);
#if !wxUSE_STD_IOSTREAM
    virtual wxOutputStream& SaveObject(wxOutputStream& wxstream);
    virtual wxInputStream& LoadObject(wxInputStream& wxstream);
#endif
protected:
    virtual bool DoSaveDocument(const wxString& file);
    virtual bool DoOpenDocument(const wxString& file);
private:
    LevelSet levelset;
    DECLARE_DYNAMIC_CLASS(LevelSetDoc)
};

}

#endif // !CHIPW_LEVELSETDOC_H_INCLUDED
