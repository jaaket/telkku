import QtQuick 1.1
import com.nokia.meego 1.0
import "UIConstants.js" as UIConstants

Page {
    property real rowHeight: Math.max(UIConstants.minRowHeight, 1.0 * listView.height / channelItems.count)
    ListView {
        id: listView
        anchors.fill: parent
        model: channelItems
        highlight: highlight
        preferredHighlightBegin: 0.3 * height
        preferredHighlightEnd: 0.7 * height
        highlightRangeMode: ListView.ApplyRange

        MouseArea {
            anchors.fill: parent

            onClicked: {
                listView.currentIndex = listView.indexAt(mouseX,
                                                         mouseY + listView.contentY);
            }
        }

        delegate: Item {
            width: parent.width
            height: rowHeight

            Behavior on pos.y {
                SpringAnimation {
                    spring: 5
                    damping: 0.2
                }
            }

            Rectangle {
                anchors.fill: parent
                anchors.margins: 2
                border.color: model.color
                border.width: UIConstants.borderWidth
                radius: UIConstants.rectangleRadius
                color: "transparent"
            }

            Text {
                anchors { top: parent.top; bottom: parent.bottom;
                    left: parent.left; right: parent.horizontalCenter;
                    margins: 10 }
                color: "white"
                font.pixelSize: 20
                text: channelModel.channel
            }

            Image {
                Behavior on y {
                    NumberAnimation {
                        duration: 400
                        easing.type: Easing.InOutQuad
                    }
                }

                id: image
                anchors { top: parent.top; bottom: parent.bottom;
                    left: parent.horizontalCenter; right: parent.right;
                    margins: 10 }
                source: "qrc:/" + channelModel.channel
                smooth: true
                fillMode: Image.PreserveAspectFit
            }
        }

        ScrollDecorator {
            flickableItem: listView
        }
    }

    Component {
        id: highlight
        Rectangle {
            width: parent.width
            height: rowHeight
            radius: UIConstants.rectangleRadius
            color: UIConstants.mainColor
        }
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: appWindow.pageStack.pop();
        }
        ToolIcon {
            iconId: "toolbar-delete"
            onClicked: listView.model.remove(listView.currentIndex);
        }
        ToolIcon {
            iconId: "toolbar-add"
            onClicked: selectionDialog.open();
        }
        ToolIcon {
            iconId: "toolbar-down"
            onClicked: {
                listView.model.move(listView.currentIndex,
                                    listView.currentIndex + 1, 1);
                listView.currentIndex = listView.currentIndex + 1;
                listView.currentIndex = listView.currentIndex - 1;
            }
        }
        ToolIcon {
            iconId: "toolbar-up"
            onClicked: {
                listView.model.move(listView.currentIndex,
                                    listView.currentIndex - 1, 1);
                listView.currentIndex = listView.currentIndex + 1;
                listView.currentIndex = listView.currentIndex - 1;
            }
        }
    }

    MultiSelectionDialog {
        id: selectionDialog
        titleText: "Add Channel"
        model: availableChannels
        acceptButtonText: "Add"
        delegate: Item {
            width: parent.width
            height: 20

            Text {
                anchors.fill: parent
                color: "white"
                font.pixelSize: 20
                text: modelData
            }
        }
    }
}
