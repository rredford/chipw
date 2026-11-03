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

#include "playtestproc.h"
#include "wxtostdstream.h"
#include "app.h"
#include "levelset.h"
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/filefn.h>
#include <wx/log.h>
#include <wx/wfstream.h>

namespace ChipW {

PlayTestProcess::PlayTestProcess() {
}

PlayTestProcess::~PlayTestProcess() {
    // Delete the temporary file.
    if(!tmpfilename.empty())
        wxGetApp().RemoveTmpFile(tmpfilename);
}

wxString PlayTestProcess::GetArgumentQuoted(wxString arg) {
    // It's platform dependent and wxWidgets does not provide a way to do it which works across all supported versions......
#ifdef __WXMSW__
    // Looks like we are on Windows.
    // Infuriatingly, we need to escape exactly the following with backslashes:
    // * Double quotes
    // * Runs of backslashes immediately before double quotes
    // So, we scan backwards for double quotes, count the run of backslashes before each one, then add the correct number for escaping.
    size_t pos = arg.size();
    while(pos > 0) {
        // Find the next quote.
        pos = arg.rfind(wxT('"'), pos - 1);
        if(pos >= arg.size())
            break;
        // We always escape the quote itself.
        size_t escapecount = 1;
        // Now scan for preceding backslashes.
        while(pos > 0 && arg[pos - 1] == '\\') {
            ++escapecount;
            --pos;
        }
        // pos is now on either the quote itself or the start of a preceding run of backslashes,
        // so inserting the appropriate number of backslashes here will give correct escaping.
        arg.insert(pos, escapecount, '\\');
    }
#else
    // Assume it's like Unix, though (FIXME) there are probably others.
    // Escape backslashes with backslashes.
    arg.Replace(wxT("\\"), wxT("\\\\"));
    // Escape double quotes with backslashes.
    arg.Replace(wxT("\""), wxT("\\\""));
#endif
    // Put the whole thing in double quotes.
    return wxT("\"") + arg + wxT("\"");
}

bool PlayTestProcess::Launch(wxString gamecmd, const LevelSet& levelset, wxUint16 levelnumber, wxString workingdir) {
    if(!tmpfilename.empty())
        return false;
    // Save the levelset to a temporary file.
    {
        // Obtain name for temporary file and open it.
        wxFile tmpfile;
        tmpfilename = wxFileName::CreateTempFileName(wxT("chipw-tmp-"), &tmpfile);
        if(tmpfilename.empty()) {
            wxLogError(wxT("Failed to obtain temporary filename."));
            return false;
        }
        // Register the temporary file with the App object.
        // This means that it will be deleted when ChipW exits even if the game process is still running.
        // This might upset the game process, but that is better than leaving temporary files around.
        wxGetApp().RegisterTmpFile(tmpfilename);
        if(!tmpfile.IsOpened()) {
            wxLogError(wxT("Failed to open temporary file."));
            return false;
        }
        // Write the levelset data.
        {
            wxFileOutputStream wxstream(tmpfile);
            {
                owxstream stream(&wxstream);
                if(!levelset.Save_MSCC(stream)) {
                    wxLogError(wxT("Failed to save levelset to temporary file."));
                    return false;
                }
            }
        }
    }
    // Enter the appropriate directory.
    wxString oldworkingdir;
    if(!workingdir.empty()) {
        oldworkingdir = wxGetCwd();
        if(!wxSetWorkingDirectory(workingdir)) {
            wxLogError(wxT("Failed to set working directory."));
            return false;
        }
    }
    // Add command line arguments.
    gamecmd << wxT(" ") << GetArgumentQuoted(tmpfilename) << wxT(" ") << levelnumber;
    // Start the game process.
    bool success = wxExecute(gamecmd, wxEXEC_ASYNC, this);
    if(!success)
        wxLogError(wxT("Failed to start game process."));
    // Restore the working directory.
    if(!oldworkingdir.empty())
        wxSetWorkingDirectory(oldworkingdir);
    // Return.
    return success;
}

}
