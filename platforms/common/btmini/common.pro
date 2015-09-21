# Configuration
VERSION = 0.9.12

CONFIG += webkit clucene svg xml


# Base section, nothing platform specific
DEFINES += BT_MINI
DEFINES += BT_MINI_VERSION=\\\"$${VERSION}\\\"

# Sword
include(../../common/sword/sword.pro)


INCLUDEPATH += . \
    ../../../src/frontend-mini \
    ../../../src

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
    ../../../src/frontend-mini/btmini.cpp \
    ../../../src/backend/cswordmodulesearch.cpp \
    ../../../src/backend/btmoduletreeitem.cpp \
    ../../../src/backend/btinstallbackend.cpp \
    ../../../src/backend/managers/referencemanager.cpp \
    ../../../src/backend/managers/cswordbackend.cpp \
    ../../../src/backend/managers/clanguagemgr.cpp \
    ../../../src/backend/managers/cdisplaytemplatemgr.cpp \
    ../../../src/backend/managers/btstringmgr.cpp \
    ../../../src/util/directory.cpp \
    ../../../src/util/cresmgr.cpp \
    ../../../src/backend/config/btconfig.cpp \
    ../../../src/backend/config/btconfigcore.cpp \
    ../../../src/backend/bookshelfmodel/btbookshelftreemodel.cpp \
    ../../../src/backend/btinstallmgr.cpp \
    ../../../src/backend/keys/cswordversekey.cpp \
    ../../../src/backend/keys/cswordldkey.cpp \
    ../../../src/backend/keys/cswordkey.cpp \
    ../../../src/backend/drivers/cswordmoduleinfo.cpp \
    ../../../src/backend/drivers/cswordlexiconmoduleinfo.cpp \
    ../../../src/backend/drivers/cswordcommentarymoduleinfo.cpp \
    ../../../src/backend/drivers/cswordbiblemoduleinfo.cpp \
    ../../../src/backend/rendering/ctextrendering.cpp \
    ../../../src/backend/rendering/chtmlexportrendering.cpp \
    ../../../src/backend/rendering/centrydisplay.cpp \
    ../../../src/backend/rendering/cdisplayrendering.cpp \
    ../../../src/backend/bookshelfmodel/moduleitem.cpp \
    ../../../src/backend/bookshelfmodel/languageitem.cpp \
    ../../../src/backend/bookshelfmodel/item.cpp \
    ../../../src/backend/bookshelfmodel/categoryitem.cpp \
    ../../../src/frontend/cinfodisplay.cpp \
    ../../../src/backend/btinstallthread.cpp \
    ../../../src/util/tool.cpp \
    ../../../src/util/dialogutil.cpp \
    ../../../src/util/geticon.cpp \
    ../../../src/backend/rendering/cchapterdisplay.cpp \
    ../../../src/backend/rendering/cbookdisplay.cpp \
    ../../../src/backend/filters/thmltohtml.cpp \
    ../../../src/backend/filters/teitohtml.cpp \
    ../../../src/backend/filters/plaintohtml.cpp \
    ../../../src/backend/filters/osistohtml.cpp \
    ../../../src/backend/filters/gbftohtml.cpp \
    ../../../src/backend/bookshelfmodel/btbookshelfmodel.cpp \
    ../../../src/backend/filters/thmltoplain.cpp \
    ../../../src/backend/drivers/cswordbookmoduleinfo.cpp \
    ../../../src/frontend/displaywindow/btactioncollection.cpp \
    ../../../src/backend/bookshelfmodel/indexingitem.cpp \
    ../../../src/backend/keys/cswordtreekey.cpp \
    ../../../src/frontend/crossrefrendering.cpp \
    ../../../src/backend/filters/btosismorphsegmentation.cpp \
    ../../../src/bibletimeapp.cpp


