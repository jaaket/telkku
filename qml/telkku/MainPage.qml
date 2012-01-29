import QtQuick 1.1
import com.nokia.meego 1.0
import Jake 1.0
import Qt.labs.components 1.1
import "UIConstants.js" as UIConstants

Page {
    property double widthScale: 500.0
    property int rowHeight: Math.max(UIConstants.minRowHeight, 1.0 * flickable.height / channelItems.count)
    property date today: new Date;

    function initFlickablePosition() {
        var date = new Date;
        flickable.contentX = (date.getHours() + date.getMinutes() / 60.0 - 0.5) * widthScale;
    }

    // Guide content
    Flickable {
        id: flickable
        anchors {
            left: parent.left; right: parent.right;
            bottom: parent.bottom; top: timeline.bottom;
        }
        contentWidth: 24 * widthScale
        contentHeight: channelItems.count * rowHeight
        flickableDirection: rowHeight == UIConstants.minRowHeight ?
                                Flickable.HorizontalAndVerticalFlick : Flickable.HorizontalFlick

        Behavior on contentX {
            PropertyAnimation {
                easing.type: Easing.OutExpo
            }
        }

        Column {
            move: Transition {
                NumberAnimation { properties: "x, y"; easing.type: Easing.InOutQuad }
            }

            Repeater {
                model: channelItems
                delegate: rowDelegate
            }
        }

        Component.onCompleted: initFlickablePosition();
    }

    // Channel row delegate
    Component {
        id: rowDelegate

        Item {
            id: row
            height: rowHeight
            width: 24 * widthScale
            property color color: channelItems.get(index).color;

            Repeater {
                model: channelModel
                delegate: itemDelegate
            }
        }
    }

    // Program item delegate
    Component {
        id: itemDelegate

        Item {
            height: rowHeight
            width: length * widthScale
            //x: startTimeSecs * widthScale
            x: (dayOffset * 24 + startTime.getHours() + startTime.getMinutes() / 60.0) * widthScale

            Rectangle {
                id: rect
                anchors.margins: 2
                anchors.fill: parent
                opacity: 1.0
                radius: UIConstants.rectangleRadius
                border.color: row.color
                border.width: UIConstants.borderWidth
                color: "transparent"
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.fill: parent
                anchors.margins: 10
                verticalAlignment: Text.AlignVCenter
                text: name
                font.pixelSize: 20
                color: "white"
                elide: Text.ElideRight
            }

           MouseArea {
                id: mouseArea
                anchors.fill: parent

                onClicked: {
                    descriptionDialog.titleText = name + "\n" +
                            Qt.formatTime(startTime, "hh.mm") + " - " +
                            Qt.formatTime(endTime, "hh.mm");
                    descriptionDialog.open();
                    descriptionFetcher.fetchDescription(channel, index);
                }
            }
        }
    }

    // Timeline background
    Rectangle {
        id: timeline
        x: 0
        y: 0
        width: 360
        anchors { left: parent.left; right: parent.right; top: parent.top; }
        height: 40
        color: UIConstants.mainColor
    }

    // Timeline content
    Flickable {
        contentX: flickable.contentX

        Row {
            Repeater {
                model: 48

                Item {
                    pos.x: 0.5 * index * widthScale

                    Text {
                        verticalAlignment: Text.AlignVCenter
                        pos.x: -35
                        height: 40
                        color: "white"
                        font.pixelSize: index % 2 === 0 ? 30 : 20
                        text: Qt.formatTime(new Date(0, 0, 0, 0, index * 30, 0, 0), "hh:mm")
                    }

                    Rectangle {
                        anchors.top: timeline.bottom
                        pos.x: -4
                        width: 8
                        height: 854
                        color: "white"
                        opacity: index % 2 === 0 ? 0.5 : 0.25
                    }
                }
            }
        }
    }



    // Current time indicator
    Flickable {
        contentX: flickable.contentX
        anchors.top: timeline.bottom
        anchors.bottom: parent.bottom

        Item {
            id: time
            width: 8
            height: parent.height

            Rectangle {
                anchors.fill: parent
                opacity: 0.8
                color: "red"
            }

            Timer {
                interval: 1000
                running: true
                repeat: true

                onTriggered: {
                    var date = new Date;
                    time.pos.x = widthScale * (date.getHours() + date.getMinutes() / 60.0);
                }
            }
        }
    }

    // Channel list background
    Rectangle {
        anchors { left: parent.left; top: timeline.bottom; bottom: parent.bottom }
        width: 60
        color: UIConstants.mainColor
    }

    // Channel list
    Flickable {
        anchors { left: parent.left; top: timeline.bottom; bottom: parent.bottom }
        width: 60
        contentY: flickable.contentY
        contentHeight: channelItems.count * rowHeight
        clip: true
        interactive: false

        Column {
            width: 60

            move: Transition {
                NumberAnimation { properties: "x, y"; easing.type: Easing.InOutQuad }
            }

            Repeater {
                model: channelItems

                Item {
                    width: parent.width
                    height: rowHeight
                    opacity: 1.0

                    Image {
                        id: channelLogo
                        anchors { left: parent.left; right: parent.right;
                            verticalCenter: parent.verticalCenter }
                        height: rowHeight
                        anchors.margins: 2
                        source: "qrc:/" + channelModel.channel
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                    }

                    Text {
                        anchors { verticalCenter: parent.verticalCenter;
                            horizontalCenter: parent.horizontalCenter }
                        text: channelModel.channel
                        font.pixelSize: 24
                        verticalAlignment: Text.AlignVCenter
                        color: "white"
                        visible: channelLogo.status == Image.Error
                    }
                }
            }
        }
    }

    // Loading in progress indicators
    Flickable {
        anchors { top: timeline.bottom; bottom: parent.bottom; horizontalCenter: parent.horizontalCenter }
        width: 100
        contentY: flickable.contentY
        contentHeight: channelItems.count * rowHeight
        clip: true
        interactive: false

        Column {
            width: 100
            anchors.horizontalCenter: parent.horizontalCenter

            Repeater {
                model: channelItems

                Item {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: rowHeight
                    height: rowHeight

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: 2
                        radius: 10
                        color: "white"
                        opacity: 0.5
                        visible: channelModel.loading
                    }

                    BusyIndicator {
                        anchors.centerIn: parent
                        running: channelModel.loading
                        visible: channelModel.loading
                    }
                }
            }
        }
    }

    // Description dialog
    QueryDialog {
        id: descriptionDialog
        message: descriptionFetcher.description
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-home"
            onClicked: initFlickablePosition();
        }

        ToolIcon {
            iconId: "toolbar-settings"
            onClicked: appWindow.pageStack.push(optionsPage);
        }
    }
}
