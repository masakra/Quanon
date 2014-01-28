import QtQuick 2.0

Rectangle {
    id: page

    property Item text: dialogText

    signal closed
    signal opened

    function forceClose() {
        if ( page.opacity == 0 )
            return;
        page.closed();
        page.opacity = 0;
    }

    function show( txt ) {
        page.opened();
        dialogText.text = txt;
        page.opacity = 1;
    }

    width: dialogText.width + 20
    height: dialogText.height + 20
    color: "white"
    border.width: 1
    opacity: 0
    visible: opacity > 0
    Behavior on opacity {
        NumberAnimation { duration: 1000 }
    }

    Text { id: dialogText; anchors.centerIn: parent; text: "Hello world!" }

    MouseArea {
        anchors.fill: parent
        onClicked: forceClose();
    }

}
