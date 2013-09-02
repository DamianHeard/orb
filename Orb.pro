SOURCES = ./gui/manifold_interface.cpp \
		./gui/console.cpp \
		./gui/organizer.cpp \
		./gui/general_position.cpp \
		./gui/interface.cpp \
		./gui/edge.cpp \
		./gui/diagram_canvas.cpp \
		./gui/diagram_window.cpp \
		./gui/clip_board.cpp \
		./gui/isometry_window.cpp \
		./gui/misc_functions.cpp \
		./gui/cover_interface.cpp \
		./gui/dehn_interface.cpp \
		./gui/list_interface.cpp \
		./gui/graph_complement.c \
		./gui/graph2manifold.c \
		./gui/main.cpp
HEADERS = ./gui/manifold_interface.h \
		./gui/console.h \
		./gui/diagram_canvas.h \
		./gui/diagram_window.h \
		./gui/clip_board.h \
		./gui/isometry_window.h \
		./gui/list_interface.h \
		./gui/cover_interface.h \
		./gui/dehn_interface.h \
		./gui/casson.h \
		./gui/color.h \
		./gui/organizer.h
INCLUDEPATH += ./snappea/headers
CONFIG += qt warn_on release
LIBS += -L./snappea/code -lorb -lm
RC_FILE = Orb.icns
MOC_DIR = ./gui/moc
OBJECTS_DIR = ./gui/obj
