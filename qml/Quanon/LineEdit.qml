import QtQuick 2.0

Rectangle {
    //width: 100
    height: textInput.contentHeight

    border.color: "gray"
    border.width: 1

    color: "#eeeefe"

    TextInput {
        id: textInput
        width: parent.width - 8
        height: parent.height
        anchors.centerIn: parent
        font.pointSize: 14
        focus: true
    }

    function setText( text ) {
        textInput.text = text
    }

    function text() {
        return textInput.text
    }

}
