import QtQuick 2.0

Rectangle {
    id: rootFrame
    width: 360
    height: 360
    state: "normal"

    Column {
        anchors.fill: parent

        spacing: 5

        Rectangle {
            width: 360
            height: 50

            Item {
                width: 180
                height: 50

                Text {
                    anchors.centerIn: parent
                    text: "Module"
                    font.pixelSize: 20
                }

                MouseArea {
                    id: modulesButtonArea
                    anchors.fill: parent
                    onClicked: {
                        rootFrame.state == "normal" ? rootFrame.state = "raise" : rootFrame.state = "normal"
                    }
                }
            }

            Item {
                width: 180
                height: 50
                x: 180

                Text {
                    anchors.centerIn: parent
                    text: "Place"
                    font.pixelSize: 20
                }

                MouseArea {
                    id: placeButtonArea
                    anchors.fill: parent
                    onClicked: {
                        rootFrame.state == "normal" ? rootFrame.state = "raise" : rootFrame.state = "normal"
                    }
                }
            }
        }

        Rectangle {
            width: 360
            height: 310

            ListView {
                clip: true
                anchors.fill: parent

                model: ListModel {
                    ListElement {
                        name: "Grey"
                        colorCode: "grey"
                    }

                    ListElement {
                        name: "Red"
                        colorCode: "red"
                    }

                    ListElement {
                        name: "Blue"
                        colorCode: "blue"
                    }

                    ListElement {
                        name: "Green"
                        colorCode: "green"
                    }
                }
                delegate: Item {
                    x: 5
                    height: 40
                    Row {
                        id: row1
                        Rectangle {
                            width: 40
                            height: 40
                            color: colorCode
                        }

                        Text {
                            text: name
                            anchors.verticalCenter: parent.verticalCenter
                            font.bold: true
                        }
                        spacing: 10
                    }
                }
            }
        }
    }

    Rectangle {
        id: rectangle1
        width: rootFrame.width
        height: 275
        y:-275

        ListView {
            clip: true
            anchors.fill: parent

            model: navigationModel

            delegate: Rectangle {
                height: 25
                width: 360

                Text {
                    text: display
                    font.pixelSize: 20
                }
            }
        }
    }

    states: [
        State {
            name: "normal"
            PropertyChanges { target: rootFrame; y: 0; }
        },
        State {
            name: "raise"
            PropertyChanges { target: rootFrame; y: 275; }
        }
    ]

    transitions: [
        Transition {
            from: "*"; to: "raise"
            NumberAnimation {
                properties: "x,y"; easing.type: Easing.OutBounce; duration: 900
            }
        },
        Transition {
            from: "*"; to: "normal"
            NumberAnimation {
                properties: "x,y"; easing.type: Easing.OutQuart; duration: 400
            }
        }
     ]
}
