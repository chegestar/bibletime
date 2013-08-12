CONFIG += qml quick

include(../../common/btmini/common.pro)

qml {
DEFINES += BT_MINI_QML

OTHER_FILES += main.qml

RESOURCES += qml.qrc
}

OTHER_FILES += \
    metro.qml
