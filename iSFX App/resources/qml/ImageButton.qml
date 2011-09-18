import QtQuick 1.1

Item {
    id: container

    property alias image: image
    property alias background: background

    width: image.width + 2;
    height: image.height + 2

    smooth: true

    signal clicked

    Rectangle {
        id: background

        anchors.fill: parent
        radius: 3

        gradient: Gradient {
            GradientStop { id: gradientStop; position: 0.0; color: palette.light }
            GradientStop { position: 1.0; color: palette.button }
        }

        SystemPalette { id: palette }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: { container.clicked() }
        }

        states: State {
            name: "pressed"
            when: mouseArea.pressed
            PropertyChanges { target: gradientStop; color: palette.dark }
        }
    }

    Image {
        id: image
        anchors.centerIn: parent
    }
}

