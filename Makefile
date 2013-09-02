#############################################################################
# Makefile for building: Orb
# Generated by qmake (1.07a) (Qt 3.3.8b) on: Tue Nov  9 23:59:06 2010
# Project:  Orb.pro
# Template: app
# Command: $(QMAKE) -o Makefile Orb.pro
#############################################################################

####### Compiler, tools and options

CC       = gcc
CXX      = g++
LEX      = flex
YACC     = yacc
CFLAGS   = -pipe -g -Wall -W -O2 -D_REENTRANT  -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED -DQT_TABLET_SUPPORT
CXXFLAGS = -pipe -g -Wall -W -O2 -D_REENTRANT  -DQT_NO_DEBUG -DQT_THREAD_SUPPORT -DQT_SHARED -DQT_TABLET_SUPPORT
LEXFLAGS = 
YACCFLAGS= -d
INCPATH  = -I/usr/share/qt3/mkspecs/default -I. -Isnappea/headers -I/usr/include/qt3 -Igui/moc/
LINK     = g++
LFLAGS   = 
LIBS     = $(SUBLIBS) -L/usr/share/qt3/lib -L/usr/X11R6/lib -L./snappea/code -lorb -lqt-mt -lXext -lX11 -lm -lpthread
AR       = ar cqs
RANLIB   = 
MOC      = /usr/share/qt3/bin/moc
UIC      = /usr/share/qt3/bin/uic
QMAKE    = qmake
TAR      = tar -cf
GZIP     = gzip -9f
COPY     = cp -f
COPY_FILE= $(COPY)
COPY_DIR = $(COPY) -r
INSTALL_FILE= $(COPY_FILE)
INSTALL_DIR = $(COPY_DIR)
DEL_FILE = rm -f
SYMLINK  = ln -sf
DEL_DIR  = rmdir
MOVE     = mv -f
CHK_DIR_EXISTS= test -d
MKDIR    = mkdir -p

####### Output directory

OBJECTS_DIR = gui/obj/

####### Files

HEADERS = gui/manifold_interface.h \
		gui/console.h \
		gui/diagram_canvas.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/list_interface.h \
		gui/cover_interface.h \
		gui/dehn_interface.h \
		gui/casson.h \
		gui/color.h \
		gui/organizer.h
SOURCES = gui/manifold_interface.cpp \
		gui/console.cpp \
		gui/organizer.cpp \
		gui/general_position.cpp \
		gui/interface.cpp \
		gui/edge.cpp \
		gui/diagram_canvas.cpp \
		gui/diagram_window.cpp \
		gui/clip_board.cpp \
		gui/isometry_window.cpp \
		gui/misc_functions.cpp \
		gui/cover_interface.cpp \
		gui/dehn_interface.cpp \
		gui/list_interface.cpp \
		gui/graph_complement.c \
		gui/graph2manifold.c \
		gui/main.cpp
OBJECTS = gui/obj/manifold_interface.o \
		gui/obj/console.o \
		gui/obj/organizer.o \
		gui/obj/general_position.o \
		gui/obj/interface.o \
		gui/obj/edge.o \
		gui/obj/diagram_canvas.o \
		gui/obj/diagram_window.o \
		gui/obj/clip_board.o \
		gui/obj/isometry_window.o \
		gui/obj/misc_functions.o \
		gui/obj/cover_interface.o \
		gui/obj/dehn_interface.o \
		gui/obj/list_interface.o \
		gui/obj/graph_complement.o \
		gui/obj/graph2manifold.o \
		gui/obj/main.o
FORMS = 
UICDECLS = 
UICIMPLS = 
SRCMOC   = gui/moc/moc_manifold_interface.cpp \
		gui/moc/moc_console.cpp \
		gui/moc/moc_diagram_canvas.cpp \
		gui/moc/moc_diagram_window.cpp \
		gui/moc/moc_clip_board.cpp \
		gui/moc/moc_isometry_window.cpp \
		gui/moc/moc_list_interface.cpp \
		gui/moc/moc_cover_interface.cpp \
		gui/moc/moc_dehn_interface.cpp \
		gui/moc/moc_organizer.cpp
OBJMOC = gui/obj/moc_manifold_interface.o \
		gui/obj/moc_console.o \
		gui/obj/moc_diagram_canvas.o \
		gui/obj/moc_diagram_window.o \
		gui/obj/moc_clip_board.o \
		gui/obj/moc_isometry_window.o \
		gui/obj/moc_list_interface.o \
		gui/obj/moc_cover_interface.o \
		gui/obj/moc_dehn_interface.o \
		gui/obj/moc_organizer.o
DIST	   = Orb.pro
QMAKE_TARGET = Orb
DESTDIR  = 
TARGET   = Orb

first: all
####### Implicit rules

