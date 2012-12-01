# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
DEPLOYMENTFOLDERS = # file1 dir1

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

DEFINES += _UCS2 _CL_DISABLE_MULTITHREADING BT_MINI

INCLUDEPATH += . \
    ../../../../bt/src/frontend-mini \
    ../../../../sword/include \
    ../../../../bt/src

SOURCES += \
    ../../../../bt/src/frontend-mini/btmini.cpp \
    ../../../../bt/src/frontend-mini/models/btminimoduletextmodel.cpp \
    ../../../../bt/src/frontend-mini/models/btminimodulenavigationmodel.cpp \
    ../../../../bt/src/frontend-mini/models/btminimodelsmodel.cpp \
    ../../../../bt/src/frontend-mini/ui/btministyle.cpp \
    ../../../../bt/src/frontend-mini/ui/btminipanel.cpp \
    ../../../../bt/src/frontend-mini/ui/btminimenu.cpp \
    ../../../../bt/src/frontend-mini/view/btminiview.cpp \
    ../../../../bt/src/frontend-mini/view/btminilayoutdelegate.cpp
HEADERS += \
    ../../../../bt/src/frontend-mini/btmini.h \
    ../../../../bt/src/frontend-mini/models/btminimoduletextmodel.h \
    ../../../../bt/src/frontend-mini/models/btminimodulenavigationmodel.h \
    ../../../../bt/src/frontend-mini/models/btminimodelsmodel.h \
    ../../../../bt/src/frontend-mini/ui/btminipanel.h \
    ../../../../bt/src/frontend-mini/ui/btminimenu.h \
    ../../../../bt/src/frontend-mini/view/btminiview.h \
    ../../../../bt/src/frontend-mini/view/btminilayoutdelegate.h

android {
LIBS += -lC:/dev/build/android/btcore/debug/libbtcore.a \
    -lC:/dev/build/android/clucene/debug/libclucene.a \
    -lC:/dev/build/android/sword/debug/libsword.a \
    -lC:/dev/qt/necessitas/android-ndk/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi/libgnustl_static.a
}

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

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
    android/version.xml

RESOURCES += \
    ../../../../bt/src/frontend-mini/ui/btministyle.qrc
