#-------------------------------------------------
#
# Project created by QtCreator 2012-10-20T23:25:29
#
#-------------------------------------------------

QT += gui

TARGET = btcore
TEMPLATE = lib
CONFIG += staticlib

DEFINES += BT_MINI

INCLUDEPATH += . \
    ../../../../bt/src \
    ../../../../sword/include \
    ../../../../clucene/src \
    ../../clucene/clucene

SOURCES += \
    ../../../../bt/src/backend/cswordmodulesearch.cpp \
    ../../../../bt/src/backend/btmoduletreeitem.cpp \
    ../../../../bt/src/backend/btinstallbackend.cpp \
    ../../../../bt/src/backend/managers/referencemanager.cpp \
    ../../../../bt/src/backend/managers/cswordbackend.cpp \
    ../../../../bt/src/backend/managers/clanguagemgr.cpp \
    ../../../../bt/src/backend/managers/cdisplaytemplatemgr.cpp \
    ../../../../bt/src/backend/managers/btstringmgr.cpp \
    ../../../../bt/src/util/directory.cpp \
    ../../../../bt/src/util/cresmgr.cpp \
    ../../../../bt/src/backend/config/cbtconfig.cpp \
    ../../../../bt/src/backend/bookshelfmodel/btbookshelftreemodel.cpp \
    ../../../../bt/src/frontend/bookshelfmanager/btinstallmgr.cpp \
    ../../../../bt/src/backend/keys/cswordversekey.cpp \
    ../../../../bt/src/backend/keys/cswordldkey.cpp \
    ../../../../bt/src/backend/keys/cswordkey.cpp \
    ../../../../bt/src/backend/drivers/cswordmoduleinfo.cpp \
    ../../../../bt/src/backend/drivers/cswordlexiconmoduleinfo.cpp \
    ../../../../bt/src/backend/drivers/cswordcommentarymoduleinfo.cpp \
    ../../../../bt/src/backend/drivers/cswordbiblemoduleinfo.cpp \
    ../../../../bt/src/backend/rendering/ctextrendering.cpp \
    ../../../../bt/src/backend/rendering/chtmlexportrendering.cpp \
    ../../../../bt/src/backend/rendering/centrydisplay.cpp \
    ../../../../bt/src/backend/rendering/cdisplayrendering.cpp \
    ../../../../bt/src/backend/bookshelfmodel/moduleitem.cpp \
    ../../../../bt/src/backend/bookshelfmodel/languageitem.cpp \
    ../../../../bt/src/backend/bookshelfmodel/item.cpp \
    ../../../../bt/src/backend/bookshelfmodel/categoryitem.cpp \
    ../../../../bt/src/frontend/cinfodisplay.cpp \
    ../../../../bt/src/util/tool.cpp \
    ../../../../bt/src/util/dialogutil.cpp \
    ../../../../bt/src/backend/rendering/cchapterdisplay.cpp \
    ../../../../bt/src/backend/rendering/cbookdisplay.cpp \
    ../../../../bt/src/backend/filters/thmltohtml.cpp \
    ../../../../bt/src/backend/filters/teitohtml.cpp \
    ../../../../bt/src/backend/filters/plaintohtml.cpp \
    ../../../../bt/src/backend/filters/osistohtml.cpp \
    ../../../../bt/src/backend/filters/gbftohtml.cpp \
    ../../../../bt/src/backend/bookshelfmodel/btbookshelfmodel.cpp \
    ../../../../bt/src/backend/filters/thmltoplain.cpp \
    ../../../../bt/src/backend/drivers/cswordbookmoduleinfo.cpp \
    ../../../../bt/src/frontend/displaywindow/btactioncollection.cpp \
    ../../../../bt/src/backend/bookshelfmodel/indexingitem.cpp \
    ../../../../bt/src/backend/keys/cswordtreekey.cpp \
    ../../../../bt/src/frontend/crossrefrendering.cpp \
    ../../../../bt/src/backend/filters/osismorphsegmentation.cpp

