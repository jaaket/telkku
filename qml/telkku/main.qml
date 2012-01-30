import QtQuick 1.1
import com.nokia.meego 1.0
import Jake 1.0
import "UIConstants.js" as UIConstants

PageStackWindow {
    id: appWindow

    ListModel {
        id: channelItems
    }

    ListModel {
        id: addableChannels
    }

    function addChannel(channel) {
        var comp = Qt.createComponent("ListingModel.qml");
        channelItems.append({ "channelModel" : comp.createObject(appWindow, { "channel" : channel }),
                              "color" : UIConstants.colors[channelItems.count % UIConstants.colors.length] });
        channelItems.get(channelItems.count - 1).channelModel.fetchData();
        for (var i = 0; i < addableChannels.count; ++i) {
            if (addableChannels.get(i).name == channel) {
                addableChannels.remove(i);
                break;
            }
        }
    }

    function initChannelItems() {
        for (var i = 0; i < 16; ++i) {
            addChannel(availableChannels[i]);
        }

        for (i = 16; i < availableChannels.length; ++i) {
            addableChannels.append({ "name" : availableChannels[i] });
        }
    }

    initialPage: mainPage

    MainPage {
        id: mainPage

        Component.onCompleted: initChannelItems();
    }

    OptionsPage {
        id: optionsPage
    }

    platformStyle: PageStackWindowStyle {
        id: defaultStyle
        background: 'image://theme/meegotouch-video-background'
        backgroundFillMode: Image.Stretch
    }

    ToolBar {
        id: sharedToolBar
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
