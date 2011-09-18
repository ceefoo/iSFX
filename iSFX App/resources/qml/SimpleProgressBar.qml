import QtQuick 1.1

Rectangle {
    id: progress
    color: "#00ff00"
    width: (parent !== null) ? (parent.width) : 100
    height: (parent !== null) ? (parent.height) : 20
    opacity: 0.200

    property double percent: 0;

    function setProgress(pct) {
        percent = pct;
        width = pct/100*parent.width
    }

    function getProgress() {
        return percent;
    }
}