.SUFFIXES: .c .o .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(UICDECLS) $(OBJECTS) $(OBJMOC)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(OBJCOMP) $(LIBS)

mocables: $(SRCMOC)
uicables: $(UICDECLS) $(UICIMPLS)

$(MOC): 
	( cd $(QTDIR)/src/moc && $(MAKE) )

Makefile: Orb.pro  /usr/share/qt3/mkspecs/default/qmake.conf /usr/share/qt3/lib/libqt-mt.prl
	$(QMAKE) -o Makefile Orb.pro
qmake: 
	@$(QMAKE) -o Makefile Orb.pro

dist: 
	@mkdir -p gui/obj/Orb && $(COPY_FILE) --parents $(SOURCES) $(HEADERS) $(FORMS) $(DIST) gui/obj/Orb/ && ( cd `dirname gui/obj/Orb` && $(TAR) Orb.tar Orb && $(GZIP) Orb.tar ) && $(MOVE) `dirname gui/obj/Orb`/Orb.tar.gz . && $(DEL_FILE) -r gui/obj/Orb

mocclean:
	-$(DEL_FILE) $(OBJMOC)
	-$(DEL_FILE) $(SRCMOC)

uiclean:

yaccclean:
lexclean:
clean: mocclean
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) $(TARGET)


FORCE:

####### Compile

gui/obj/manifold_interface.o: gui/manifold_interface.cpp gui/manifold_interface.h \
		gui/cover_interface.h \
		gui/dehn_interface.h \
		gui/list_interface.h \
		gui/color.h \
		gui/console.h \
		gui/diagram_canvas.h \
		gui/orb32.xpm \
		gui/view.xpm \
		gui/disk.xpm \
		gui/action.xpm \
		gui/draw.xpm \
		gui/diagram_window.h \
		gui/graph_complement.h \
		gui/organizer.h \
		gui/clip_board.h \
		gui/isometry_window.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/manifold_interface.o gui/manifold_interface.cpp

gui/obj/console.o: gui/console.cpp gui/console.h \
		gui/color.h \
		gui/zoomin.xpm \
		gui/zoomout.xpm \
		gui/clear.xpm
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/console.o gui/console.cpp

gui/obj/organizer.o: gui/organizer.cpp gui/organizer.h \
		gui/casson.h \
		gui/clip.xpm \
		gui/compare.xpm \
		gui/draw.xpm \
		gui/import.xpm \
		gui/help.xpm \
		gui/open.xpm \
		gui/orb32.xpm \
		gui/switch.xpm \
		gui/quit.xpm \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/organizer.o gui/organizer.cpp

gui/obj/general_position.o: gui/general_position.cpp gui/diagram_canvas.h \
		gui/graph_complement.h \
		gui/color.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/general_position.o gui/general_position.cpp

gui/obj/interface.o: gui/interface.cpp gui/diagram_canvas.h \
		gui/graph_complement.h \
		gui/color.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/interface.o gui/interface.cpp

gui/obj/edge.o: gui/edge.cpp gui/diagram_canvas.h \
		gui/graph_complement.h \
		gui/color.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/edge.o gui/edge.cpp

gui/obj/diagram_canvas.o: gui/diagram_canvas.cpp gui/diagram_canvas.h \
		gui/graph_complement.h \
		gui/color.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/diagram_canvas.o gui/diagram_canvas.cpp

gui/obj/diagram_window.o: gui/diagram_window.cpp gui/organizer.h \
		gui/diagram_window.h \
		gui/drill.xpm \
		gui/clear.xpm \
		gui/draw.xpm \
		gui/examine.xpm \
		gui/manifold_interface.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/diagram_window.o gui/diagram_window.cpp

gui/obj/clip_board.o: gui/clip_board.cpp gui/clip_board.h \
		gui/organizer.h \
		gui/color.h \
		gui/examine.xpm \
		gui/clip.xpm \
		gui/disk.xpm \
		gui/zoomin.xpm \
		gui/clear.xpm \
		gui/zoomout.xpm \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/isometry_window.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/clip_board.o gui/clip_board.cpp

gui/obj/isometry_window.o: gui/isometry_window.cpp gui/isometry_window.h \
		gui/color.h \
		gui/compare.xpm \
		gui/organizer.h \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/isometry_window.o gui/isometry_window.cpp

gui/obj/misc_functions.o: gui/misc_functions.cpp gui/diagram_canvas.h \
		gui/graph_complement.h \
		gui/color.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/misc_functions.o gui/misc_functions.cpp

gui/obj/cover_interface.o: gui/cover_interface.cpp gui/cover_interface.h \
		gui/organizer.h \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/cover_interface.o gui/cover_interface.cpp

gui/obj/dehn_interface.o: gui/dehn_interface.cpp gui/dehn_interface.h \
		gui/organizer.h \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/dehn_interface.o gui/dehn_interface.cpp

