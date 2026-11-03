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

#include "tileset.h"
#include "level.h"
#include "refcount.h"
#include <istream>
#include <fstream>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/dc.h>
#include <wx/dcmemory.h>
#include <wx/gdicmn.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/confbase.h>

namespace ChipW {

class TilesetData : public RefCounted {
public:
    TilesetData() : size(-1), handlemagenta(true) { }
    // Tile size.
    wxCoord size;
    // Whether the Tile World magenta transparency behaviour should be used.
    bool handlemagenta;
    // Original image(s), used to regenerate the bitmaps upon scaling.
    wxImage img;
    wxImage img2;
    // Bitmaps used to draw tiles.
    wxBitmap bmpf[TILE_MAX_VALID + 2];     // Tile on floor.
    wxBitmap bmpu[TILE_MAX_VALID + 2];     // Tile on top of another tile.
};

Tileset::Tileset()
 : w(0), h(0), data(NULL)
{
}

Tileset::Tileset(const wxString& filename, wxCoord width, wxCoord height)
 : w(0), h(0), data(NULL)
{
    LoadFile(filename, width, height);
}

Tileset::Tileset(const Tileset& copy)
 : w(0), h(0), data(NULL)
{
    *this = copy;
}

Tileset& Tileset::operator=(const Tileset& copy) {
    w = copy.w;
    h = copy.h;
    if(copy.data != NULL)
        copy.data->RefAdd();
    if(data != NULL)
        data->RefDel();
    data = copy.data;
    return *this;
}

Tileset::~Tileset() {
    if(data != NULL)
        data->RefDel();
}

namespace {

#if wxCHECK_VERSION(2, 8, 0)
template<class wxNuisance>
inline bool CheckOk(const wxNuisance& nuisance) {return nuisance.IsOk();}
inline void BmpCopy(wxBitmap& to, const wxBitmap& from) {to = from;}
#else
template<class wxNuisance>
inline bool CheckOk(const wxNuisance& nuisance) {return nuisance.Ok();}
inline void BmpCopy(wxBitmap& to, const wxBitmap& from) {to = from.GetSubBitmap(wxRect(0, 0, from.GetWidth(), from.GetHeight()));}
#endif

bool CheckMaxSize(wxCoord w, wxCoord h) {
    wxConfigBase* config = wxConfigBase::Get();
    long maxw = 128, maxh = 128, maxarea = 16384;
    if(config != NULL) {
        config->Read(wxT("MaxTileWidth"), &maxw);
        config->Read(wxT("MaxTileHeight"), &maxh);
        config->Read(wxT("MaxTileArea"), &maxarea);
    }
    if(maxw > 0 && w > maxw) {
        wxLogError(wxT("The maximum supported tile width is %i."), (int) maxw);
        return false;
    }
    if(maxh > 0 && h > maxh) {
        wxLogError(wxT("The maximum supported tile height is %i."), (int) maxh);
        return false;
    }
    if(maxarea > 0 && w > 0 && h > 0 && w * h > maxarea) {
        wxLogError(wxT("The maximum supported tile area is %i."), (int) maxarea);
        return false;
    }
    return true;
}

bool Transfer(const wxBitmap& src, wxBitmap& dst, wxCoord w, wxCoord h, wxCoord size) {
    if(!CheckOk(src) || !CheckOk(dst)) {
        wxLogError(wxT("Bitmap error."));
        return false;
    }
    wxMemoryDC dc;
    dc.SelectObject(dst);
    if(!CheckOk(dc)) {
        wxLogError(wxT("Device context error."));
        return false;
    }
    dc.SetUserScale((double) w / size, (double) h / size);
    dc.DrawBitmap(src, 0, 0, true);
    return true;
}

bool Transfer(const wxImage& src, wxBitmap& dst, wxCoord w, wxCoord h, wxCoord size, bool transfermask) {
    if(!CheckOk(src)) {
        wxLogError(wxT("Image error."));
    }
    if(!Transfer(wxBitmap(src), dst, w, h, size))
        return false;
    if(transfermask) {
        if(src.HasMask()) {
            wxBitmap src2(src.ConvertToMono(src.GetMaskRed(), src.GetMaskGreen(), src.GetMaskBlue()));
            wxBitmap mask(dst.GetWidth(), dst.GetHeight());
            if(!CheckOk(src2) || !CheckOk(mask)) {
                wxLogError(wxT("Bitmap error."));
                return false;
            }
            wxMemoryDC dc;
            dc.SelectObject(mask);
            if(!CheckOk(dc)) {
                wxLogError(wxT("Device context error."));
                return false;
            }
            dc.SetBackground(*wxWHITE_BRUSH);
            dc.Clear();
            dc.SetUserScale((double) w / size, (double) h / size);
//            dc.SetLogicalFunction(wxSET);
            dc.DrawBitmap(src2, 0, 0, true);
            dc.SelectObject(wxNullBitmap);
            dst.SetMask(new wxMask(mask, *wxWHITE));
        } else {
            dst.SetMask(NULL);
        }
    }
    return true;
}

bool DoLoad(TilesetData* data, wxCoord& w, wxCoord& h) {
    if(data == NULL)
        return false;
    // Get the images.
    if(!CheckOk(data->img))
        return false;
    wxImage& img = data->img;
    wxImage& img2 = data->img2;
    // Find the native tile size.
    wxCoord& size = data->size;
    if(size < 1) {
        // Reject any image whose height is not a multiple of 16.
        if(img.GetHeight() % 16 != 0) {
            wxLogError(wxT("The image height must be a multiple of 16."));
            return false;
        }
        // The image is always 16 tiles high, and tiles are always square.
        size = img.GetHeight() / 16;
        if(size < 1) {
            wxLogError(wxT("Invalid tile size."));
            return false;
        }
    } else {
        if(img.GetHeight() != 16 * size) {
            wxLogError(wxT("The image height must be 16 times the tile size."));
            return false;
        }
    }
    // Fail if the native tile size is too high.
    if(!CheckMaxSize(size, size))
        return false;
    // Fail if the image is not at least one tile wide and high.
    if(img.GetWidth() < size || img.GetHeight() < size) {
        wxLogError(wxT("The image is not large enough to contain a tile."));
        return false;
    }
    // Fill in width and height if not set.
    if(w < 1)
        w = size;
    if(h < 1)
        h = size;
    // Fail if the tile size is too high.
    if(!CheckMaxSize(size, size))
        return false;
    // Avoiding alpha simplifies the code, and it doesn't seem to work well in wxWidgets anyway.
    if(img.HasAlpha() && !img.ConvertAlphaToMask()) {
        wxLogError(wxT("Failed to generate image mask."));
        return false;
    }
    if(CheckOk(img2) && img2.HasAlpha() && !img2.ConvertAlphaToMask()) {
        wxLogError(wxT("Failed to generate secondary image mask."));
        return false;
    }
    // Counters.
    int x, y;
    // Initialise bitmaps.
    {
        // Generate a bitmap filled with black.
        wxBitmap bmpblank(w, h);
        if(!CheckOk(bmpblank)) {
            wxLogError(wxT("Bitmap error."));
            return false;
        }
        wxMemoryDC dc;
        dc.SelectObject(bmpblank);
        if(!CheckOk(dc)) {
            wxLogError(wxT("Device context error."));
            return false;
        }
        dc.SetBackground(*wxBLACK_BRUSH);
        dc.Clear();
        dc.SelectObject(wxNullBitmap);
        // Use this for the invalid tile.
        BmpCopy(data->bmpu[TILE_INVALID], bmpblank);
        BmpCopy(data->bmpf[TILE_INVALID], bmpblank);
        // Make a transparent version of this.
        wxBitmap bmptrans;
        BmpCopy(bmptrans, bmpblank);
        bmptrans.SetMask(new wxMask(bmptrans, *wxBLACK));
        // Set all the upper-layer bitmaps to use this.
        for(x = 0; x <= TILE_MAX_VALID; ++x)
            BmpCopy(data->bmpu[x], bmptrans);
        // The top-left tile is always the opaque empty tile.
        wxBitmap bmpempty(img.GetSubImage(wxRect(0, 0, size, size)));
        // Generate a scaled version.
        wxBitmap bmpempty2;
        BmpCopy(bmpempty2, bmpblank);
        if(!Transfer(bmpempty, bmpempty2, w, h, size))
            return false;
        // Set all the on-floor bitmaps to use this.
        for(x = 0; x <= TILE_MAX_VALID; ++x)
            BmpCopy(data->bmpf[x], bmpempty2);
    }
    // Determine the type of image by the number and dimensions.
    if(CheckOk(img2)) {
        // CCEdit multi-image format.
        // Check dimensions.
        if(img.GetWidth() != 7 * size) { // img height was checked earlier.
            wxLogError(wxT("Incorrect image dimensions."));
            return false;
        }
        if(img2.GetWidth() != 7 * size || img2.GetHeight() != 16 * size) {
            wxLogError(wxT("Incorrect secondary image dimensions."));
            return false;
        }
        // img = on-floor
        // img2 = upper-layer (with mask already applied)
        for(y = 0; y < 16; ++y) {
            for(x = 0; x < 7; ++x) {
                {   // On floor
                    // Extract tile image.
                    wxImage subimg = img.GetSubImage(wxRect(x * size, y * size, size, size));
                    if(!CheckOk(subimg)) {
                        wxLogError(wxT("Image error."));
                        return false;
                    }
                    // Transfer to on-floor bitmap.
                    if(!Transfer(subimg, data->bmpf[y + 16 * x], w, h, size, false))
                        return false;
                }
                {   // Upper layer
                    // Extract tile image.
                    wxImage subimg = img2.GetSubImage(wxRect(x * size, y * size, size, size));
                    if(!CheckOk(subimg)) {
                        wxLogError(wxT("Image error."));
                        return false;
                    }
                    // Add magenta transparency if necessary.
                    if(data->handlemagenta && !subimg.HasMask())
                        subimg.SetMaskColour(255, 0, 255);
                    // Transfer to upper-layer bitmap.
                    if(!Transfer(subimg, data->bmpu[y + 16 * x], w, h, size, true))
                        return false;
                }
            }
        }
    } else if(img.GetWidth() == 7 * size) {
        // Tile World small format.
        // Simple grid of tile images.
        for(y = 0; y < 16; ++y) {
            for(x = 0; x < 7; ++x) {
                // Extract tile image.
                wxImage subimg = img.GetSubImage(wxRect(x * size, y * size, size, size));
                if(!CheckOk(subimg)) {
                    wxLogError(wxT("Image error."));
                    return false;
                }
                // The last 3 columns get magenta as a mask colour if there is no inherent transparency.
                if(x >= 4 && data->handlemagenta && !subimg.HasMask())
                    subimg.SetMaskColour(255, 0, 255);
                // Transfer to both bitmaps.
                if(!Transfer(subimg, data->bmpf[y + 16 * x], w, h, size, false))
                    return false;
                if(!Transfer(subimg, data->bmpu[y + 16 * x], w, h, size, true))
                    return false;
            }
        }
    } else if(img.GetWidth() == 13 * size) {
        // MSCC / Tile World / CCEdit masked format.
        // On-floor images in first 7 columns.
        for(y = 0; y < 16; ++y) {
            for(x = 0; x < 7; ++x) {
                // Extract tile image.
                wxImage subimg = img.GetSubImage(wxRect(x * size, y * size, size, size));
                if(!CheckOk(subimg)) {
                    wxLogError(wxT("Image error."));
                    return false;
                }
                // Transfer to on-floor bitmap.
                if(!Transfer(subimg, data->bmpf[y + 16 * x], w, h, size, false))
                    return false;
                // For the first 4 columns, this is the only image, so use it for the upper-layer too.
                if(x < 4) {
                    if(!Transfer(subimg, data->bmpu[y + 16 * x], w, h, size, true))
                        return false;
                }
            }
        }
        // Upper-layer images in next 3 columns, with masks in final 3.
        // Unfortunately, MSCC and Tile World use black for the mask, but CCEdit uses white.
        // However, both use a white background for the actual images.
        // Thus, we search for a non-white pixel in the image section, then take the mask to be the
        // inverse of the corresponding pixel of the mask image.
        // This will work as long as the tile images have at least one non-white pixel.
        unsigned char maskr = 0, maskg = 0, maskb = 0;
        for(y = 0; y < 16 * size; ++y) {
            for(x = 7 * size; x < 10 * size; ++x) {
                if(img.GetRed(x, y) != 255 || img.GetGreen(x, y) != 255 || img.GetBlue(x, y) != 255) {
                    maskr = 255 - img.GetRed(x + 3 * size, y);
                    maskg = 255 - img.GetGreen(x + 3 * size, y);
                    maskb = 255 - img.GetBlue(x + 3 * size, y);
                    x = 10 * size - 1;
                    y = 16 * size - 1;
                }
            }
        }
        // Phew... now we can actually get the images.
        for(y = 0; y < 16; ++y) {
            for(x = 4; x < 7; ++x) {
                // Extract tile image.
                wxImage subimg = img.GetSubImage(wxRect((x + 3) * size, y * size, size, size));
                if(!CheckOk(subimg)) {
                    wxLogError(wxT("Image error."));
                    return false;
                }
                // Set mask.
                if(!subimg.SetMaskFromImage(img.GetSubImage(wxRect((x + 6) * size, y * size, size, size)),
                maskr, maskg, maskb)) {
                    wxLogError(wxT("Failed to transfer image mask."));
                    return false;
                }
                // Transfer to upper-layer bitmap.
                if(!Transfer(subimg, data->bmpu[y + 16 * x], w, h, size, true))
                    return false;
            }
        }
    } else {
        // Unrecognised format.
        wxLogError(wxT("Incorrect image dimensions."));
        return false;
    }
    return true;
}

bool IsCCToolsINI(const wxString& filename) {
    if(filename.empty())
        return false;
    std::ifstream stream(filename.mb_str());
    char headerbuf[10];
    if(!stream.read(headerbuf, 9))
        return false;
    stream.close();
    headerbuf[9] = 0;
    wxString header(headerbuf, wxConvUTF8);
    header.MakeLower();
    return header == wxT("[tileset]");
}

}

bool Tileset::LoadFile(const wxString& filename, wxCoord width, wxCoord height) {
    // If the requested size is too high, don't even try to load the image.
    if(!CheckMaxSize(width, height))
        return false;
    // Create the new data object.
    TilesetData* newdata = new TilesetData;
    newdata->RefAdd();
    // Use the appropriate file format.
    if(IsCCToolsINI(filename)) {
        // CCTools text file.
        wxTextFile textfile(filename);
        if(!textfile.Open()) {
            wxLogError(wxT("Failed to read tileset file as text."));
            newdata->RefDel();
            return false;
        }
        wxString line;
        wxString sect, key, val;
        wxString f, u, m;
        wxString combined;
        wxString list;
        size_t pos;
        for(line = textfile.GetFirstLine(); !textfile.Eof(); line = textfile.GetNextLine()) {
            if(line.size() > 2 && line[0] == wxT('[') && line[line.size() - 1] == wxT(']')) {
                sect = line.substr(1, line.size() - 2);
                sect.MakeLower();
            } else if(line.size() > 0) {
                pos = line.find(wxT('='));
                if(pos == 0 || pos >= line.size() - 1)
                    continue;
                key = line.substr(0, pos);
                key.MakeLower();
                key.Trim(true);
                key.Trim(false);
                val = line.substr(pos + 1);
                val.Trim(true);
                val.Trim(false);
                if(sect == wxT("tileset")) {
                    if(key == wxT("tiles"))
                        f = val;
                    else if(key == wxT("mtiles"))
                        u = val;
                    else if(key == wxT("mtilesmask"))
                        m = val;
                    else if(key == wxT("unmaskedtiles"))
                        combined = val;
                    else if(key == wxT("listtiles"))
                        list = val;
                    else if(key == wxT("size")) {
                        long l;
                        if(val.ToLong(&l))
                            newdata->size = l;
                    }
                }
            }
        }
        wxFileName fn1(filename);
        wxFileName fn;
        if(!f.empty()) {
            fn = f;
            fn.MakeAbsolute(fn1.GetPath());
            if(!newdata->img.LoadFile(fn.GetFullPath())) {
                wxLogError(wxT("Failed to load tile-on-floor image file."));
                newdata->RefDel();
                return false;
            }
        }
        if(!u.empty()) {
            fn = u;
            fn.MakeAbsolute(fn1.GetPath());
            if(!newdata->img2.LoadFile(fn.GetFullPath())) {
                wxLogError(wxT("Failed to load upper-tile image file."));
                newdata->RefDel();
                return false;
            }
        }
        if(!CheckOk(newdata->img))
            newdata->img = newdata->img2;
        else if(!CheckOk(newdata->img2))
            newdata->img2 = newdata->img;
        if(!m.empty() && CheckOk(newdata->img2)) {
            fn = m;
            fn.MakeAbsolute(fn1.GetPath());
            wxImage img3;
            if(!img3.LoadFile(fn.GetFullPath())) {
                wxLogError(wxT("Failed to load mask image file."));
                newdata->RefDel();
                return false;
            }
            if(!newdata->img2.SetMaskFromImage(img3, 255, 255, 255)) {
                wxLogError(wxT("Failed to set image mask."));
                newdata->RefDel();
                return false;
            }
            newdata->handlemagenta = false;
        }
        if(!CheckOk(newdata->img)) {
            if(CheckOk(newdata->img2)) {
                newdata->img = newdata->img2;
                newdata->img2 = wxImage();
            } else if (!combined.empty()) {
                fn = combined;
                fn.MakeAbsolute(fn1.GetPath());
                if(!newdata->img.LoadFile(fn.GetFullPath())) {
                    wxLogError(wxT("Failed to load combined image file."));
                    newdata->RefDel();
                    return false;
                }
            } else {
                wxLogError(wxT("No suitable images found."));
                newdata->RefDel();
                return false;
            }
        }
    } else {
        // Plain old image file.
        if(!newdata->img.LoadFile(filename)) {
            wxLogError(wxT("Failed to load tileset file as image."));
            newdata->RefDel();
            return false;
        }
    }
    // Interpret the image.
    if(!DoLoad(newdata, width, height)) {
        wxLogError(wxT("Failed to parse tile images."));
        newdata->RefDel();
        return false;
    }
    if(data != NULL)
        data->RefDel();
    data = newdata;
    w = width;
    h = height;
    return true;
}

bool Tileset::Resize(wxCoord width, wxCoord height) {
    if(data == NULL)
        return false;
    if(width == w && height == h)
        return true;
    if(!CheckMaxSize(width, height))
        return false;
    TilesetData* newdata = new TilesetData;
    newdata->RefAdd();
    newdata->size = data->size;
    newdata->handlemagenta = data->handlemagenta;
    newdata->img = data->img;
    newdata->img2 = data->img2;
    if(!DoLoad(newdata, width, height)) {
        wxLogError(wxT("Failed to parse tile images."));
        newdata->RefDel();
        return false;
    }
    if(data != NULL)
        data->RefDel();
    data = newdata;
    w = width;
    h = height;
    return true;
}

wxCoord Tileset::GetNativeTileWidth() const {
    if(data == NULL || !CheckOk(data->img))
        return 0;
    return data->img.GetHeight() / 16;
}

wxCoord Tileset::GetNativeTileHeight() const {
    if(data == NULL || !CheckOk(data->img))
        return 0;
    return data->img.GetHeight() / 16;
}

bool Tileset::DrawTile(Tile tile, wxDC* target, wxCoord x, wxCoord y) {
    if(target == NULL)
        return true;
    if(data == NULL)
        return false;
    if(tile > TILE_MAX_VALID)
        tile = TILE_INVALID;
    else
        target->DrawBitmap(data->bmpu[tile], x, y, true);
    return CheckOk(*target);
}

bool Tileset::DrawPair(Tile upper, Tile lower, wxDC* target, wxCoord x, wxCoord y) {
    if(target == NULL)
        return true;
    if(data == NULL)
        return false;
    if(upper > TILE_MAX_VALID)
        upper = TILE_INVALID;
    if(lower > TILE_MAX_VALID)
        lower = TILE_INVALID;
    if(lower == TILE_FLOOR) {
        target->DrawBitmap(data->bmpf[upper], x, y, false);
    } else {
        target->DrawBitmap(data->bmpf[lower], x, y, false);
        target->DrawBitmap(data->bmpu[upper], x, y, true);
    }
    return CheckOk(*target);
}

bool Tileset::DrawLevel(const Level* level, wxDC* target, wxCoord x, wxCoord y, bool drawupper, bool drawlower) {
    if(target == NULL || level == NULL)
        return true;
    if(data == NULL)
        return false;
    wxUint32 levx, levy;
    Tile upper = TILE_FLOOR, lower = TILE_FLOOR;
    bool success = true;
    for(levy = 0; levy < 32; ++levy) {
        for(levx = 0; levx < 32; ++levx) {
            if(drawupper) {
                upper = level->GetUpperTile(levx, levy);
                if(drawlower)
                    lower = level->GetLowerTile(levx, levy);
            } else if(drawlower) {
                upper = level->GetLowerTile(levx, levy);
            }
            if(!DrawPair(upper, lower, target, x + levx * w, y + levy * h))
                success = false;
        }
    }
    return success;
}

}

