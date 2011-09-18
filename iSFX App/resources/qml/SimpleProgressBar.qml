import QtQuick 1.1

Rectangle {
    id: progress
    color: "#00ff00"
    property double percent: 0;
    width: (parent !== null) ? (parent.width)*percent/100 : percent
    height: (parent !== null) ? (parent.height) : 20
    opacity: 0.200

    function setProgress(pct) {
        percent = pct;
        width = pct/100*parent.width
    }

    function getProgress() {
        return percent;
    }
}
