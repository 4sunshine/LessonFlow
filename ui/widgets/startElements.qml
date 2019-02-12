import QtQuick 2.12

Item {
    id: startElements
    width: parent.width
    height: parent.height

    Rectangle {
        id: rectStart
        width: parent.width
        height: parent.height
        radius: 40
        color: "#aa35c0cd"//"#aaea5345"
    }

    Image {
        id: lessonFlow
        source: "qrc:/ui/graphics/lfm.png"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        width: 0.9*parent.width
    }

    Image {
        id: image
        width: 0.6*parent.width
        fillMode: Image.PreserveAspectFit
        anchors.bottom: dev.top
        anchors.horizontalCenter: parent.horizontalCenter
//        x: (parent.width-image.width)/2
//        y: image.height/4
        source: "qrc:/ui/graphics/logos.png"
    }

    Text {
        id: dev
        text: qsTr("Приложение разработано в рамках проекта SmartClass")
        color: "#ffffffff"
        anchors.bottom: smarturl.top
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: parent.height/25
    }

    Text {
        id: smarturl
        text: qsTr("vk.com/lalaboratory, 2019")
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        font.pixelSize: parent.height/25
        font.bold: true
        color: "#fffffd00"
    }

}
