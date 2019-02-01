import QtQuick 2.11
import QtQuick.Controls 2.4

Tumbler {
    wrap: true
    anchors.centerIn: parent
    font.bold: true
    font.pointSize: 20
    font.family: "Ubuntu"
    id: subjectsTumbler
    objectName: "subjectsTumbler"
    width: parent.width/2

    Item {
         id: selection
         anchors.fill: parent
         focus: true
         Keys.onReturnPressed: {
             selection.focus = false
             subjectsTumbler.visible = false
             subjectsTumbler.enabled = false
             var newSpin = Qt.createComponent("lessonNumber.qml");
             var lessonNum=newSpin.createObject(window)
            }
         }

    }
