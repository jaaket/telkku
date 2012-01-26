import QtQuick 1.1
import com.nokia.meego 1.0
import Jake 1.0

Item {
    property ListingModel listingModel;

    Component {
        id: itemDelegate

        Item {
            height: 100
            width: length * 400

            Rectangle {
                anchors.fill: parent
                border.color: "lightblue"
                opacity: 0.0
            }

            Text {
                id: timeText
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 5
                text: Qt.formatTime(time, "hh:mm")
                font.pixelSize: 20
                color: "lightblue"
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: timeText.right
                anchors.right: parent.right
                text: name
                font.pixelSize: 30
                color: "white"
                elide: Text.ElideRight
            }
        }
    }

    ListView {
        id: flickable
        anchors.fill: parent
        model: listingModel
        delegate: itemDelegate
        orientation: ListView.Horizontal
    }
}
