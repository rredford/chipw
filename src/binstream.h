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

#ifndef CHIPW_BINSTREAM_H_INCLUDED
#define CHIPW_BINSTREAM_H_INCLUDED

#include <wx/defs.h>
#include <ostream>
#include <istream>

namespace ChipW {

// Binary I/O.
template<typename T>
inline std::ostream& Write(std::ostream& stream, const T& data) {
    return stream.write((const char*) &data, sizeof(T));
}

template<typename T>
inline std::istream& Read(std::istream& stream, T& data) {
    return stream.read((char*) &data, sizeof(T));
}

// Little-endian.
inline std::ostream& WriteLE(std::ostream& stream, wxUint32 data) {
    return Write(stream, wxUINT32_SWAP_ON_BE(data));
}

inline std::istream& ReadLE(std::istream& stream, wxUint32& data) {
    wxUint32 tmp;
    Read(stream, tmp);
    data = wxUINT32_SWAP_ON_BE(tmp);
    return stream;
}

inline std::ostream& WriteLE(std::ostream& stream, wxInt32 data) {
    return Write(stream, wxINT32_SWAP_ON_BE(data));
}

inline std::istream& ReadLE(std::istream& stream, wxInt32& data) {
    wxInt32 tmp;
    Read(stream, tmp);
    data = wxINT32_SWAP_ON_BE(tmp);
    return stream;
}

inline std::ostream& WriteLE(std::ostream& stream, wxUint16 data) {
    return Write(stream, wxUINT16_SWAP_ON_BE(data));
}

inline std::istream& ReadLE(std::istream& stream, wxUint16& data) {
    wxUint16 tmp;
    Read(stream, tmp);
    data = wxUINT16_SWAP_ON_BE(tmp);
    return stream;
}

inline std::ostream& WriteLE(std::ostream& stream, wxInt16 data) {
    return Write(stream, wxINT16_SWAP_ON_BE(data));
}

inline std::istream& ReadLE(std::istream& stream, wxInt16& data) {
    wxInt16 tmp;
    Read(stream, tmp);
    data = wxINT16_SWAP_ON_BE(tmp);
    return stream;
}

inline std::ostream& WriteLE(std::ostream& stream, wxUint8 data) {
    return Write(stream, data);
}

inline std::istream& ReadLE(std::istream& stream, wxUint8& data) {
    return Read(stream, data);
}

inline std::ostream& WriteLE(std::ostream& stream, wxInt8 data) {
    return Write(stream, data);
}

inline std::istream& ReadLE(std::istream& stream, wxInt8& data) {
    return Read(stream, data);
}

// Big-endian.
inline std::ostream& WriteBE(std::ostream& stream, wxUint32 data) {
    return Write(stream, wxUINT32_SWAP_ON_LE(data));
}

inline std::istream& ReadBE(std::istream& stream, wxUint32& data) {
    wxUint32 tmp;
    Read(stream, tmp);
    data = wxUINT32_SWAP_ON_LE(tmp);
    return stream;
}

inline std::ostream& WriteBE(std::ostream& stream, wxInt32 data) {
    return Write(stream, wxINT32_SWAP_ON_LE(data));
}

inline std::istream& ReadBE(std::istream& stream, wxInt32& data) {
    wxInt32 tmp;
    Read(stream, tmp);
    data = wxINT32_SWAP_ON_LE(tmp);
    return stream;
}

inline std::ostream& WriteBE(std::ostream& stream, wxUint16 data) {
    return Write(stream, wxUINT16_SWAP_ON_LE(data));
}

inline std::istream& ReadBE(std::istream& stream, wxUint16& data) {
    wxUint16 tmp;
    Read(stream, tmp);
    data = wxUINT16_SWAP_ON_LE(tmp);
    return stream;
}

inline std::ostream& WriteBE(std::ostream& stream, wxInt16 data) {
    return Write(stream, wxINT16_SWAP_ON_LE(data));
}

inline std::istream& ReadBE(std::istream& stream, wxInt16& data) {
    wxInt16 tmp;
    Read(stream, tmp);
    data = wxINT16_SWAP_ON_LE(tmp);
    return stream;
}

inline std::ostream& WriteBE(std::ostream& stream, wxUint8 data) {
    return Write(stream, data);
}

inline std::istream& ReadBE(std::istream& stream, wxUint8& data) {
    return Read(stream, data);
}

inline std::ostream& WriteBE(std::ostream& stream, wxInt8 data) {
    return Write(stream, data);
}

inline std::istream& ReadBE(std::istream& stream, wxInt8& data) {
    return Read(stream, data);
}

}

#endif // !CHIPW_BINSTREAM_H_INCLUDED
