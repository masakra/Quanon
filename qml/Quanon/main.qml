import QtQuick 2.0
import QtMultimedia 5.0
//import "Button.qml"

Rectangle {
    width: 360
    height: 360

    property var mode: {
        Ready: 0
        AudioRecord: 1
        Sending: 2
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
                //imageProcessor.processImage( preview );
            }
        }
    }

    Button {
        id: button
        text: qsTr("Shot")
        opacity: .5

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 30
        }

        onClicked: {
            camera.imageCapture.capture();
            camera.stop();
            button.text = qsTr("Send")
            progressBar.visible = true
            timer.start()
            //Qt.quit();
        }
    }

    ProgressBar {
        id: progressBar
        anchors {
            bottom: button.top
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
           button.text = progress + "%"
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

    /*
    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
    */
}
