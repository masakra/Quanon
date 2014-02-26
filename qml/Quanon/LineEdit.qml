import QtQuick 2.0

Rectangle {
    //width: 100
    height: 20

    border.color: "gray"
    border.width: 1

    color: "#eeeefe"

    TextInput {
        id: textInput
        width: parent.width
        height: parent.height
        anchors.centerIn: parent

    }

    function setText( text ) {
        textInput.text = text
    }

    function text() {
        return textInput.text
    }

}
