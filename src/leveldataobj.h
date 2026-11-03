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

#ifndef CHIPW_LEVELDATAOBJ_H_INCLUDED
#define CHIPW_LEVELDATAOBJ_H_INCLUDED

#include <wx/defs.h>
#include <wx/dataobj.h>
#include <string>
#include <list>
#include <vector>
#include "level.h"
#include "refcount.h"

namespace ChipW {

class Level;

class LevelsDataObject : public wxDataObjectSimple {
public:
    LevelsDataObject() : wxDataObjectSimple(GetDataFormat()) { }
    static wxDataFormat GetDataFormat();
    bool AddLevel(const Level* level);
    bool HasData() const {return !msccdata.empty();}
    wxUint32 GetLevelCount() const {return msccdata.size();}
    bool GetLevels(std::vector<CountedPtr<Level> >& levels) const;
    virtual size_t GetDataSize() const;
    virtual bool GetDataHere(void* buf) const;
    virtual bool SetData(size_t len, const void* buf);
private:
    std::list<std::string> msccdata;
};

class LevelSectionDataObject : public wxDataObjectSimple {
public:
    LevelSectionDataObject() : wxDataObjectSimple(GetDataFormat()), width(0), height(0) { }
    static wxDataFormat GetDataFormat();
    bool SetSection(const Level* level, wxUint32 x = 0, wxUint32 y = 0, wxUint32 w = 32, wxUint32 h = 32);
    bool HasData() const {return width > 0 && height > 0;}
    wxUint32 GetWidth() const {return width;}
    wxUint32 GetHeight() const {return height;}
    bool PasteToLevel(CountedPtr<Level> target, wxUint32 x = 0, wxUint32 y = 0) const;
    virtual size_t GetDataSize() const;
    virtual bool GetDataHere(void* buf) const;
    virtual bool SetData(size_t len, const void* buf);
private:
    wxUint32 width;
    wxUint32 height;
    std::string msccdata;
};

}

#endif // !CHIPW_LEVELDATAOBJ_H_INCLUDED
