# Configuration
VERSION = 0.9.12

CONFIG += webkit clucene svg xml mini


# Base section, nothing platform specific
DEFINES += BT_MINI
DEFINES += BT_MINI_VERSION=\\\"$${VERSION}\\\"

INCLUDEPATH += . \
    ../../../src/frontend-mini \

SOURCES += \
    ../../../src/frontend-mini/models/btminimoduletextmodel.cpp \
    ../../../src/frontend-mini/models/btminimodulenavigationmodel.cpp \
    ../../../src/frontend-mini/models/btminimodelsmodel.cpp \
    ../../../src/frontend-mini/models/btminisettingsmodel.cpp \
    ../../../src/frontend-mini/ui/btministyle.cpp \
    ../../../src/frontend-mini/ui/btminipanel.cpp \
    ../../../src/frontend-mini/ui/btminimenu.cpp \
    ../../../src/frontend-mini/view/btminiview.cpp \
    ../../../src/frontend-mini/view/btminilayoutdelegate.cpp \
    ../../../src/frontend-mini/btmini.cpp


HEADERS += \
    ../../../src/frontend-mini/models/btminimoduletextmodel.h \
    ../../../src/frontend-mini/models/btminimodulenavigationmodel.h \
    ../../../src/frontend-mini/models/btminimodelsmodel.h \
    ../../../src/frontend-mini/models/btminisettingsmodel.h \
    ../../../src/frontend-mini/ui/btminipanel.h \
    ../../../src/frontend-mini/ui/btminimenu.h \
    ../../../src/frontend-mini/view/btminiview.h \
    ../../../src/frontend-mini/view/btminilayoutdelegate.h \
    ../../../src/frontend-mini/ui/btministyle.cpp


RESOURCES += ../../../src/frontend-mini/ui/btministyle.qrc \
    ../../../btmini.qrc


OTHER_FILES += ../../../src/frontend-mini/todo.txt \
    ../../../src/display-templates/Basic-Mini.tmpl


# BtMini translations
TRANSLATIONS += ../../../src/frontend-mini/translations/bibletimemini_ru.ts


# Windows platform
windows {
}

# iOS Platform
mac {
DEFINES -= BT_MINI_VERSION=\\\"$${VERSION}\\\"
DEFINES += BT_MINI_VERSION="\\\\\"$${VERSION}\\\\\""

LIBS += -framework AudioToolbox
}

# MeeGo platform
unix:contains(MEEGO_EDITION,harmattan) {

include(../../common/btmini/deployment.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog
}

# Symbian platform
symbian {
# version
DEFINES -= BT_MINI_VERSION=\\\"$${VERSION}\\\"
greaterThan(S60_VERSION, 5.0) {
DEFINES += BT_MINI_VERSION=\"$${VERSION}\"
}
else {
DEFINES += BT_MINI_VERSION=\"\\\"$${VERSION}\\\"\"
}

LIBS += -lhwrmvibraclient

# icon
ICON += btmini.svg
DEPLOYMENT += ICON

TARGET.UID3 = 0x2007174E
TARGET.CAPABILITY += NetworkServices
#TARGET.EPOCSTACKSIZE = 0x200000
TARGET.EPOCHEAPSIZE = 0x040000 0x4000000

DEPLOYMENT.display_name = BibleTime Mini

packageheader = "$${LITERAL_HASH}{\"BibleTime Mini\"}, ($${TARGET.UID3}), $$replace(VERSION, ([.]\\d+), ""), \
    $$replace(VERSION, (^\\d+.)|(.\\d+$), ""), $$replace(VERSION, (\\d+[.]), ""), TYPE=SA"

vendorinfo = \
"%{\"Crosswire\"}" \
":\"Crosswire\""

mini_deployment.pkg_prerules = packageheader vendorinfo

DEPLOYMENT += mini_deployment

include(../../common/btmini/deployment.pri)
qtcAddDeployment()
}


# Windows Mobile Platform
wince {
DEFINES += BT_STATIC_TEXT
SOURCES += ../../../src/frontend-mini/view/btstatictext.cpp
}

# BibleTime Core
include(../../common/core/core.pro)


# Platforms section
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

!symbian {
INCLUDEPATH += $${SWORD_PATH}/include/internal/regex
SOURCES += $${SWORD_PATH}/src/utilfuns/regex.c \
    $${SWORD_PATH}/src/utilfuns/ftplib.c \
}

win32 {
INCLUDEPATH += $${SWORD_PATH}/src/utilfuns/win32

SOURCES += $${SWORD_PATH}/src/utilfuns/win32/dirent.cpp

LIBS += -lws2_32
}

# iOS Platform
mac {
DEFINES -= BT_MINI_VERSION=\\\"$${VERSION}\\\"
DEFINES += BT_MINI_VERSION="\\\\\"$${VERSION}\\\\\""
}

# Android platform
android {
}

# MeeGo platform
unix {
contains(MEEGO_EDITION,harmattan) {

include(../../common/btmini/deployment.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog
}
}

# Symbian platform
symbian {
DEPLOYMENTFOLDERS = # file1 dir1

TARGET.UID3 = 0xE5723167

TARGET.CAPABILITY += NetworkServices

TARGET.EPOCSTACKSIZE = 0x14000
TARGET.EPOCHEAPSIZE = 0x020000 0x2000000

DEPLOYMENT.display_name = BibleTime Mini

packageheader = "$${LITERAL_HASH}{\"BibleTime Mini\"}, (0xE5723167), 0, 9, 1, TYPE=SA"

vendorinfo = \
"%{\"Crosswire\"}" \
":\"Crosswire\""

mini_deployment.pkg_prerules = packageheader vendorinfo

DEPLOYMENT += mini_deployment

clucene {
DEFINES += __GNUC__ NO_DUMMY_DECL
}

# following is valid for Symbian^3 build system but not for S60
DEFINES -= BT_MINI_VERSION=\\\"$${VERSION}\\\"
DEFINES += BT_MINI_VERSION=\"$${VERSION}\"

SOURCES += ftplib.c

include(../../common/btmini/deployment.pri)
qtcAddDeployment()
}

# Windows Mobile Platform
wince {
DEFINES += BT_STATIC_TEXT

SOURCES += ../../../src/frontend-mini/view/btstatictext.cpp
}
