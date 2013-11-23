import QtQuick 2.0

Item {
    id: progressbar

    property int minimum: 0
    property int maximum: 100
    property int value: 0
    property color color: "#77B753"

    width: 250
    height: 23

    clip: true

    Rectangle {
        id: border
        anchors.fill: parent
        anchors.bottomMargin: 1
        anchors.rightMargin: 1
        color: "transparent"
        border.width: 1
        border.color: parent.color
    }

    Rectangle {
        id: highlight
        property int widthDest: ((progressbar.width * (progressbar.value-progressbar.minimum))/(progressbar.maximum-progressbar.minimum)-8)
        width: highlight.widthDest
        opacity: .7

        Behavior on width {
            SmoothedAnimation {
                velocity: 1200
            }
        }

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            topMargin: 2
            rightMargin: 2
            bottomMargin: 3
            leftMargin: 2
        }

        color: parent.color
    }

    Text {
        text: qsTr("Audio recording...")
        anchors.centerIn: parent
        color: "#faff00"
    }
}
