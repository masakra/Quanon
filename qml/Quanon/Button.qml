import QtQuick 2.0

Rectangle {
    id: container

    width: 100
    height: 62

    property string text: "Shot"
    signal clicked

    radius: 4
    smooth: true
    border.width: 1

    gradient: Gradient {
        GradientStop {
            position: 0.0
            //color: !mouseArea.pressed ? activePalette.light : activePalette.button
            color: ! mouseArea.pressed ? activePalette.button : activePalette.light
        }
        GradientStop {
            position: 1.0
//            color: !mouseArea.pressed ? activePalette.button : activePalette.dark
            color: !mouseArea.pressed ? "black" : activePalette.dark
        }
    }

    SystemPalette { id: activePalette }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: container.clicked()
    }

    Text {
        id: text
        anchors.centerIn: parent
        font.pointSize: 10
        text: parent.text
        color: activePalette.buttonText
    }
}
