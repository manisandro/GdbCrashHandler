############################################################################
#    Copyright (C) 2020 Stephen Lyons <slysven@virginmedia.com             #
#                                                                          #
#    This program is free software; you can redistribute it and/or modify  #
#    it under the terms of the GNU General Public License as published by  #
#    the Free Software Foundation; either version 3 of the License, or     #
#    (at your option) any later version.                                   #
#                                                                          #
#    This program is distributed in the hope that it will be useful,       #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#    GNU General Public License for more details.                          #
#                                                                          #
#    You should have received a copy of the GNU General Public License     #
#    along with this program; if not, write to the                         #
#    Free Software Foundation, Inc.,                                       #
#    59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
############################################################################

TEMPLATE = lib
TARGET = GdbCrashHandler
VERSION = 0.3.0

QT += widgets network

CONFIG += lrelease embed_translations

# On current Debian Linux there is both a quazip {qt4} and a quazip5 {qt5} and
# we need the qt5 one:
LIBS += -lquazip5

INCLUDEPATH += .

# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Make the header file mark the symbols for export when compiling into the
# library - but for import when it is used by users - technically this is
# largely a Windows OS requirement:
DEFINES += GDBCRASHHANDLER_LIBRARY

# Pending in another PR - these ensures QStrings are either marked for
# translation with a QObject::tr(...) or QApplication:;translate(...) wrapper
# or with a QStringLiteral(...) or QLatin1String(...) wrapper if NOT.
# DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

FORMS += \
    GdbCrashHandlerDialog.ui

HEADERS += \
    GdbCrashHandler.hpp \
    GdbCrashHandlerDialog.hpp

SOURCES += \
    GdbCrashHandler.cpp \
    GdbCrashHandlerDialog.cpp

# Other languages are pending a further PR:
TRANSLATIONS += \
    locale/GdbCrashHandler_de.ts #\
#    locale/GdbCrashHandler_el.ts \
#    locale/GdbCrashHandler_es.ts \
#    locale/GdbCrashHandler_fr.ts \
#    locale/GdbCrashHandler_it.ts \
#    locale/GdbCrashHandler_nl.ts \
#    locale/GdbCrashHandler_pl.ts \
#    locale/GdbCrashHandler_pt.ts \
#    locale/GdbCrashHandler_ru.ts \
#    locale/GdbCrashHandler_tr.ts \
#    locale/GdbCrashHandler_zh_CN.ts \
#    locale/GdbCrashHandler_zh_TW.ts

# It is convenient when working in one project file system to be able to see the
# files for the other one:
DISTFILES += \
    CMakeLists.txt
