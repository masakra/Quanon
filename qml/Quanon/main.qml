import QtQuick 2.0
import QtQuick.LocalStorage 2.0
import QtMultimedia 5.0
import Processor 1.0

Rectangle {
    width: 360
    height: 360

    readonly property int ready: 0
    readonly property int audioRecord: 1
    property int status: ready

    property var db: null

    function setMode( m ) {

        status = m

        switch( m ) {
        case ready: {
            timer.stop()
            progressBar.visible = false
            progressBar.value = 0
            buttonReset.visible = false
            buttonShot.text = qsTr("Shot")
            photoPrevew.visible = false

            break;
        }
        case audioRecord: {
               progressBar.visible = true
            buttonReset.visible = true
            buttonShot.text = qsTr("Send")
            photoPrevew.visible = true
            break;
        }
        }
    }


    // отображает live поток с камеры

    VideoOutput {
        source: camera
        anchors.fill: parent
        focus: visible
    }

    Image {
        id: photoPrevew
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
    }

    Camera {
        id: camera
        imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceFlash
        captureMode: Camera.CaptureStillImage

        exposure {
            exposureCompensation: -1.0
            exposureMode: Camera.ExposurePortrait
        }

        flash.mode: Camera.FlashRedEyeReduction

        imageCapture {
            onImageCaptured: {
                photoPrevew.source = preview
                processor.processImage( preview );
            }
        }
    }

    Button {
        id: buttonShot
        text: qsTr("Shot")
        opacity: .5
        width: parent.width / 2
        height: parent.height / 4

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 30
        }

        onClicked: {
            switch ( status ) {
            case ready: {
                camera.imageCapture.capture()
                setMode( audioRecord )
                timer.start()
                break;
            }
            case audioRecord: {
                processor.send()
                setMode( ready )
            }
            }
        }
    }

    Button {
        id: buttonReset
        text: qsTr("Reset")
        opacity: .5
        width: parent.width / 2
        height: parent.height / 4

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: progressBar.top
            bottomMargin: 10
        }

        onClicked: {
            processor.stopAudioRecord();
            setMode( ready );
        }
        visible: false
    }

    ProgressBar {
        id: progressBar
        anchors {
            bottom: buttonShot.top
            horizontalCenter: parent.horizontalCenter
            bottomMargin: 10
        }
        visible: false
    }

    Rectangle {
        id: buttonConfig
        width: 54
        height: 54
        anchors.left: parent.left
        anchors.top: parent.top
        color: "black"
        radius: 4
        smooth: true
        opacity: .2
        anchors.topMargin: 4
        anchors.leftMargin: 4
        Image {
            source: "qrc:/icon-config_48x48.png"
            anchors.centerIn: parent
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                editURL.visible = true;
                editURL.text = loadIni('url');
                editURL.focus = true;
            }
        }
    }

    TextInput {
        id: editURL
        width: 200
        height: 20
        anchors.verticalCenter: buttonConfig.verticalCenter
        anchors.left: buttonConfig.right
        anchors.leftMargin: 4
        color: "black"
        visible: false
        onAccepted: {
            editURL.visible = false;
            saveIni('url', editURL.text );

        }
        Keys.onPressed: {
            if ( event.key == Qt.Key_Escape ) {
                editURL.visible = false;
            }
        }

    }


    Timer {
        id: timer
        interval: 30
        running: false
        repeat: true
        onTriggered: {
           var progress = progressBar.value
           //buttonShot.text = progress + "%"
           progressBar.value = progress + 1
           if ( progress == 100 ) {
               processor.stopAudioRecord();
               stop();

           }

           //if ( progress == 100 ) {
               //timer.stop()
           ////} else {
               //progressBar.value = progress + 1
           //}
        }
    }

    Processor {
        id: processor
    }

    /*
    Component.onCompleted: {
        openDb();
    }
    */

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


    /*
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
    */
}
