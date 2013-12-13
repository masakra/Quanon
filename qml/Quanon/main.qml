import QtQuick 2.0
import QtMultimedia 5.0
import Processor 1.0

Rectangle {
    width: 360
    height: 360

    readonly property int ready: 0
    readonly property int audioRecord: 1
    property int status: ready

    //readonly property var mode: {
        //Ready: 0
        //AudioRecord: 1
        //Sending: 2
    //}

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
            setMode( ready )
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
               stop()

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
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
    */
}