gui/obj/list_interface.o: gui/list_interface.cpp gui/list_interface.h \
		gui/organizer.h \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/list_interface.o gui/list_interface.cpp

gui/obj/graph_complement.o: gui/graph_complement.c gui/graph_complement.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o gui/obj/graph_complement.o gui/graph_complement.c

gui/obj/graph2manifold.o: gui/graph2manifold.c gui/graph_complement.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o gui/obj/graph2manifold.o gui/graph2manifold.c

gui/obj/main.o: gui/main.cpp gui/organizer.h \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/main.o gui/main.cpp

gui/obj/moc_manifold_interface.o: gui/moc/moc_manifold_interface.cpp  gui/manifold_interface.h gui/diagram_window.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h \
		gui/color.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_manifold_interface.o gui/moc/moc_manifold_interface.cpp

gui/obj/moc_console.o: gui/moc/moc_console.cpp  gui/console.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_console.o gui/moc/moc_console.cpp

gui/obj/moc_diagram_canvas.o: gui/moc/moc_diagram_canvas.cpp  gui/diagram_canvas.h gui/graph_complement.h \
		gui/color.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_diagram_canvas.o gui/moc/moc_diagram_canvas.cpp

gui/obj/moc_diagram_window.o: gui/moc/moc_diagram_window.cpp  gui/diagram_window.h gui/diagram_canvas.h \
		gui/graph_complement.h \
		gui/color.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_diagram_window.o gui/moc/moc_diagram_window.cpp

gui/obj/moc_clip_board.o: gui/moc/moc_clip_board.cpp  gui/clip_board.h 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_clip_board.o gui/moc/moc_clip_board.cpp

gui/obj/moc_isometry_window.o: gui/moc/moc_isometry_window.cpp  gui/isometry_window.h gui/organizer.h \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_isometry_window.o gui/moc/moc_isometry_window.cpp

gui/obj/moc_list_interface.o: gui/moc/moc_list_interface.cpp  gui/list_interface.h gui/organizer.h \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_list_interface.o gui/moc/moc_list_interface.cpp

gui/obj/moc_cover_interface.o: gui/moc/moc_cover_interface.cpp  gui/cover_interface.h gui/organizer.h \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_cover_interface.o gui/moc/moc_cover_interface.cpp

gui/obj/moc_dehn_interface.o: gui/moc/moc_dehn_interface.cpp  gui/dehn_interface.h gui/organizer.h \
		gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_dehn_interface.o gui/moc/moc_dehn_interface.cpp

gui/obj/moc_organizer.o: gui/moc/moc_organizer.cpp  gui/organizer.h gui/manifold_interface.h \
		gui/diagram_window.h \
		gui/clip_board.h \
		gui/isometry_window.h \
		gui/color.h \
		gui/diagram_canvas.h \
		gui/graph_complement.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o gui/obj/moc_organizer.o gui/moc/moc_organizer.cpp

gui/moc/moc_manifold_interface.cpp: $(MOC) gui/manifold_interface.h
	$(MOC) gui/manifold_interface.h -o gui/moc/moc_manifold_interface.cpp

gui/moc/moc_console.cpp: $(MOC) gui/console.h
	$(MOC) gui/console.h -o gui/moc/moc_console.cpp

gui/moc/moc_diagram_canvas.cpp: $(MOC) gui/diagram_canvas.h
	$(MOC) gui/diagram_canvas.h -o gui/moc/moc_diagram_canvas.cpp

gui/moc/moc_diagram_window.cpp: $(MOC) gui/diagram_window.h
	$(MOC) gui/diagram_window.h -o gui/moc/moc_diagram_window.cpp

gui/moc/moc_clip_board.cpp: $(MOC) gui/clip_board.h
	$(MOC) gui/clip_board.h -o gui/moc/moc_clip_board.cpp

gui/moc/moc_isometry_window.cpp: $(MOC) gui/isometry_window.h
	$(MOC) gui/isometry_window.h -o gui/moc/moc_isometry_window.cpp

gui/moc/moc_list_interface.cpp: $(MOC) gui/list_interface.h
	$(MOC) gui/list_interface.h -o gui/moc/moc_list_interface.cpp

gui/moc/moc_cover_interface.cpp: $(MOC) gui/cover_interface.h
	$(MOC) gui/cover_interface.h -o gui/moc/moc_cover_interface.cpp

gui/moc/moc_dehn_interface.cpp: $(MOC) gui/dehn_interface.h
	$(MOC) gui/dehn_interface.h -o gui/moc/moc_dehn_interface.cpp

gui/moc/moc_organizer.cpp: $(MOC) gui/organizer.h
	$(MOC) gui/organizer.h -o gui/moc/moc_organizer.cpp

####### Install

install:  

uninstall:  

