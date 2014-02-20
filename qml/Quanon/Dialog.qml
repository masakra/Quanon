import QtQuick 2.0
import QtQuick.LocalStorage 2.0

Rectangle {
    id: page

    signal closed
    signal opened

    function forceClose() {
        if ( page.opacity == 0 )
            return;
        page.closed();
        page.opacity = 0;
    }

    function show() {
        page.opened();
        page.opacity = 1;
        inputName.text = loadIni("user");
        inputUrl.text = loadIni("url");
    }

    anchors.fill: parent
    color: "white"
    border.width: 1
    opacity: 0
    visible: opacity > 0
    Behavior on opacity {
        NumberAnimation { duration: 300 }
    }

    Text {
        id: textName
        anchors.bottom: parent.verticalCenter
        anchors.right: parent.horizontalCenter
        text: "Имя пользователя"
    }

    TextInput {
        id: inputName
        anchors.left: textName.right
        anchors.bottom: textName.bottom
        anchors.leftMargin: 5
        width: 200
        height: textName.height
        color: "blue"
    }

    Text {
        id: textUrl
        anchors.top: textName.bottom
        anchors.right: textName.right
        text: "URL"
    }

    TextInput {
        id: inputUrl
        anchors.left: textUrl.right
        anchors.bottom: textUrl.bottom
        anchors.leftMargin: 5
        height: textUrl.height
        width: 200
        color: "blue"
    }

//    Text { id: dialogText; anchors.centerIn: parent; text: "Hello world!" }

//    MouseArea {
//        anchors.fill: parent
//        onClicked: ;
//    }

    Button {
        id: buttonCancel
        text: qsTr("Cancel")
        width: parent.width / 3
        height: width / 3
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10
        anchors.bottomMargin: 10
        opacity: .5

        onClicked: {
            forceClose()
        }
    }

    Button {
        id: buttonOk
        text: qsTr("OK")
        width: parent.width / 3
        height: width / 3
        anchors.bottom: parent.bottom
        anchors.right: buttonCancel.left
        anchors.bottomMargin: 10
        anchors.rightMargin: 10
        opacity: .5

        onClicked: {
            saveSettings();
            forceClose();
        }
    }

    function saveSettings()
    {
        saveIni("user", inputName.text );
        saveIni("url", inputUrl.text );
    }

    function openDb()
    {
        db = LocalStorage.openDatabaseSync("quanon", "0", "", 1024, 0);
        try {
            db.transaction( function( tx ){
                tx.executeSql('CREATE TABLE IF NOT EXISTS ini ( key TEXT UNIQUE, value TEXT)');
                var table = tx.executeSql("SELECT key, value FROM ini");
                if ( table.rows.length == 0 ) {
                    tx.executeSql('INSERT INTO ini VALUES (?, ?)', ["url", "http://"]);
                    console.log('ini table created');
                }
            });
        } catch( err ) {
            console.log("Error creating table in database: " + err );
        }
    }

    function saveIni( key, value )
    {
        openDb();
        db.transaction( function( tx ){
            tx.executeSql('INSERT OR REPLACE INTO ini VALUES (?, ?)', [key, value]);
        });
    }

    function loadIni( key )
    {
        openDb();
        var res = "";
        db.transaction( function( tx ) {
            var rs = tx.executeSql('SELECT value FROM ini WHERE key = ?;', [key]);
            res = rs.rows.item( 0 ).value;
        });
        return res;
    }

}
