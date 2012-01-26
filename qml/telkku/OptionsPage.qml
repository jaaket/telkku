import QtQuick 1.1
import com.nokia.meego 1.0
import "colors.js" as Colors

Page {
    ListView {
        id: listView
        anchors.fill: parent
        model: channelItems
        highlight: highlight
        preferredHighlightBegin: 300
        preferredHighlightEnd: 500
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
            height: 80

            Behavior on pos.y {
                SpringAnimation {
                    spring: 5
                    damping: 0.2
                }
            }

            Rectangle {
                anchors.fill: parent
                anchors.margins: 2
                radius: 10
                color: model.color
            }

            Image {
                Behavior on y {
                    NumberAnimation {
                        duration: 400
                        easing.type: Easing.InOutQuad
                    }
                }

                id: image
                anchors.fill: parent
                anchors.margins: 4
                source: "qrc:/" + channelModel.channel
                smooth: true
                fillMode: Image.PreserveAspectFit
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.pixelSize: 40
                color: "white"
                visible: image.status == Image.Error
                text: channelModel.channel
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
            height: 80
            radius: 10
            color: "white"
        }
    }

    tools: ToolBarLayout {
        ToolIcon {
            iconId: "toolbar-back"
            onClicked: appWindow.pageStack.pop()
        }
        ToolIcon {
            iconId: "toolbar-delete"
            onClicked: listView.model.remove(listView.currentIndex);
        }
        ToolIcon {
            iconId: "toolbar-add"
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
}
