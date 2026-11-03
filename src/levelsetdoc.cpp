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

#include "levelsetdoc.h"
#include "levelchange.h"
#include "levelupdatehint.h"
#include "wxtostdstream.h"
#include <ostream>
#include <istream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <wx/app.h>
#include <wx/intl.h>
#include <wx/cmdproc.h>

namespace ChipW {

class LevelChangeCommand : public wxCommand {
public:
    LevelChangeCommand(CountedPtr<Level> lev) : level(lev) { }
    LevelChangeCommand(CountedPtr<Level> lev, LevelChange* change) : level(lev) {changes.push_back(change);}
    virtual ~LevelChangeCommand();
    virtual wxString GetName() const {return wxT("Edit Level Map");}
    virtual bool CanUndo() const {return true;}
    virtual bool Do();
    virtual bool Undo();
    void Finish(wxCommandProcessor* processor);
    CountedPtr<Level> level;
    std::vector<LevelChange*> changes;
};

LevelChangeCommand::~LevelChangeCommand() {
    for(std::vector<LevelChange*>::const_iterator it = changes.begin(); it != changes.end(); ++it)
        delete *it;
}

bool LevelChangeCommand::Do() {
    for(std::vector<LevelChange*>::const_iterator it = changes.begin(); it != changes.end(); ++it) {
        if(!(*it)->Do(level))
            return false;
    }
    return true;
}

bool LevelChangeCommand::Undo() {
    for(std::vector<LevelChange*>::const_reverse_iterator it = changes.rbegin(); it != changes.rend(); ++it) {
        if(!(*it)->Undo(level))
            return false;
    }
    return true;
}

void LevelChangeCommand::Finish(wxCommandProcessor* processor) {
    if(changes.empty()) {
        delete this;
    } else {
        if(!processor->Submit(this))
            processor->ClearCommands();
    }
}

class LevelCommandProcessor : public wxCommandProcessor {
public:
    LevelCommandProcessor() : levelchanges(NULL) { }
    virtual void ClearCommands() {FinishLevelChangeCommand(); wxCommandProcessor::ClearCommands();}
    virtual bool Undo() {FinishLevelChangeCommand(); return wxCommandProcessor::Undo();}
    virtual bool Redo() {FinishLevelChangeCommand(); return wxCommandProcessor::Redo();}
    virtual bool Submit(wxCommand* command, bool storeIt = true) {FinishLevelChangeCommand(); return wxCommandProcessor::Submit(command, storeIt);}
    virtual void MarkAsSaved() {FinishLevelChangeCommand(); wxCommandProcessor::MarkAsSaved();}
    virtual bool IsDirty() {return (levelchanges != NULL && !levelchanges->changes.empty()) || wxCommandProcessor::IsDirty();}
    void AddLevelChange(CountedPtr<Level> level, LevelChange* change);
    void FinishLevelChangeCommand();
    bool CancelLevelChangeCommand();
private:
    LevelChangeCommand* levelchanges;
};

void LevelCommandProcessor::AddLevelChange(CountedPtr<Level> level, LevelChange* change) {
    if(levelchanges != NULL && levelchanges->level != level)
        FinishLevelChangeCommand();
    if(levelchanges == NULL)
        levelchanges = new LevelChangeCommand(level);
    levelchanges->changes.push_back(change);
}

void LevelCommandProcessor::FinishLevelChangeCommand() {
    if(levelchanges != NULL) {
        LevelChangeCommand* oldlevelchanges = levelchanges;
        levelchanges = NULL;
        oldlevelchanges->Finish(this);
    }
}

bool LevelCommandProcessor::CancelLevelChangeCommand() {
    if(levelchanges != NULL) {
        if(!levelchanges->Undo())
            return false;
        levelchanges = NULL;
    }
    return true;
}

class LevelAddDelCommand : public wxCommand {
public:
    LevelAddDelCommand(CountedPtr<Level> lev, LevelSetDoc* doc, bool isdel, wxUint16 n = 65535)
        : levels(1, lev), document(doc), index(n), isdelete(isdel) { }
    LevelAddDelCommand(const std::vector<CountedPtr<Level> >& levs, LevelSetDoc* doc, bool isdel, wxUint16 n = 65535)
        : levels(levs), document(doc), index(n), isdelete(isdel) { }
    template<class InputIterator>
    LevelAddDelCommand(InputIterator begin, InputIterator end, LevelSetDoc* doc, bool isdel, wxUint16 n = 65535)
        : levels(begin, end), document(doc), index(n), isdelete(isdel) { }
    virtual wxString GetName() const {return isdelete ? wxT("Delete Level") : wxT("Add Level");}
    virtual bool CanUndo() const {return true;}
    virtual bool Do() {return isdelete ? Del() : Add();}
    virtual bool Undo() {return isdelete ? Add() : Del();}
    bool Add();
    bool Del();
    std::vector<CountedPtr<Level> > levels;
    LevelSetDoc* document;
    wxUint16 index;
    bool isdelete;
};

bool LevelAddDelCommand::Add() {
    if(levels.empty())
        return true;
    if(document == NULL)
        return false;
    LevelSet* levelset = document->GetLevelSet();
    if(levelset == NULL)
        return false;
    if(index > levelset->GetLevelCount())
        index = levelset->GetLevelCount();
    wxUint16 n = index;
    std::vector<CountedPtr<Level> >::const_iterator it;
    for(it = levels.begin(); it != levels.end(); ++it) {
        if(!levelset->InsertLevel(*it, n))
            break;
        ++n;
    }
    if(it != levels.end()) {
        while(n > index) {
            --n;
            levelset->RemoveLevel(n);
        }
        return false;
    }
    document->Modify(true);
    LevelUpdateHint hint;
    hint.type = LevelUpdateHint::CREATE;
    if(levels.size() == 1)
        hint.level = levels.front();
    document->UpdateAllViews(NULL, &hint);
    return true;
}

bool LevelAddDelCommand::Del() {
    if(levels.empty())
        return true;
    if(document == NULL)
        return false;
    LevelSet* levelset = document->GetLevelSet();
    if(levelset == NULL)
        return false;
    wxUint16 n = index + levels.size();
    while(n > index) {
        --n;
        levelset->RemoveLevel(n);
    }
    document->Modify(true);
    LevelUpdateHint hint;
    hint.type = LevelUpdateHint::DESTROY;
    if(levels.size() == 1)
        hint.level = levels.front();
    document->UpdateAllViews(NULL, &hint);
    return true;
}

class LevelMoveCommand : public wxCommand {
public:
    LevelMoveCommand(CountedPtr<Level> lev, LevelSetDoc* doc, wxUint16 from, wxUint16 to) : level(lev), document(doc), movefrom(from), moveto(to) { }
    virtual wxString GetName() const {return wxT("Reorder Levels");}
    virtual bool CanUndo() const {return true;}
    virtual bool Do() {return Move(moveto);}
    virtual bool Undo() {return Move(movefrom);}
    bool Move(wxUint16 newindex);
    CountedPtr<Level> level;
    LevelSetDoc* document;
    wxUint16 movefrom;
    wxUint16 moveto;
};

bool LevelMoveCommand::Move(wxUint16 newindex) {
    if(document == NULL || level == NULL)
        return false;
    LevelSet* levelset = document->GetLevelSet();
    if(levelset == NULL)
        return false;
    if(!levelset->InsertLevel(level, newindex))
        return false;
    document->Modify(true);
    LevelUpdateHint hint;
    hint.type = LevelUpdateHint::LIST;
    hint.level = level;
    document->UpdateAllViews(NULL, &hint);
    return true;
}

class LevelMonstersCommand : public wxCommand {
public:
    LevelMonstersCommand(CountedPtr<Level> lev, LevelSetDoc* doc, const std::list<MonsterRecord>& oldmons, const std::list<MonsterRecord>& newmons) : level(lev), document(doc), oldmonsters(oldmons), newmonsters(newmons) { }
    virtual wxString GetName() const {return wxT("Edit Monster List");}
    virtual bool CanUndo() const {return true;}
    virtual bool Do() {return SetMonsters(newmonsters);}
    virtual bool Undo() {return SetMonsters(oldmonsters);}
    bool SetMonsters(const std::list<MonsterRecord>& monsters);
    CountedPtr<Level> level;
    LevelSetDoc* document;
    std::list<MonsterRecord> oldmonsters;
    std::list<MonsterRecord> newmonsters;
};

bool LevelMonstersCommand::SetMonsters(const std::list<MonsterRecord>& monsters) {
    if(document == NULL || level == NULL)
        return false;
    level->monsters = monsters;
    document->Modify(true);
    LevelUpdateHint hint;
    hint.type = LevelUpdateHint::MONSTER;
    hint.level = level;
    document->UpdateAllViews(NULL, &hint);
    return true;
}

class LevelPropertiesCommand : public wxCommand {
public:
    struct Properties {
        std::string title;
        std::string psw;
        wxUint16 chips;
        wxUint16 time;
        std::string hint;
        wxUint16 levelnumber;
        wxUint32 ruleset;
    };
    LevelPropertiesCommand(CountedPtr<Level> lev, LevelSetDoc* doc, const Properties& oldprops, const Properties& newprops) : level(lev), document(doc), oldproperties(oldprops), newproperties(newprops) { }
    virtual wxString GetName() const {return wxT("Edit Level Properties");}
    virtual bool CanUndo() const {return true;}
    virtual bool Do() {return SetProperties(newproperties);}
    virtual bool Undo() {return SetProperties(oldproperties);}
    bool SetProperties(const Properties& properties);
    CountedPtr<Level> level;
    LevelSetDoc* document;
    Properties oldproperties;
    Properties newproperties;
};

inline bool operator==(const LevelPropertiesCommand::Properties& a, const LevelPropertiesCommand::Properties& b) {
    return a.title == b.title
        && a.psw == b.psw
        && a.chips == b.chips
        && a.time == b.time
        && a.hint == b.hint
        && a.levelnumber == b.levelnumber
        && a.ruleset == b.ruleset
    ;
}

inline bool operator!=(const LevelPropertiesCommand::Properties& a, const LevelPropertiesCommand::Properties& b) {return !(a == b);}

bool LevelPropertiesCommand::SetProperties(const Properties& properties) {
    if(document == NULL || level == NULL)
        return false;
    LevelSet* levelset = document->GetLevelSet();
    if(levelset == NULL)
        return false;
    level->title = properties.title;
    level->psw = properties.psw;
    level->chips = properties.chips;
    level->time = properties.time;
    level->hint = properties.hint;
    level->levelnumber = properties.levelnumber;
    levelset->ruleset = properties.ruleset;
    document->Modify(true);
    LevelUpdateHint hint;
    hint.type = LevelUpdateHint::LIST;
    hint.level = level;
    document->UpdateAllViews(NULL, &hint);
    return true;
}

IMPLEMENT_DYNAMIC_CLASS(LevelSetDoc, wxDocument)

LevelSetDoc::LevelSetDoc(wxDocument* parent)
 : wxDocument(parent)
{
    levelset.SetMonitor(this);
}

LevelSetDoc::~LevelSetDoc() {
    levelset.RemoveMonitor(this);
}

wxCommandProcessor* LevelSetDoc::OnCreateCommandProcessor() {
    return new LevelCommandProcessor;
}

void LevelSetDoc::FinishLevelChangeCommand() {
    LevelCommandProcessor* cmdproc = (LevelCommandProcessor*) GetCommandProcessor();
    if(cmdproc != NULL)
        cmdproc->FinishLevelChangeCommand();
}

bool LevelSetDoc::CancelLevelChangeCommand() {
    LevelCommandProcessor* cmdproc = (LevelCommandProcessor*) GetCommandProcessor();
    if(cmdproc != NULL)
        return cmdproc->CancelLevelChangeCommand();
    return false;
}

void LevelSetDoc::OnLevelChange(CountedPtr<Level> level, LevelChange* change, bool isnew, bool reverse) {
    if(level == NULL || change == NULL)
        return;
    LevelUpdateHint hint;
    hint.level = level;
    switch(change->type) {
    case LevelChange::UPPER:
    case LevelChange::LOWER:
    {
        hint.type = LevelUpdateHint::TILE;
        LevelChangeTile* tilechange = (LevelChangeTile*) change;
        hint.x = tilechange->x;
        hint.y = tilechange->y;
        break;
    }
    case LevelChange::ADDMONSTER:
    case LevelChange::DELMONSTER:
    {
        hint.type = LevelUpdateHint::MONSTER;
        LevelChangeMonster* monsterchange = (LevelChangeMonster*) change;
        hint.x = monsterchange->record.x;
        hint.y = monsterchange->record.y;
        break;
    }
    case LevelChange::ADDTRAPWIRE:
    case LevelChange::DELTRAPWIRE:
    case LevelChange::ADDCLONEWIRE:
    case LevelChange::DELCLONEWIRE:
    {
        hint.type = LevelUpdateHint::WIRE;
        break;
    }
    }
    Modify(true);
    if(isnew) {
        LevelCommandProcessor* cmdproc = (LevelCommandProcessor*) GetCommandProcessor();
        if(cmdproc != NULL)
            cmdproc->AddLevelChange(level, change);
    }
    if(hint.type != LevelUpdateHint::NONE)
        UpdateAllViews(NULL, &hint);
}

bool LevelSetDoc::InsertLevel(CountedPtr<Level> level, wxUint16 index) {
    if(level == NULL)
        return false;
    return GetCommandProcessor()->Submit(new LevelAddDelCommand(level, this, false, index));
}

bool LevelSetDoc::InsertLevels(const std::vector<CountedPtr<Level> >& levels, wxUint16 index) {
    if(levels.empty())
        return false;
    return GetCommandProcessor()->Submit(new LevelAddDelCommand(levels, this, false, index));
}

bool LevelSetDoc::RemoveLevel(CountedPtr<Level> level) {
    if(level == NULL)
        return false;
    return GetCommandProcessor()->Submit(new LevelAddDelCommand(level, this, true, levelset.FindLevel(level)));
}

bool LevelSetDoc::MoveLevel(CountedPtr<Level> level, wxUint16 newindex) {
    if(level == NULL)
        return false;
    return GetCommandProcessor()->Submit(new LevelMoveCommand(level, this, levelset.FindLevel(level), newindex));
}

bool LevelSetDoc::SetLevelMonsters(CountedPtr<Level> level, const std::list<MonsterRecord>& newmonsters) {
    if(level == NULL)
        return false;
    if(newmonsters == level->monsters)
        return true;
    return GetCommandProcessor()->Submit(new LevelMonstersCommand(level, this, level->monsters, newmonsters));
}

bool LevelSetDoc::SetLevelProperties(CountedPtr<Level> level, std::string title, std::string psw, wxUint16 chips, wxUint16 time, std::string hint, wxUint16 levelnumber, wxUint32 ruleset) {
    if(level == NULL)
        return false;
    LevelPropertiesCommand::Properties oldproperties;
    oldproperties.title = level->title;
    oldproperties.psw = level->psw;
    oldproperties.chips = level->chips;
    oldproperties.time = level->time;
    oldproperties.hint = level->hint;
    oldproperties.levelnumber = level->levelnumber;
    oldproperties.ruleset = levelset.ruleset;
    LevelPropertiesCommand::Properties newproperties;
    newproperties.title = title;
    newproperties.psw = psw;
    newproperties.chips = chips;
    newproperties.time = time;
    newproperties.hint = hint;
    newproperties.levelnumber = levelnumber;
    newproperties.ruleset = ruleset;
    if(newproperties == oldproperties)
        return true;
    return GetCommandProcessor()->Submit(new LevelPropertiesCommand(level, this, oldproperties, newproperties));
}

bool LevelSetDoc::DoSaveDocument(const wxString& file) {
    wxTempFileOutputStream wxstream(file);
    if(!wxstream.IsOk()) {
        wxLogError(wxT("%s"), _("Sorry, could not open this file for saving."));
        return false;
    }
    {
        owxstream stream(&wxstream);
        if(!SaveObject(stream)) {
            wxLogError(wxT("%s"), _("Sorry, could not save this file."));
            return false;
        }
    }
    if(!wxstream.Commit()) {
        wxLogError(wxT("%s"), _("Sorry, could not save this file."));
        return false;
    }
    return true;
}

bool LevelSetDoc::DoOpenDocument(const wxString& file)
{
#ifdef CHIPW_DEBUG_IWXSTREAM
    wxFileInputStream wxstream(file);
    iwxstream stream(&wxstream);
    if(wxstream.GetLastError() == wxSTREAM_NO_ERROR)
#else
    std::ifstream stream(file.mb_str(), std::ios::binary);
    if(stream)
#endif
    {
        if(LoadObject(stream))
            return true;
    }
    wxLogError(wxT("%s"), _("Sorry, could not open this file."));
    return false;
}

std::ostream& LevelSetDoc::SaveObject(std::ostream& stream) {
    if(!levelset.Save_MSCC(stream))
        stream.setstate(std::ios_base::failbit);
    return stream;
}

std::istream& LevelSetDoc::LoadObject(std::istream& stream) {
    if(!levelset.Load_MSCC(stream)) {
#if 0
        wxLogError(wxT("The levelset may not have been loaded successfully.\n"
            "If you attempt to save this file from within Chip's Workshop, data corruption may result.\n"
            "If you believe that this is a valid MSCC levelset file, please send a bug report,\n"
            "including a copy of the file that caused this message and all error messages seen."));
#endif
        stream.setstate(std::ios_base::failbit);
    }
    return stream;
}

#if !wxUSE_STD_IOSTREAM
wxOutputStream& LevelSetDoc::SaveObject(wxOutputStream& wxstream) {
    owxstream stream(&wxstream);
    SaveObject(stream);
    return wxstream;
}

wxInputStream& LevelSetDoc::LoadObject(wxInputStream& wxstream) {
    iwxstream stream(&wxstream);
    LoadObject(stream);
    return wxstream;
}
#endif

}

