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
    signal selectedSubj(int sId);
    onSelectedSubj: {
        window.subjectSelected(sId)
    }

    Item {
         id: selection
         anchors.fill: parent
         focus: true
         Keys.onReturnPressed: {
             subjectsTumbler.selectedSubj(subjectsTumbler.currentIndex);
             subjectsTumbler.destroy(1)
         }
         }

    }
