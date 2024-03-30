import QtQuick 2.5
import QtQuick.Controls 2.5

Window {
    id: mainWin
    visible: true
    width: 250
    height: 380
    title: qsTr("Список")

    signal chooseDay(value: int)

    Connections {
        target: pb // Specify the target to connect

        function onWriteTask(strValue, idx) {

            //console.log(strValue)
            if ( strValue.includes("$") ) {
                strValue = strValue.substring(0, strValue.length - 2) // get string without 2 last chars
                listModel.append({idshnik: strValue})

                listView1.model.get(idx).checkBoxValue = true
            }
            else {
                listModel.append({idshnik: strValue})
            }
        }

        function onClearTasks() {
            listModel.clear()
        }

        function onSetTextPrevDay(strValue) {
            lblPrevDay.text = strValue
        }

        function onSetTextNewDay(strValue) {
            lblNewDay.text = strValue
        }
    }

    Rectangle {
        id: prevDay
        x: 0
        y: 360
        width: 125
        height: 20
        border.width: 0.5

        MouseArea {
           id: myMouseArea
           anchors.fill: parent
           anchors.rightMargin: 0
           anchors.bottomMargin: 0
           anchors.leftMargin: 0
           anchors.topMargin: 0
           onClicked:
           {
               chooseDay(-1);
           }

           Label {
               anchors.fill: parent
               id: lblPrevDay
               text: ""
               horizontalAlignment: Text.AlignHCenter
               verticalAlignment: Text.AlignVCenter

           }

         }
    }

    Rectangle {
        id: nextDay
        x: 125
        y: 360
        width: 125
        height: 20
        border.width: 0.5

        MouseArea {
           id: myMouseArea2
           anchors.fill: parent
           anchors.rightMargin: 0
           anchors.bottomMargin: 0
           anchors.leftMargin: 0
           anchors.topMargin: 0
           onClicked: {
               chooseDay(1);
           }

           Label {
               anchors.fill: parent
               id: lblNewDay
               x: 0
               y: 364
               width: 69
               height: 16
               text: ""
               horizontalAlignment: Text.AlignHCenter
               verticalAlignment: Text.AlignVCenter
           }

         }
    }

    // ListView для представления данных в виде списка
    ListView {
        id: listView1

        model: ListModel {
            id: listModel // задаём ей id для обращения
            ListElement { checkBoxValue: true; idshnik: ""; }
            ListElement { checkBoxValue: false; idshnik: ""; }
        }

        width : parent.width
        anchors.top: parent.top
        anchors.bottom: prevDay.top
        anchors.bottomMargin: 0
        anchors.topMargin: 0


        delegate : CheckBox {
            anchors.left: parent.left
            anchors.right: parent.right
            enabled: !checked
            checked: checkBoxValue

            //задаём свойству text переменную, по имени которой будем задавать
            //свойства элемента
            text: idshnik
            height: 30

            onCheckStateChanged: {

                if ( !idshnik.includes("$") ) {
                    pb.taskDone = idshnik
                }

                switch (checkState) {
                    case Qt.Checked:
                        font.strikeout = true
                        break;
                    default:
                        break;
                }
            }
        }
    }


}
