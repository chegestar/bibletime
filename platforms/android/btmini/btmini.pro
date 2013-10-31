
include(../../common/btmini/btmini.pro)

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml

TARGET = bibletimemini
ANDROID_APP_NAME = BibleTime Mini
ANDROID_PACKAGE = $${ANDROID_APP_NAME}
