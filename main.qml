import QtQuick 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.12

Window {

    signal subjectSelected(int classIndex, int lessonNumber)

    property var models
    property var popups

    id: window
    visible: true
    width: Screen.width/2
    height: Screen.height/2
    color: "#00000000"
    x: (Screen.width-window.width)/2
    y: (Screen.height-window.height)/2
    flags: Qt.FramelessWindowHint

    onModelsChanged: {
        console.log("Classes got->CreatingTumbler")
        var componentst = Qt.createComponent("subjectsTumbler.qml");
        var tumblersub=componentst.createObject(window,
                                                {"x": 200, "y": 200})
        tumblersub.model=window.models}

    onPopupsChanged: {
        console.log("POPUPS CHANGED")

        image.visible=false
        window.width=Screen.desktopAvailableWidth
        window.height=Screen.desktopAvailableHeight
        window.color="#00000000"
        x:0
        y:0

        var componentst = Qt.createComponent("viewStudents.qml");
        var studFlowView=componentst.createObject(window,
                                                {"x": 0, "y": 0})
        studFlowView.model=studflowModel
        }

    Image {
        id: image
        width: parent.width/6
        height: image.width
        x: (parent.width-image.width)/2
        y: image.height/4
        source: "path868white.png"
    }

}
