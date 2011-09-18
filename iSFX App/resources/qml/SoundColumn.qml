import QtQuick 1.1

Rectangle {
    id: container
    width: 225
    height: 798
    color: "#404040"

    signal signalNewSound(variant o, int i)

    // Component encapsulates stuff that would usually make up another file.
    // So it doesn't show up anywhere. But this one does because we use it
    // as the lists delegate (or something like that).
    Component {
        id: elementDelegate

        Item {
            id: songContainer
            width: listView.width
            height: 17
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                id: background
                color: "#808080"
                border.width:1
                border.color: "#606060"
                anchors.fill: parent


                StripedBar { // Stripes for denoting loops
                    id: marks
                    z: 0
                    anchors.fill: parent
                }

                SimpleProgressBar {
                    id: progressbar
                    percent: progress
                }

                ImageButton {
                    objectName: "play"
                    id: play_button
                    //text: "Play"
                    image.source: "../images/play.png"
                    signal play
                    y: 0
                    x: 0
                    
                    width:17
                    height:17
                    image.width: width
                    image.height: height
                    image.sourceSize.width: width
                    image.sourceSize.height: height

                    background.radius:0
                    background.opacity: 0.200

                    Component.onCompleted: {

                        //progressbar.setProgress(Math.random()*100);
                        clicked.connect(songContainer.play);
                        marks.addStripe(Math.random()*150, {color:"red"});
                    }
                }

                Text { // Text for song name // possibly time
                    id: songName
                    width: 180
                    anchors.bottom: parent.bottom
                    anchors.top: parent.top
                    anchors.left: play_button.right
                    anchors.leftMargin: 4

                    text: key + " - " + song_name
                    verticalAlignment: Text.AlignVCenter

                }

            }
            
            Component.onCompleted: {
            console.log("index:" + index + " completed.")
              signalNewSound(songContainer, index)
            }
            
            signal clicked;

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    drag.target = null;
                if (mouse.button == Qt.RightButton)
                    background.color = 'blue';
                else {
                    background.color = 'red';
                    clicked()
                    }
                }
                preventStealing: true
                drag.target: null
                drag.axis: Drag.YAxis
                drag.minimumY: listView.contentY-songContainer.height/2
                drag.maximumY: listView.contentHeight-songContainer.height

                property int positionStarted: 0
                property int positionCurrent: 0
                property int newIndex:
                    index + ((positionCurrent - positionStarted < 0) ?
                        Math.ceil((positionCurrent - positionStarted)/songContainer.height)
                      : Math.floor((positionCurrent - positionStarted)/songContainer.height))
                property bool held: false

                onPressAndHold: {
                    songContainer.z = 2;
                    positionStarted = songContainer.y;
                    drag.target = songContainer;
                    songContainer.opacity = 0.80;
                    //listView.interactive = false;
                    held = true;
                }
                onReleased: {
                    songContainer.y = listView.contentY + index*songContainer.height
                    songContainer.z = 1;
                    songContainer.opacity = 1;
                    //listView.interactive = true;
                    drag.target = null;
                    held = false;
                }

                onPositionChanged: {
                    if (!held) {
                      //listView.interactive = true;
                      drag.target = null;
                    } else {
                        positionCurrent = drag.target.y;
                        if (newIndex < 1 && index >= 1) {
                            listModel.move(index,0,1);
                            drag.target = songContainer;
                            positionStarted = drag.target.y;
                        } else if (newIndex > listView.count - 1) {
                            listModel.move(index,listView.count - 1,1);
                            positionStarted = drag.target.y;
                        } else if (index != newIndex){
                            listModel.move(index,newIndex,1);
                            positionStarted = drag.target.y;
                        }
                    }
                }
            }
        }
    }

    ListModel {
        id: listModel
        ListElement { key: "1"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "2"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "3"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "4"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "5"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "6"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "7"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "8"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "9"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "0"; song_name: "23.mp3"; progress: 0.0 }
        ListElement { key: "q"; song_name: "13.mp3"; progress: 0.0 }
        ListElement { key: "w"; song_name: "Rich Girl.mp3"; progress: 0.0 }
        ListElement { key: "e"; song_name: "1.mp3"; progress: 0.0 }
        ListElement { key: "r"; song_name: "2.mp3"; progress: 0.0 }
        ListElement { key: "t"; song_name: "3.mp3"; progress: 0.0 }
        ListElement { key: "y"; song_name: "4.mp3"; progress: 0.0 }
        ListElement { key: "u"; song_name: "7.mp3"; progress: 0.0 }
        ListElement { key: "i"; song_name: "5.mp3"; progress: 0.0 }
        ListElement { key: "o"; song_name: "Lazy Sunday.mp3"; progress: 0.0 }
        ListElement { key: "p"; song_name: "6.mp3"; progress: 0.0 }
        ListElement { key: "a"; song_name: "8.mp3"; progress: 0.0 }
        ListElement { key: "s"; song_name: "9.mp3"; progress: 0.0 }
        ListElement { key: "d"; song_name: "10.mp3"; progress: 0.0 }
        ListElement { key: "f"; song_name: "11.mp3"; progress: 0.0 }
        ListElement { key: "g"; song_name: "Lights.mp3"; progress: 0.0 }
        ListElement { key: "h"; song_name: "unknow.mp3"; progress: 0.0 }
        ListElement { key: "j"; song_name: "12.mp3"; progress: 0.0 }
        ListElement { key: "k"; song_name: "14.mp3"; progress: 0.0 }
        ListElement { key: "l"; song_name: "15.mp3"; progress: 0.0 }
        ListElement { key: "z"; song_name: "16.mp3"; progress: 0.0 }
        ListElement { key: "x"; song_name: "17.mp3"; progress: 0.0 }
        ListElement { key: "c"; song_name: "18.mp3"; progress: 0.0 }
        ListElement { key: "v"; song_name: "19.mp3"; progress: 0.0 }
        ListElement { key: "b"; song_name: "20.mp3"; progress: 0.0 }
        ListElement { key: "n"; song_name: "21.mp3"; progress: 0.0 }
        ListElement { key: "m"; song_name: "22.mp3"; progress: 0.0 }
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: listModel
        delegate: elementDelegate
        interactive: false
        signal itemClicked (variant obj)
        //boundsBehavior: Flickable.DragOverBounds
        /*

        onItemClicked: {
            console.log(index)
        }
        */
    }
    
    function addSound(i, k, s) {
      listModel.insert(i, {key:k, song_name:s, progress:0.0})
    }
    
    function getItem(i) {
      return listModel((i+1)*225/2, (i+1)*17/2)
    }
    
    function model() {
    return listModel;
    }
}
