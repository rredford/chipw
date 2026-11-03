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

#include "leveldataobj.h"
#include "level.h"
#include <sstream>
#include <cstring>
#include "binstream.h"

namespace ChipW {

// LevelsDataObject

wxDataFormat LevelsDataObject::GetDataFormat() {
    return wxT("CHIPEDIT LEVELS");
}

bool LevelsDataObject::AddLevel(const Level* level) {
    if(level == NULL)
        return false;
    std::ostringstream stream;
    if(!level->Save_SubMSCC(stream, false))
        return false;
    if(stream.str().size() > 65535)
        return false;
    msccdata.push_back(stream.str());
    return true;
}

bool LevelsDataObject::GetLevels(std::vector<CountedPtr<Level> >& levels) const {
    std::vector<CountedPtr<Level> >::size_type origsize = levels.size();
    levels.reserve(levels.size() + msccdata.size());
    CountedPtr<Level> level = NULL;
    for(std::list<std::string>::const_iterator it = msccdata.begin(); it != msccdata.end(); ++it) {
        std::istringstream stream(*it);
        level = new Level;
        if(it->size() > 65535 || !level->Load_SubMSCC(stream, false, it->size())) {
            levels.resize(origsize);
            return false;
        }
        levels.push_back(level);
    }
    return true;
}

size_t LevelsDataObject::GetDataSize() const {
    size_t result = 4; // Uint32 level count
    result += 6 * msccdata.size(); // Uint16 level data size + 4 unused bytes, for each level
    for(std::list<std::string>::const_reverse_iterator it = msccdata.rbegin(); it != msccdata.rend(); ++it)
        result += it->size();
    return result;
}

bool LevelsDataObject::GetDataHere(void* buf) const {
//    wxPrintf(wxT("Trying to copy from LevelsDataObject\n"));
    char* bufptr = (char*) buf;
    std::list<std::string>::const_reverse_iterator it;
    // Level count.
    *((wxUint32*) bufptr) = wxUINT32_SWAP_ON_BE((wxUint32) msccdata.size());
    bufptr += 4;
    // Level data sizes and filler bytes.
    // The levels are stored in reverse order because that is how ChipEdit does it.
    for(it = msccdata.rbegin(); it != msccdata.rend(); ++it) {
        *((wxUint16*) bufptr) = wxUINT16_SWAP_ON_BE((wxUint16) it->size());
        bufptr += 2;
        *((wxUint32*) bufptr) = 0;
        bufptr += 4;
    }
    // Level data.
    for(it = msccdata.rbegin(); it != msccdata.rend(); ++it) {
        std::memcpy(bufptr, it->data(), it->size());
        bufptr += it->size();
    }
    return true;
}

bool LevelsDataObject::SetData(size_t len, const void* buf) {
//    wxPrintf(wxT("Trying to paste into LevelsDataObject\n"));
    const char* bufptr = (const char*) buf;
    msccdata.clear();
    // Level count.
    if(len < 4)
        return false;
    wxUint32 levelcount = wxUINT32_SWAP_ON_BE(*((const wxUint32*) bufptr));
    if(levelcount > 65535)
        return false;
    bufptr += 4;
    len -= 4;
    // Level data sizes and filler bytes.
    std::list<wxUint16> leveldatasizes;
    while(levelcount > 0) {
        if(len < 6)
            return false;
        leveldatasizes.push_back(wxUINT16_SWAP_ON_BE(*((const wxUint16*) bufptr)));
        bufptr += 6;
        len -= 6;
        --levelcount;
    }
    // Level data.
    CountedPtr<Level> level = NULL;
    for(std::list<wxUint16>::const_iterator it = leveldatasizes.begin(); it != leveldatasizes.end(); ++it) {
        if(len < *it)
            return false;
        msccdata.push_front(std::string(bufptr, *it)); // push_front because the levels are copied in reverse order
        bufptr += *it;
        len -= *it;
    }
    return true;
}

// LevelSectionDataObject

wxDataFormat LevelSectionDataObject::GetDataFormat() {
    return wxT("CHIPEDIT MAPSECT");
}

bool LevelSectionDataObject::SetSection(const Level* level, wxUint32 x, wxUint32 y, wxUint32 w, wxUint32 h) {
    if(level == NULL)
        return false;
    // Create a level object for the selected map section.
    Level tmplevel;
    tmplevel.RefStatic();
    tmplevel.levelnumber = level->levelnumber;
    tmplevel.time = level->time;
    tmplevel.chips = level->chips;
    tmplevel.title = level->title;
    tmplevel.hint = level->hint;
    tmplevel.psw = level->psw;
    for(wxUint32 x1 = 0; x1 < w; ++x1) {
        for(wxUint32 y1 = 0; y1 < h; ++y1) {
            if(!tmplevel.PlaceTileUpper(x1, y1, level->GetUpperTile(x + x1, y + y1), false))
                return false;
            if(!tmplevel.PlaceTileLower(x1, y1, level->GetLowerTile(x + x1, y + y1), false))
                return false;
        }
    }
    MonsterRecord tmpmon;
    std::list<MonsterRecord>::const_iterator monit;
    for(monit = level->monsters.begin(); monit != level->monsters.end(); ++monit) {
        if(monit->x >= x && monit->x < x + w && monit->y >= y && monit->y < y + h) {
            tmpmon.x = monit->x - x;
            tmpmon.y = monit->y - y;
            tmplevel.monsters.push_back(tmpmon);
        }
    }
    WireRecord tmpwire;
    std::list<WireRecord>::const_iterator wireit;
    for(wireit = level->trapwires.begin(); wireit != level->trapwires.end(); ++wireit) {
        if(wireit->buttonx >= x && wireit->buttonx < x + w && wireit->buttony >= y && wireit->buttony < y + h
        && wireit->targetx >= x && wireit->targetx < x + w && wireit->targety >= y && wireit->targety < y + h) {
            tmpwire.buttonx = wireit->buttonx - x;
            tmpwire.buttony = wireit->buttony - y;
            tmpwire.targetx = wireit->targetx - x;
            tmpwire.targety = wireit->targety - y;
            tmplevel.trapwires.push_back(tmpwire);
        }
    }
    for(wireit = level->clonewires.begin(); wireit != level->clonewires.end(); ++wireit) {
        if(wireit->buttonx >= x && wireit->buttonx < x + w && wireit->buttony >= y && wireit->buttony < y + h
        && wireit->targetx >= x && wireit->targetx < x + w && wireit->targety >= y && wireit->targety < y + h) {
            tmpwire.buttonx = wireit->buttonx - x;
            tmpwire.buttony = wireit->buttony - y;
            tmpwire.targetx = wireit->targetx - x;
            tmpwire.targety = wireit->targety - y;
            tmplevel.clonewires.push_back(tmpwire);
        }
    }
    // Convert it to MSCC format.
    std::ostringstream stream;
    if(!tmplevel.Save_SubMSCC(stream, false))
        return false;
    if(stream.str().size() > 65535)
        return false;
    msccdata = stream.str();
    width = w;
    height = h;
    return true;
}

bool LevelSectionDataObject::PasteToLevel(CountedPtr<Level> target, wxUint32 x, wxUint32 y) const {
    if(target == NULL)
        return false;
    if(msccdata.size() > 65535)
        return false;
    Level tmplevel;
    tmplevel.RefStatic();
    std::istringstream stream(msccdata);
    if(!tmplevel.Load_SubMSCC(stream, false, msccdata.size()))
        return false;
    wxUint32 w = ((x + width > 32) ? (32 - x) : width);
    wxUint32 h = ((y + height > 32) ? (32 - y) : height);
    for(wxUint32 x1 = 0; x1 < w; ++x1) {
        for(wxUint32 y1 = 0; y1 < h; ++y1) {
            if(!target->ClearTile(x + x1, y + y1))
                return false;
            if(!target->PlaceTileUpper(x + x1, y + y1, tmplevel.GetUpperTile(x1, y1), false))
                return false;
            if(!target->PlaceTileLower(x + x1, y + y1, tmplevel.GetLowerTile(x1, y1), false))
                return false;
        }
    }
    std::list<MonsterRecord>::const_iterator monit;
    for(monit = tmplevel.monsters.begin(); monit != tmplevel.monsters.end(); ++monit) {
        if(monit->x < w && monit->y < h) {
            if(!target->AddMonster(x + monit->x, y + monit->y, true))
                return false;
        }
    }
    WireRecord tmpwire;
    std::list<WireRecord>::const_iterator wireit;
    for(wireit = tmplevel.trapwires.begin(); wireit != tmplevel.trapwires.end(); ++wireit) {
        if(wireit->buttonx < w && wireit->buttony < h && wireit->targetx < w && wireit->targety < h) {
            tmpwire.buttonx = wireit->buttonx + x;
            tmpwire.buttony = wireit->buttony + y;
            tmpwire.targetx = wireit->targetx + x;
            tmpwire.targety = wireit->targety + y;
            if(!target->AddTrapWire(tmpwire, true))
                return false;
        }
    }
    for(wireit = tmplevel.clonewires.begin(); wireit != tmplevel.clonewires.end(); ++wireit) {
        if(wireit->buttonx < w && wireit->buttony < h && wireit->targetx < w && wireit->targety < h) {
            tmpwire.buttonx = wireit->buttonx + x;
            tmpwire.buttony = wireit->buttony + y;
            tmpwire.targetx = wireit->targetx + x;
            tmpwire.targety = wireit->targety + y;
            if(!target->AddCloneWire(tmpwire, true))
                return false;
        }
    }
    return true;
}

size_t LevelSectionDataObject::GetDataSize() const {
    return 14 // Uint32 width + Uint32 height + Uint16 level data size + 4 unused bytes
        + msccdata.size();
}

bool LevelSectionDataObject::GetDataHere(void* buf) const {
//    wxPrintf(wxT("Trying to copy from LevelSectionDataObject\n"));
    char* bufptr = (char*) buf;
    // Width.
    *((wxUint32*) bufptr) = wxUINT32_SWAP_ON_BE(width);
    bufptr += 4;
    // Height.
    *((wxUint32*) bufptr) = wxUINT32_SWAP_ON_BE(height);
    bufptr += 4;
    // Level data size.
    *((wxUint16*) bufptr) = wxUINT16_SWAP_ON_BE((wxUint16) msccdata.size());
    bufptr += 2;
    // Filler bytes.
    *((wxUint32*) bufptr) = 0;
    bufptr += 4;
    // Level data.
    std::memcpy(bufptr, msccdata.data(), msccdata.size());
    bufptr += msccdata.size();
    return true;
}

bool LevelSectionDataObject::SetData(size_t len, const void* buf) {
//    wxPrintf(wxT("Trying to paste into LevelSectionDataObject\n"));
    const char* bufptr = (const char*) buf;
    width = 0;
    height = 0;
    // Width.
    if(len < 4)
        return false;
    wxUint32 w = wxUINT32_SWAP_ON_BE(*((const wxUint32*) bufptr));
    if(w > 32)
        return false;
    bufptr += 4;
    len -= 4;
    // Height.
    if(len < 4)
        return false;
    wxUint32 h = wxUINT32_SWAP_ON_BE(*((const wxUint32*) bufptr));
    if(h > 32)
        return false;
    bufptr += 4;
    len -= 4;
    // Level data size and filler bytes.
    if(len < 6)
        return false;
    wxUint16 leveldatasize = wxUINT16_SWAP_ON_BE(*((const wxUint16*) bufptr));
    bufptr += 6;
    len -= 6;
    // Level data.
    if(len < leveldatasize)
        return false;
    msccdata.assign(bufptr, leveldatasize);
    bufptr += leveldatasize;
    len -= leveldatasize;
    width = w;
    height = h;
    return true;
}

}

