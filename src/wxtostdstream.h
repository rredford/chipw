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

#ifndef CHIPW_WXTOSTDSTREAM_H_INCLUDED
#define CHIPW_WXTOSTDSTREAM_H_INCLUDED

#include <wx/stream.h>
#include <streambuf>
#include <ostream>
#include <istream>

namespace ChipW {

template<typename charT, typename traits = std::char_traits<charT> >
class basic_wxstreambuf : public std::basic_streambuf<charT, traits> {
public:
    typedef charT char_type;
    typedef traits traits_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    explicit basic_wxstreambuf(wxOutputStream* out = NULL, wxInputStream* in = NULL)
        : wxout(out), wxin(in), inputbuffer(traits::eof()) { }
    explicit basic_wxstreambuf(wxInputStream* in)
        : wxout(NULL), wxin(in), inputbuffer(traits::eof()) { }
    wxOutputStream* GetWXOut() const {return wxout;}
    wxInputStream* GetWXIn() const {return wxin;}
    void SetWXOut(wxOutputStream* out) {wxout = out;}
    void SetWXIn(wxInputStream* in) {wxin = in;}
protected:
    virtual basic_wxstreambuf* setbuf(char_type* s, std::streamsize n) {return NULL;}
    virtual int_type underflow();
    virtual int_type uflow();
    virtual int_type overflow(int_type c = traits::eof());
    virtual std::streamsize xsgetn(char_type* s, std::streamsize n);
    virtual std::streamsize xsputn(const char_type* s, std::streamsize n);
private:
    wxOutputStream* wxout;
    wxInputStream* wxin;
    int_type inputbuffer; // Needed for underflow().
};

typedef basic_wxstreambuf<char> wxstreambuf;

template<typename charT, typename traits = std::char_traits<charT> >
class basic_owxstream : public std::basic_ostream<charT, traits> {
public:
    explicit basic_owxstream(wxOutputStream* out = NULL) : std::basic_ostream<charT, traits>(&buf), buf(out) { }
    wxOutputStream* GetWXOut() const {return buf.GetWXOut();}
    void SetWXOut(wxOutputStream* out) {buf.SetWXOut(out);}
private:
    basic_wxstreambuf<charT, traits> buf;
};

typedef basic_owxstream<char> owxstream;

template<typename charT, typename traits = std::char_traits<charT> >
class basic_iwxstream : public std::basic_istream<charT, traits> {
public:
    explicit basic_iwxstream(wxInputStream* in = NULL) : std::basic_istream<charT, traits>(&buf), buf(in) { }
    wxInputStream* GetWXIn() const {return buf.GetWXIn();}
    void SetWXIn(wxInputStream* in) {buf.SetWXIn(in);}
private:
    basic_wxstreambuf<charT, traits> buf;
};

typedef basic_iwxstream<char> iwxstream;

template<typename charT, typename traits = std::char_traits<charT> >
class basic_wxstream : public std::basic_iostream<charT, traits> {
public:
    explicit basic_wxstream(wxOutputStream* out = NULL, wxInputStream* in = NULL) : std::basic_iostream<charT, traits>(&buf), buf(out, in) { }
    explicit basic_wxstream(wxInputStream* in) : std::basic_iostream<charT, traits>(&buf), buf(in) { }
    wxOutputStream* GetWXOut() const {return buf.GetWXOut();}
    void SetWXOut(wxOutputStream* out) {buf.SetWXOut(out);}
    wxInputStream* GetWXIn() const {return buf.GetWXIn();}
    void SetWXIn(wxInputStream* in) {buf.SetWXIn(in);}
private:
    basic_wxstreambuf<charT, traits> buf;
};

typedef basic_wxstream<char> wxstream;

template<typename charT, typename traits>
typename basic_wxstreambuf<charT, traits>::int_type basic_wxstreambuf<charT, traits>::underflow() {
    if(inputbuffer == traits::eof()) {
        if(wxin == NULL || !wxin->IsOk() || wxin->Eof())
            return traits::eof();
        char_type buf;
        wxin->Read(&buf, sizeof(char_type));
        if(wxin->LastRead() < sizeof(char_type))
            return traits::eof();
        inputbuffer = buf;
    }
    return inputbuffer;
}

template<typename charT, typename traits>
typename basic_wxstreambuf<charT, traits>::int_type basic_wxstreambuf<charT, traits>::uflow() {
    int_type result = underflow();
    inputbuffer = traits::eof();
    return result;
}

template<typename charT, typename traits>
typename basic_wxstreambuf<charT, traits>::int_type basic_wxstreambuf<charT, traits>::overflow(basic_wxstreambuf<charT, traits>::int_type c) {
    if(c == traits::eof())
        return 0;
    if(wxout == NULL || !wxout->IsOk())
        return traits::eof();
    char_type buf = c;
    wxout->Write(&buf, sizeof(char_type));
    if(wxout->LastWrite() < sizeof(char_type))
        return traits::eof();
    return 0;
}

template<typename charT, typename traits>
std::streamsize basic_wxstreambuf<charT, traits>::xsgetn(basic_wxstreambuf<charT, traits>::char_type* s, std::streamsize n) {
    if(s == NULL || n < 1)
        return 0;
    std::streamsize result = 0;
    if(inputbuffer != traits::eof()) {
        *s = inputbuffer;
        inputbuffer = traits::eof();
        ++s;
        --n;
        ++result;
    }
    if(wxin == NULL || !wxin->IsOk() || wxin->Eof())
        return result;
    wxin->Read(s, n * sizeof(char_type));
    result += wxin->LastRead() / sizeof(char_type);
    return result;
}

template<typename charT, typename traits>
std::streamsize basic_wxstreambuf<charT, traits>::xsputn(const basic_wxstreambuf<charT, traits>::char_type* s, std::streamsize n) {
    if(s == NULL || wxout == NULL || !wxout->IsOk())
        return 0;
    wxout->Write(s, n * sizeof(char_type));
    return wxout->LastWrite() / sizeof(char_type);
}

}


#endif // !CHIPW_WXTOSTDSTREAM_H_INCLUDED
