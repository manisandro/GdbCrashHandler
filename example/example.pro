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

TEMPLATE = app
TARGET = example

QT += widgets network

LIBS += \
    -L$${OUT_PWD}/../lib \
    -lGdbCrashHandler

INCLUDEPATH += \
    $${PWD}/../lib


# You can make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# Please consult the documentation of the deprecated API in order to know
# how to port your code away from it.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += \
    example.hpp

SOURCES += \
    example.cpp

RESOURCES += \
    example.qrc

# It is convenient when working in one project file system to be able to see the
# files for the other one:
DISTFILES += \
    CMakeLists.txt
