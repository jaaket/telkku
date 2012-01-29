import QtQuick 1.1
import com.nokia.meego 1.0
import Jake 1.0
import "UIConstants.js" as UIConstants

PageStackWindow {
    id: appWindow

    ListModel {
        id: channelItems
    }

    function initChannelItems() {
        for (var i = 0; i < 16; ++i) {
            var comp = Qt.createComponent("ListingModel.qml");

            var obj = comp.createObject(appWindow);

            channelItems.append({ "channelModel" : comp.createObject(appWindow,
                                                                     { "channel" : availableChannels[i] }),
                                  "color" : UIConstants.colors[i % UIConstants.colors.length] });

            channelItems.get(i).channelModel.fetchData();
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
