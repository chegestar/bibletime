# Configuration
VERSION = 1.0.5

CONFIG += clucene svg xml mini


# Base section, nothing platform specific
DEFINES += BT_MINI
DEFINES += BT_MINI_VERSION=\\\"$${VERSION}\\\"

INCLUDEPATH += . \
    ../../../src/frontend-mini \

SOURCES += \
    ../../../src/bibletimeapp.cpp \
    ../../../src/frontend-mini/btmini.cpp \
    ../../../src/frontend-mini/models/btminimodulenavigationmodel.cpp \
    ../../../src/frontend-mini/models/btminimodulesmodel.cpp \
    ../../../src/frontend-mini/models/btminimoduletextmodel.cpp \
    ../../../src/frontend-mini/models/btminisettingsmodel.cpp \
    ../../../src/frontend-mini/view/btminilayoutdelegate.cpp \
    ../../../src/frontend-mini/view/btminiview.cpp \
    ../../../src/frontend-mini/ui/btminimenu.cpp \
    ../../../src/frontend-mini/ui/btminipanel.cpp \
    ../../../src/frontend-mini/ui/btminiui.cpp \


HEADERS += \
    ../../../src/bibletimeapp.h \
    ../../../src/frontend-mini/btmini.h \
    ../../../src/frontend-mini/models/btminimodulesmodel.h \
    ../../../src/frontend-mini/models/btminimodulenavigationmodel.h \
    ../../../src/frontend-mini/models/btminimoduletextmodel.h \
    ../../../src/frontend-mini/models/btminisettingsmodel.h \
    ../../../src/frontend-mini/view/btminiview.h \
    ../../../src/frontend-mini/view/btminilayoutdelegate.h \
    ../../../src/frontend-mini/ui/btminimenu.h \
    ../../../src/frontend-mini/ui/btminipanel.h \
    ../../../src/frontend-mini/ui/btministyle.h \
    ../../../src/frontend-mini/ui/btminiui.h \


RESOURCES += \
    ../../../btmini.qrc \
    ../../../src/frontend-mini/ui/btministyle.qrc \


OTHER_FILES += \
    ../../../src/display-templates/Basic-Mini.tmpl \
    ../../../src/frontend-mini/todo.txt \


# BtMini translations
TRANSLATIONS += \
	../../../src/frontend-mini/translations/bibletimemini_ru.ts \


# Android platform
android {
QT += androidextras
}

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

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
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

TARGET.UID3 = 0xE5723167 # Developer UID, Store is 0x2007174E
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
}


# Windows Mobile Platform
wince {
DEFINES += BT_STATIC_TEXT
SOURCES += ../../../src/frontend-mini/view/btstatictext.cpp
}

# BlackBerry10 Platform
blackberry {
LIBS += -lbbdevice
}

# BibleTime Core
include(../../common/core/core.pro)

# show translations in project explorer
OTHER_FILES += $${TRANSLATIONS}
