import QtQuick 1.1
import "."

Rectangle {
    id: container
    width: 1137
    height: 798
    color: "#FFFFFF"
    
    signal updateSignal;
    signal newSound (string s, variant obj);
    signal itemClicked (variant obj);
    
    
    Timer {
            id:timer
            interval: 15
            repeat: true
            onTriggered: {
              updateSignal();
            }
    }

    SoundColumn {
        id: soundcolumn1
        anchors.left: parent.left
        anchors.leftMargin: 2
    }

    SoundColumn {
        id: soundcolumn2
        anchors.left: soundcolumn1.right
        anchors.leftMargin: 2
        signal newSound
    }

    SoundColumn {
        id: soundcolumn3
        anchors.left: soundcolumn2.right
        anchors.leftMargin: 2
        signal newSound
    }

    SoundColumn {
        id: soundcolumn4
        anchors.left: soundcolumn3.right
        anchors.leftMargin: 2
        signal newSound
    }

    SoundColumn {
        id: soundcolumn5
        anchors.left: soundcolumn4.right
        anchors.leftMargin: 2
        signal newSound
    }

    Component.onCompleted: {
        newSound.connect(soundcolumn1.newSound)
        newSound.connect(soundcolumn2.newSound)
        newSound.connect(soundcolumn3.newSound)
        newSound.connect(soundcolumn4.newSound)
        newSound.connect(soundcolumn5.newSound)
        itemClicked.connect(soundcolumn1.itemClicked)
        timer.start();
    }
}