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

#include "playtestmgr.h"
#include "leveleditids.h"
#include "ruleset.h"
#include <wx/confbase.h>

namespace ChipW {

PlayTestManager::PlayTestManager()
 : ListMenuManager(ID_PLAYTEST_MIN, ID_PLAYTEST_MAX, wxT("Not configured"))
{
    LoadTypes();
}

void PlayTestManager::SetTypes(std::vector<PlayTestType> newtypes, bool saveconfig) {
    std::vector<wxString> newmenuitems(newtypes.size());
    for(size_t i = 0; i < newtypes.size(); ++i)
        newmenuitems[i] = newtypes[i].name;
    SetMenuItems(newmenuitems);
    types.swap(newtypes);
    if(saveconfig)
        SaveTypes();
}

std::vector<PlayTestType> PlayTestManager::GetConfTypes() {
    wxConfigBase* config = wxConfigBase::Get();
    if(config == NULL)
        return std::vector<PlayTestType>();
    wxString prefix = wxT("PlayTestProfiles/");
    long count = config->Read(prefix + wxT("Count"), 0L);
    if(config->Read(prefix + wxT("ConfigVersion"), 2L) < 2)
        count = 0;
    std::vector<PlayTestType> types(count);
    for(unsigned int i = 0; i < types.size(); ++i) {
        wxString profileprefix = prefix;
        profileprefix << wxT("Profile") << (i + 1) << wxT("/");
        config->Read(profileprefix + wxT("Name"), &types[i].name);
        config->Read(profileprefix + wxT("Program"), &types[i].program);
        config->Read(profileprefix + wxT("Arguments"), &types[i].arguments);
        config->Read(profileprefix + wxT("WorkingDirectory"), &types[i].workingdir);
        config->Read(profileprefix + wxT("OverrideRuleSet"), &types[i].ruleset.hasruleset);
        if(types[i].ruleset.hasruleset)
            types[i].ruleset.ruleset = (RuleSetId) config->Read(profileprefix + wxT("RuleSetId"), (long) types[i].ruleset.ruleset);
    }
    return types;
}

void PlayTestManager::SetConfTypes(const std::vector<PlayTestType>& types) {
    wxConfigBase* config = wxConfigBase::Get();
    if(config == NULL)
        return;
    wxString prefix = wxT("PlayTestProfiles/");
    config->Write(prefix + wxT("ConfigVersion"), 2L);
    unsigned int oldcount = config->Read(prefix + wxT("Count"), 0L);
    config->Write(prefix + wxT("Count"), (long) types.size());
    unsigned int i;
    for(i = 0; i < types.size(); ++i) {
        wxString profileprefix = prefix;
        profileprefix << wxT("Profile") << (i + 1) << wxT("/");
        config->Write(profileprefix + wxT("ConfigVersion"), 2L);
        config->Write(profileprefix + wxT("Name"), types[i].name);
        config->Write(profileprefix + wxT("Program"), types[i].program);
        config->Write(profileprefix + wxT("Arguments"), types[i].arguments);
        config->Write(profileprefix + wxT("WorkingDirectory"), types[i].workingdir);
        config->Write(profileprefix + wxT("OverrideRuleSet"), types[i].ruleset.hasruleset);
        if(types[i].ruleset.hasruleset)
            config->Write(profileprefix + wxT("RuleSetId"), (long) types[i].ruleset.ruleset);
    }
    for(; i < oldcount; ++i) {
        wxString profilegroup = prefix;
        profilegroup << wxT("Profile") << (i + 1);
        config->DeleteGroup(profilegroup);
    }
}

}

