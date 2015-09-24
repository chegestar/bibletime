TEMPLATE = app
TARGET = BibleTime-Mini
DESTDIR = ../../Debug
QT += core gui xml svg
CONFIG += debug
DEFINES += QT_LARGEFILE_SUPPORT _WINDOWS _UNICODE _WIN32 QT_NO_PRINTER QT_NO_PRINTDIALOG _CRT_SECURE_NO_WARNINGS BT_MINI _CL_DISABLE_MULTITHREADING LUCENE_DISABLE_MEMTRACKING QT_SVG_LIB QT_XML_LIB SWUSINGDLL
INCLUDEPATH += $(OutDir)/GeneratedFiles \
    . \
    ./../../../sword/include \
    ./.. \
    ./../../Debug/GeneratedFiles \
    ./../../../clucene/src \
    $(QTDIR)/src/3rdparty/harfbuzz/src
LIBS += -l../../../build/win32/sword/Debug/sword \
    -lWs2_32 \
    -l$(QTDIR)/plugins/imageformats/qsvgd4
DEPENDPATH += .
MOC_DIR += ./../../debug/GeneratedFiles
OBJECTS_DIR += debug
UI_DIR += ./../../debug/GeneratedFiles
RCC_DIR += ./../../debug/GeneratedFiles
include(BibleTime-Mini.pri)
TRANSLATIONS += ./translations/bibletimemini_ru.ts

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
    android/version.xml