HEADERS += \
    ../../../src/frontend-mini/models/btminimoduletextmodel.h \
    ../../../src/frontend-mini/models/btminimodulenavigationmodel.h \
    ../../../src/frontend-mini/models/btminimodelsmodel.h \
    ../../../src/frontend-mini/models/btminisettingsmodel.h \
    ../../../src/frontend-mini/ui/btminipanel.h \
    ../../../src/frontend-mini/ui/btminimenu.h \
    ../../../src/frontend-mini/view/btminiview.h \
    ../../../src/frontend-mini/view/btminilayoutdelegate.h \
    ../../../src/backend/cswordmodulesearch.h \
    ../../../src/backend/btmoduletreeitem.h \
    ../../../src/backend/btinstallbackend.h \
    ../../../src/backend/managers/referencemanager.h \
    ../../../src/backend/managers/cswordbackend.h \
    ../../../src/backend/managers/clanguagemgr.h \
    ../../../src/backend/managers/cdisplaytemplatemgr.h \
    ../../../src/backend/managers/btstringmgr.h \
    ../../../src/util/directory.h \
    ../../../src/util/cresmgr.h \
    ../../../src/backend/config/btconfig.h \
    ../../../src/backend/config/btconfigcore.h \
    ../../../src/backend/bookshelfmodel/btbookshelftreemodel.h \
    ../../../src/backend/btinstallmgr.h \
    ../../../src/backend/btinstallthread.h \
    ../../../src/frontend/cinfodisplay.h \
    ../../../src/frontend/displaywindow/btactioncollection.h \
    ../../../src/frontend/crossrefrendering.h \
    ../../../src/backend/keys/cswordversekey.h \
    ../../../src/backend/keys/cswordldkey.h \
    ../../../src/backend/keys/cswordkey.h \
    ../../../src/backend/keys/cswordtreekey.h \
    ../../../src/backend/drivers/cswordmoduleinfo.h \
    ../../../src/backend/drivers/cswordlexiconmoduleinfo.h \
    ../../../src/backend/drivers/cswordcommentarymoduleinfo.h \
    ../../../src/backend/drivers/cswordbiblemoduleinfo.h \
    ../../../src/backend/rendering/ctextrendering.h \
    ../../../src/backend/rendering/chtmlexportrendering.h \
    ../../../src/backend/rendering/centrydisplay.h \
    ../../../src/backend/rendering/cdisplayrendering.h \
    ../../../src/backend/rendering/cplaintextexportrendering.h \
    ../../../src/backend/bookshelfmodel/moduleitem.h \
    ../../../src/backend/bookshelfmodel/languageitem.h \
    ../../../src/backend/bookshelfmodel/item.h \
    ../../../src/backend/bookshelfmodel/categoryitem.h \
    ../../../src/util/tool.h \
    ../../../src/util/dialogutil.h \
    ../../../src/util/btsignal.h \
    ../../../src/backend/rendering/cchapterdisplay.h \
    ../../../src/backend/rendering/cbookdisplay.h \
    ../../../src/backend/filters/thmltohtml.h \
    ../../../src/backend/filters/teitohtml.h \
    ../../../src/backend/filters/plaintohtml.h \
    ../../../src/backend/filters/osistohtml.h \
    ../../../src/backend/filters/gbftohtml.h \
    ../../../src/backend/bookshelfmodel/btbookshelfmodel.h \
    ../../../src/backend/filters/thmltoplain.h \
    ../../../src/backend/drivers/cswordbookmoduleinfo.h \
    ../../../src/backend/bookshelfmodel/indexingitem.h \
    ../../../src/backend/keys/cswordtreekey.h \
    ../../../src/frontend/crossrefrendering.h \
    ../../../src/backend/filters/btosismorphsegmentation.h \
    ../../../src/frontend-mini/btmini.h \
    ../../../src/bibletimeapp.h \
    ../../../src/frontend-mini/ui/btministyle.cpp

RESOURCES += ../../../src/frontend-mini/ui/btministyle.qrc \
    ../../../btmini.qrc

OTHER_FILES += ../../../src/frontend-mini/todo.txt \
    ../../../src/display-templates/Basic-Mini.tmpl

# BtMini translations
TRANSLATIONS += ../../../src/frontend-mini/translations/bibletimemini_ru.ts

# BibleTime translations
TRANSLATIONS += ../../../i18n/messages/bibletime_ui_ru.ts



# Platforms section, can turn off/on options below
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
svg:QT += svg xml
qml {
DEFINES += BT_MINI_QML
QT += qml quick
}

# Windows platform
windows {
}

# iOS Platform
mac {
DEFINES -= BT_MINI_VERSION=\\\"$${VERSION}\\\"
DEFINES += BT_MINI_VERSION="\\\\\"$${VERSION}\\\\\""

CONFIG -= webkit

LIBS += -framework AudioToolbox
}

# Android platform
android {
greaterThan(QT_MAJOR_VERSION, 4):CONFIG -= webkit

DEFINES += STDC_HEADERS
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

CONFIG -= webkit # seems that on S60 webkit is not supported, maybe wrong packaging?
}

SOURCES += ftplib.c

LIBS += -lhwrmvibraclient


# icon
ICON += btmini.svg
DEPLOYMENT += ICON

TARGET.UID3 = 0xE5723167
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



# Options section

# WebKit
webkit {
greaterThan(QT_MAJOR_VERSION, 4) {
    QT += webkitwidgets
}
else {
    QT += webkit
}
DEFINES += BT_MINI_WEBKIT
}

# Clucene
include(../../common/clucene/clucene.pro)


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
