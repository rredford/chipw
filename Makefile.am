## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = chipw
chipw_SOURCES = src/app.cpp src/app.h src/apppaths.h src/binstream.h src/bufferscroll.cpp src/bufferscroll.h src/global.h src/help.cpp src/help.h src/levelchange.cpp src/levelchange.h src/level.cpp src/leveldataobj.cpp src/leveldataobj.h src/leveleditframe.cpp src/leveleditframe.h src/leveleditids.h src/leveleditview.cpp src/leveleditview.h src/level.h src/levelpropertiesdlg.cpp src/levelpropertiesdlg.h src/levelset.cpp src/levelsetdoc.cpp src/levelsetdoc.h src/levelset.h src/levelupdatehint.cpp src/levelupdatehint.h src/listeditdlg.cpp src/listeditdlg.h src/listmenumgr.cpp src/listmenumgr.h src/monsterlistdlg.cpp src/monsterlistdlg.h src/playtestdlg.cpp src/playtestdlg.h src/playtestmgr.cpp src/playtestmgr.h src/playtestproc.cpp src/playtestproc.h src/playtesttype.cpp src/playtesttype.h src/refcount.h src/ruleset.cpp src/ruleset.h src/sdidocframe.cpp src/sdidocframe.h src/tile.cpp src/tile.h src/tileset.cpp src/tileset.h src/valruleset.cpp src/valruleset.h src/wxtostdstream.h
nobase_dist_pkgdata_DATA = tilesets/default tilesets/tileworld-32x32.bmp tilesets/tileworld-32x32-ccedit.tis tilesets/tileworld-48x48.bmp tilesets/TW_a.bmp tilesets/TW_b.bmp tilesets/TW_m.bmp
dist_html_DATA = doc/chipw.html doc/COPYING.html

EXTRA_DIST = chipw.ico chipw.iss

AM_CXXFLAGS = $(WX_CXXFLAGS)
LDADD = $(WX_LIBS)

if !ENABLE_AUTODATADIR
AM_CXXFLAGS += -DCHIPW_DATADIR='"$(pkgdatadir)"' -DCHIPW_DOCDIR='"$(htmldir)"'
endif

if ENABLE_WINDRES
chipw_SOURCES += chipw.rc
endif

SUFFIXES = .rc
.rc.o:
	$(RESCOMP) $(DEFAULT_INCLUDES) -i '$<' -o '$@'
