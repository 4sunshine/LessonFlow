import QtQuick 2.12
import QtQuick.Window 2.12

Window {

    signal subjectSelected(int classIndex, int lessonNumber)
    signal nstudents(int studentsCount) //COUNT OF STUDENTS IN FLOW

    property var models //PROPERTY HOLDS CLASSNAMES
    property var popups //PROPERTY HOLDS STUDENTS
    property var optWidth //OPTIMAL CELL WIDTH IN STUDENTS FLOW MODEL
    property var optCount //OPTIMAL HORIZONTAL CELLS COUNT

    id: window
    visible: true
    width: Screen.width/2
    height: Screen.height/2
    color: "#00000000"
    x: (Screen.width-window.width)/2
    y: (Screen.height-window.height)/2

    flags: {Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.TransparentMode}

    onNstudents: {
        if( optCount > 0 ) {
            if ( studentsCount > 0) {
                height = (optWidth/0.95)*( Math.floor((studentsCount-1)/optCount)+1 )
            }
            else {
                height = Screen.desktopAvailableHeight/40
            }
        }
    }
    //DEAL WITH Y POSITION OF WINDOW WHEN STUDENTS CREATED
    onHeightChanged: {
        if (optCount)
            y = Screen.height - height
    }

    onModelsChanged: {

        var componentst = Qt.createComponent("qrc:/ui/widgets/subjectsTumbler.qml");
        var tumblersub=componentst.createObject(window,
                                                {"x": 200, "y": 200})
        tumblersub.model=window.models}

    onPopupsChanged: {
        image.visible=false
        width=Screen.desktopAvailableWidth
        height=Screen.desktopAvailableHeight
        color="#00000000"
//        x:0
//        y:0

        var componentst = Qt.createComponent("qrc:/ui/widgets/viewStudents.qml");
        var studFlowView=componentst.createObject(window,
                                                {"x": 0, "y": 0,
                                                  "cellWidth": optWidth})
        studFlowView.model=studflowModel
        }

    Image {
        id: image
        width: parent.width/6
        height: image.width
        x: (parent.width-image.width)/2
        y: image.height/4
        source: "qrc:/ui/graphics/path868white.png"
    }
}
