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

#ifndef CHIPW_REFCOUNT_H_INCLUDED
#define CHIPW_REFCOUNT_H_INCLUDED

#ifdef CHIPW_DEBUG_REFCOUNT
#include <iostream>
#endif

#include <wx/log.h>

namespace ChipW {

class RefCounted {
public:
    RefCounted() : id(++nextid) {
#ifdef CHIPW_DEBUG_REFCOUNT
        std::cout << "Object " << id << " created." << std::endl;
#endif
    }
    virtual ~RefCounted() {
#ifdef CHIPW_DEBUG_REFCOUNT
        std::cout << "Object " << id << " destroyed." << std::endl;
#endif
    }
    void RefAdd() {if(refcount.n >= 0) ++refcount.n;
#ifdef CHIPW_DEBUG_REFCOUNT
        if(refcount.n == 1) std::cout << typeid(*this).name() << " " << id << " referenced to " << refcount.n << "." << std::endl;
#endif
    }
    static void RefAdd(RefCounted* obj) {if(obj != NULL) obj->RefAdd();}
    void RefStatic() {refcount.n = -1;}
    bool RefRemove() {if(refcount.n > 0) --refcount.n;
#ifdef CHIPW_DEBUG_REFCOUNT
//std::cout << typeid(*this).name() << " " << id << " unreferenced to " << refcount.n << "." << std::endl;
#endif
        return refcount.n == 0;}
    void RefDel() {if(RefRemove()) delete this;}
    void AssertDel() {if(RefRemove()) delete this; else wxLogError(wxT("Object had trailing references."));}
    static void RefDel(RefCounted* obj) {if(obj != NULL) obj->RefDel();}
    long GetRefCount() const {return refcount.n;}
    void SetRefCount(long nrefs) {refcount.n = nrefs;}
private:
    // This sub-object is used so that a class depending on automatic memberwise copy
    // does not copy the reference count.
    struct RefCount {
        RefCount() : n(0) { }
        RefCount(const RefCount& copy) : n(0) { }
        RefCount& operator=(const RefCount& copy) {return *this;}
        long n;
    };
    RefCount refcount;
long id;
static long nextid;
};

inline void RefAdd(RefCounted* obj) {if(obj != NULL) obj->RefAdd();}
inline void RefDel(RefCounted* obj) {if(obj != NULL) obj->RefDel();}

template<class T>
class CountedPtr {
public:
    CountedPtr(T* ob = NULL) : obj(ob) {RefAdd(obj);}
    CountedPtr(const CountedPtr& copy) : obj(copy.obj) {RefAdd(obj);}
    ~CountedPtr() {RefDel(obj);}
    CountedPtr& operator=(T* ob) {RefAdd(ob); RefDel(obj); obj = ob; return *this;}
    CountedPtr& operator=(const CountedPtr& copy) {RefAdd(copy.obj); RefDel(obj); obj = copy.obj; return *this;}
    operator T*() const {return obj;}
    T& operator*() const {return *obj;}
    T* operator->() const {return obj;}
    bool operator!() const {return !obj;}
    T* Steal() {T* ob = obj; obj = NULL; return ob;}
private:
    T* obj;
};

}

#endif // !defined(CHIPW_REFCOUNT_H_INCLUDED)
