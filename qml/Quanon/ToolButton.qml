import QtQuick 2.0

Rectangle {
    width: Math.max( 64, Math.min( Math.min( parent.width, parent.height ) / 5, 255 ) ); // from 64 to 255
    height: width
    color: "black"
    radius: 4
    smooth: true
    opacity: .2
    anchors.topMargin: 4
    anchors.leftMargin: 4
}