HEADERS += \
    ../../../../bt/src/backend/cswordmodulesearch.h \
    ../../../../bt/src/backend/btmoduletreeitem.h \
    ../../../../bt/src/backend/btinstallbackend.h \
    ../../../../bt/src/backend/managers/referencemanager.h \
    ../../../../bt/src/backend/managers/cswordbackend.h \
    ../../../../bt/src/backend/managers/clanguagemgr.h \
    ../../../../bt/src/backend/managers/cdisplaytemplatemgr.h \
    ../../../../bt/src/backend/managers/btstringmgr.h \
    ../../../../bt/src/util/directory.h \
    ../../../../bt/src/util/cresmgr.h \
    ../../../../bt/src/backend/config/cbtconfig.h \
    ../../../../bt/src/backend/bookshelfmodel/btbookshelftreemodel.h \
    ../../../../bt/src/frontend/bookshelfmanager/btinstallmgr.h \
    ../../../../bt/src/frontend/bookshelfmanager/installpage/btinstallthread.h \
    ../../../../bt/src/frontend/cinfodisplay.h \
    ../../../../bt/src/frontend/displaywindow/btactioncollection.h \
    ../../../../bt/src/frontend/crossrefrendering.h \
    ../../../../bt/src/backend/keys/cswordversekey.h \
    ../../../../bt/src/backend/keys/cswordldkey.h \
    ../../../../bt/src/backend/keys/cswordkey.h \
    ../../../../bt/src/backend/keys/cswordtreekey.h \
    ../../../../bt/src/backend/drivers/cswordmoduleinfo.h \
    ../../../../bt/src/backend/drivers/cswordlexiconmoduleinfo.h \
    ../../../../bt/src/backend/drivers/cswordcommentarymoduleinfo.h \
    ../../../../bt/src/backend/drivers/cswordbiblemoduleinfo.h \
    ../../../../bt/src/backend/rendering/ctextrendering.h \
    ../../../../bt/src/backend/rendering/chtmlexportrendering.h \
    ../../../../bt/src/backend/rendering/centrydisplay.h \
    ../../../../bt/src/backend/rendering/cdisplayrendering.h \
    ../../../../bt/src/backend/rendering/cplaintextexportrendering.h \
    ../../../../bt/src/backend/bookshelfmodel/moduleitem.h \
    ../../../../bt/src/backend/bookshelfmodel/languageitem.h \
    ../../../../bt/src/backend/bookshelfmodel/item.h \
    ../../../../bt/src/backend/bookshelfmodel/categoryitem.h \
    ../../../../bt/src/util/tool.h \
    ../../../../bt/src/util/dialogutil.h \
    ../../../../bt/src/util/btsignal.h \
    ../../../../bt/src/backend/rendering/cchapterdisplay.h \
    ../../../../bt/src/backend/rendering/cbookdisplay.h \
    ../../../../bt/src/backend/filters/thmltohtml.h \
    ../../../../bt/src/backend/filters/teitohtml.h \
    ../../../../bt/src/backend/filters/plaintohtml.h \
    ../../../../bt/src/backend/filters/osistohtml.h \
    ../../../../bt/src/backend/filters/gbftohtml.h \
    ../../../../bt/src/backend/bookshelfmodel/btbookshelfmodel.h \
    ../../../../bt/src/backend/filters/thmltoplain.h \
    ../../../../bt/src/backend/drivers/cswordbookmoduleinfo.h \
    ../../../../bt/src/backend/bookshelfmodel/indexingitem.h \
    ../../../../bt/src/backend/keys/cswordtreekey.h \
    ../../../../bt/src/frontend/crossrefrendering.h \
    ../../../../bt/src/backend/filters/osismorphsegmentation.h
	

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/version.xml \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/version.xml \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/version.xml
