import QtQuick 2.0
import QtMultimedia 5.0
import Processor 1.0

Rectangle {
    width: 360
    height: 360

    readonly property int ready: 0
    readonly property int audioRecord: 1
    readonly property int locked: 2
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
            buttonShot.visible = true
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
        case locked: {
            buttonShot.visible = false
            buttonShot.text = qsTr("Locked...")
            break
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
                processor.save( dlgConfig.loadIni("user") );
                processor.send( dlgConfig.loadIni("url") );
                timer.stop();
                //setMode( ready )
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

    ToolButton {
        id: buttonConfig
        anchors.left: parent.left
        anchors.top: parent.top
        Image {
            source: "qrc:/icon-config_48x48.png"
            anchors.centerIn: parent
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                dlgConfig.show();
            }
        }
    }

    ToolButton {
        id: buttonExit
        anchors.right: parent.right
        anchors.top: parent.top
        Image {
            source: "qrc:/exit_48x48.png"
            anchors.centerIn: parent
        }
        MouseArea {
            anchors.fill: parent
            onClicked: {
                Qt.quit();
            }
        }

    }

    Dialog {
        id: dlgConfig
        anchors.centerIn: parent
    }

//    TextInput {
//        id: editURL
//        width: 200
//        height: 20
//        anchors.verticalCenter: buttonConfig.verticalCenter
//        anchors.left: buttonConfig.right
//        anchors.leftMargin: 4
//        color: "black"
//        visible: false
//        onAccepted: {
//            editURL.visible = false;
//            saveIni('url', editURL.text );

//        }
//        Keys.onPressed: {
//            if ( event.key == Qt.Key_Escape ) {
//                editURL.visible = false;
//            }
//        }

//    }


    Timer {
        id: timer
        interval: 60
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

    Text {
        id: message
        text: "ready"
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }

    Processor {
        id: processor

    }

    Connections {
        target: processor
        onSendBegin: { console.log("locked"); setMode( locked ) }
        onSendEnd: { console.log("ready"); setMode( ready ) }
        onMessage: { message.text = text }
    }

    Component.onCompleted: {
        processor.send( dlgConfig.loadIni("url") );
    }


    /*
    Component.onCompleted: {
        openDb();
    }
    */



    /*
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
    */
}
