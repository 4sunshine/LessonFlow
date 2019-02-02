import QtQuick 2.11

GridView {
    visible: true
    id: studentsGrid
<<<<<<< HEAD:ui/widgets/viewStudents.qml
    width: parent.width
    height: parent.height
    cellHeight: cellWidth
=======
//    x: parent.width-40
//    y: parent.height-5
    width: parent.width
    height: parent.height
//    cellWidth: width/7 //7
    cellHeight: cellWidth //5
>>>>>>> b83d31d... Visual behaviour improved:viewStudents.qml
    flow: GridView.FlowLeftToRight
    verticalLayoutDirection: GridView.BottomToTop

    onCountChanged: {
        console.log(studflowModel.rowCount());
        window.nstudents(studflowModel.rowCount())
    }

    layoutDirection: GridView.RightToLeft

    removeDisplaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 200 }
    }

    delegate:

        Column{


        id: curItem

        Image {
            id: avatarImage
            source: avatar
            width: 8*studentsGrid.cellWidth/10
            height: width
            Rectangle{
                id: dataRecta
                width: avatarImage.width
                height: 2.5*avatarImage.height/10
                anchors.bottom: avatarImage.bottom
                color: "#ffffffff"
                radius: 5
                gradient:

                    Gradient {

                    GradientStop {
                        position: 0.00;
                        color: (status) ? "#9900ff00" : "#99ff00ea";
                    }
                    GradientStop {
                        position: 1.00;
                        color: (!ismain) ? "#99ae00ff" : "#99bbea00";
                    }

                }
                Grid{
                    rows: 1 //2
                    columns: 2
                    spacing: 2
                    Column{
                    Text{
                        id: nameText
                        text: name
                        font.bold: true
                        font.pointSize: 15
                        color: "#ffffffff"
                        anchors.leftMargin: 5//: parent.horizontalCenter
                    }
                    Text{
                        id: surnameText
                        text: surname
                        font.bold: true
                        font.pointSize: 15
                        color: "#ffffffff"
                        anchors.leftMargin: 5//: parent.horizontalCenter
                    }
                    }
                    Column{
                            Text{
                                id: pluminText
                                text: (pluses != "") ? pluses : " "
                                font.bold: true
                                font.pointSize: 15
                                color: "#ffffffff"
                                }

                            Text{
                                id: avmarkText
                                text: average
                                font.bold: true
                                font.pointSize: 15
                                color: "#ffffffff"
                                anchors.rightMargin: 5
                            }
                    }
                }
            }
        }
    }
}

