
CONFIG += windeployqt curl
DEFINES += WINPHONE

include(../../common/btmini/btmini.pro)

WINRT_MANIFEST.description = Simple and convinient app for daily Bible reading
WINRT_MANIFEST.name = BibleTime Mini
WINRT_MANIFEST.publisher = BibleTime Team
WINRT_MANIFEST.identity = 4c262b59-1392-402d-932c-7e07add4488c
WINRT_MANIFEST.version = $${VERSION}.0
