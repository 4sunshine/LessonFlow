import QtQuick 2.12
import QtQuick.Controls 2.4

Column{
    property var subjectId
    property var lessonNum
    id: lessonNumber
    anchors.centerIn: parent
    signal selectedSubj(int subjId, int lsnNum);//SELECTED SUBJECT ID + LESSON NUMBER
    onSelectedSubj: {
        window.subjectSelected(subjectId, lessonNum)
    }

    Text{
        id: spinText
        text: qsTr("Номер урока")
        font.bold: true
        font.pointSize: 16
        font.family: "Ubuntu"
        anchors.horizontalCenter: spinNum.horizontalCenter
    }
    SpinBox {
        id: spinNum
        value: 1
        from: 0 // 0 IF LESSON OUT OF TIMETABLE. ALSO IF LESSON OUT OF TIMETABLE BEFORE
        //FIRST LESSON AT SEMESTER MUST BE CHECKED
        to: 6
        focus: true
        stepSize: 1
        font.bold: true
        font.pointSize: 20
        font.family: "Ubuntu"
        Keys.onRightPressed: {spinNum.increase()}
        Keys.onLeftPressed: {spinNum.decrease()}
        Keys.onReturnPressed: {
            lessonNum = spinNum.value
            subjectId = subjectsTumbler.currentIndex
            lessonNumber.selectedSubj(subjectId, lessonNum);
            lessonNumber.destroy(1)
            subjectsTumbler.destroy(1)
        }
    }
}
