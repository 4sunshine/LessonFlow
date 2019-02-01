import QtQuick 2.11

GridView {
    visible: true
    id: studentsGrid
    x: parent.width-40; y: parent.height-5;
    width: 9.5*parent.width/10
    height: 9.5*parent.height/10
    cellWidth: width/7 //7
    cellHeight: height/5 //5
    flow: GridView.FlowLeftToRight
    verticalLayoutDirection: GridView.BottomToTop
    layoutDirection: GridView.RightToLeft
    removeDisplaced: Transition {
        NumberAnimation { properties: "x,y"; duration: 200 }
    }
    delegate:

        Column{


        id: curItem

        Image {
            id: avatarImage
            source: avatar//"file:///home/sunshine/qtprojects/smartjournal/avatar/10А/25.png"
            width: (!isMain) ? 8*studentsGrid.cellWidth/10 : 2*studentsGrid.cellWidth
            height: width


            Rectangle{
                id: dataRecta
                width: parent.width
                height: 2.5*parent.height/10
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
                        color: "#99ae00ff";
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

