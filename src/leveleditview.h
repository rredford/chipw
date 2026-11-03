// Chip's Workshop - a level editor for Chip's Challenge.
// Copyright 2008-2011,2021 Christopher Elsby <chrise@chrise.me.uk>
// Modified 2026 by the Chip's Workshop fork contributors (see README.md for a list of changes).
// This is an unofficial modified version; it is not endorsed by the original author.
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

#ifndef CHIPW_LEVELEDITVIEW_H_INCLUDED
#define CHIPW_LEVELEDITVIEW_H_INCLUDED

#include <wx/docview.h>
#include <wx/event.h>
#include <wx/notebook.h>
#include "tileset.h"
#include "level.h"
#include <map>

class wxBitmap;
class wxListBox;
class wxStatusBar;
class wxDC;
class wxStaticText;

namespace ChipW {

class LevelSet;
class LevelSetDoc;
class LevelMapWindow;
class TileSelectWindow;
class LevelUpdateHint;

class LevelEditView : public wxView {
public:
    LevelEditView();
    virtual ~LevelEditView();
    virtual bool OnCreate(wxDocument* doc, long flags);
    virtual void OnDraw(wxDC* dc);
    virtual void OnUpdate(wxView* sender, wxObject* hint = NULL);
    virtual bool OnClose(bool delwin);
    virtual void OnActivateView(bool activate, wxView *activeView, wxView *deactiveView);
    virtual void OnChangeFilename();
    void DrawTileSelector(wxDC* dc);
    void OnMouse(wxMouseEvent& event);
    CountedPtr<Level> GetLevel() const {return level;}
    void SetLevel(CountedPtr<Level> newlevel);
    LevelSetDoc* GetLevelSetDoc() const;
    LevelSet* GetLevelSet() const;
    const Tileset& GetTileset() const {return tileset;}
    bool SetTileset(const Tileset& tiles);
    bool InitTileset();
    // Tile size at 1:1 (as set by "Scale tiles"); used by the tile selector.
    wxCoord GetTileW() const {return tileset.GetTileWidth();}
    wxCoord GetTileH() const {return tileset.GetTileHeight();}
    // Tile size on the level map at the current zoom.
    wxCoord GetMapTileW() const {return maptileset.IsOk() ? maptileset.GetTileWidth() : GetTileW();}
    wxCoord GetMapTileH() const {return maptileset.IsOk() ? maptileset.GetTileHeight() : GetTileH();}
    // Zoom. The index refers to the table of zoom percentages in leveleditview.cpp.
    int GetZoomPercent() const;
    // Changes the zoom, keeping the map point under 'anchor' (window coordinates) in place.
    // If anchor is NULL, the centre of the map window is used.
    bool SetZoom(int index, const wxPoint* anchor = NULL);
    bool ZoomBy(int steps, const wxPoint* anchor = NULL) {return SetZoom(zoomindex + steps, anchor);}
    // Ends a drag-to-move of the map view, if one is in progress.
    void EndPan();
    bool SetTileSize(wxCoord w = -1, wxCoord h = -1);
    typedef wxUint16 Tool;
    enum {
        TOOL_RECTSELECT = TILE_MAX + 1,
        TOOL_TRAPWIRE,
        TOOL_CLONEWIRE,
        TOOL_NONE,
        TOOL_MIN = TILE_MIN,
        TOOL_MAX = TOOL_NONE - 1,
    };
    static wxString GetToolName(Tool tool, bool hexcode = false);
    Tool GetSelectedTool() const;
    bool SetSelectedTool(Tool tool);
    bool HasSelectedTile() const {return IsTile(GetSelectedTool());}
    Tile GetSelectedTile() const {return GetSelectedTool();}
    bool SetSelectedTile(Tile tile) {return SetSelectedTool(tile);}
    bool HasSelection() const {return selx < 32 && sely < 32 && selw > 0 && selh > 0;}
    bool IsInSelection(wxUint32 x, wxUint32 y) const {return x >= selx && x < (selx + selw) && y >= sely && y < (sely + selh);}
    void SetSelection(wxUint32 x, wxUint32 y, wxUint32 w = 1, wxUint32 h = 1);
    void ClearSelection() {SetSelection(32, 32, 0, 0);}
    void SetStatusText(const wxString& text, int i = 0);
    LevelEditView* NewView(CountedPtr<Level> newlevel);
    LevelEditView* NewView() {return NewView(level);}
    void Refresh();
private:
    void OnCommand(wxCommandEvent& event);
    void OnUpdateUI(wxUpdateUIEvent& event);
    void OnListBox(wxCommandEvent& event);
    void OnNotebookPageChanged(wxNotebookEvent& event);
//    void SendUpdate(wxObject* hint = NULL, bool self = true, bool modify = true);
    wxMemoryDC* NewBackingDC();
    void DrawSelection(wxDC* dc, wxUint32 x, wxUint32 y, wxUint32 w, wxUint32 h, wxCoord tilew, wxCoord tileh);
    void DrawSelection(wxDC* dc) {DrawSelection(dc, selx, sely, selw, selh, GetMapTileW(), GetMapTileH());}
    // Rebuilds the zoomed tileset used to draw the map. Returns false if the size is not supported.
    bool UpdateMapTileset();
    // Applies the map's virtual size/scroll position for the current zoom.
    void UpdateMapScrollbars(int viewx, int viewy);
    void UpdateZoomLabel();
    // Move overlay (drag it to move the view when the map does not fit).
    bool IsPanAvailable() const;
    wxRect GetPanIconRect() const;
    void DrawPanIcon(wxDC* dc);
    void DrawWires(wxDC* dc, bool force = false);
    bool DoTileAction(wxUint32 x, wxUint32 y, wxUint32 tile, wxString action/*, LevelUpdateHint& hint*/, bool single = true);
    bool DoTileActionSel(wxUint32 tile, wxString action/*, LevelUpdateHint& hint*/);
    enum DrawSpec {
        DRAW_NONE,
        DRAW_WIRES,
        DRAW_MAP,
        DRAW_ALL,
    };
    // Schedules a repaint of the map (and, for DRAW_ALL, the tile selector).
    // On modern toolkits (GTK3, Wayland) drawing must happen inside the paint
    // handler, so all screen updates go through Refresh() instead of drawing
    // directly with a wxClientDC.
    void AddIdleRedraw(DrawSpec spec);
    CountedPtr<Level> level;
    Tileset tileset;
    Tileset maptileset;
    std::map<int, Tileset> zoomcache;
    int zoomindex;
    int wheelrotation;
    bool panning;
    bool panhover;
    wxPoint panlock;     // Where the cursor is held during a drag (window coordinates).
    wxPoint panlast;     // Last cursor position seen during a drag.
    wxPoint panview;     // View position being dragged to (map pixels).
    bool panwarp;        // Whether the cursor is being moved back to panlock.
    wxStaticText* zoomlabel;
    static bool loadeddefaulttileset;
    static Tileset defaulttileset;
    wxUint32 selx, sely, selw, selh;
    wxUint32 clickx, clicky;
    bool readonly;
    bool showwires;
    bool showupper;
    int activebutton;
    bool buttonswap;
    LevelMapWindow* mapwin;
    wxStatusBar* statusbar;
    wxBitmap* mapbmp;
    wxListBox* levellist;
    wxListBox* tilelist;
    TileSelectWindow* tilesel;
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(LevelEditView)
};

}

#endif // !CHIPW_LEVELEDITVIEW_H_INCLUDED
