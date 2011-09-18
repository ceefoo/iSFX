import QtQuick 1.1
import "."

Rectangle {
    id: container;
    width: 300;
    height: 300;
    color: "#606060";
    
    signal updateSignal;
    signal newEntry;
    
    ListModel {
      id:listModel;
    }
    
    Component {
      id: elementDelegate;
      Rectangle {
        width: parent.width;
        height: 20;
        color:"#A0A0A0";
        Text {
          anchors.fill: parent;
          text:theText;
          color:"white";
        }  
      }
    }
    
    ListView {
      id: listView;
      anchors.fill: parent;
      model: listModel;
      delegate: elementDelegate;
    }
    
    MouseArea {
      focus: true;
      anchors.fill: parent;
      onClicked: {
        listModel.append({theText:"dummy"})
      }
    }
    
    function insertItem(i, text) {
      listModel.insert(i, {theText:text})
    }
    
    function appendItem(text) {
      listModel.append({theText:text})
    }
    
    Timer {
      id:timer;
      interval: 15;
      repeat: true;
      onTriggered: {
        updateSignal();
      }
    }

    Component.onCompleted: {
      timer.start();
    }
}
