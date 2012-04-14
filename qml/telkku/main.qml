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
        for (var i = 0; i < availableChannels.length; ++i) {
            addableChannels.append({ "name" : availableChannels[i] });
        }

        var db = openDatabaseSync("TelkkuSettingsDB", "1.0",
                                  "Settings for telkku", 10000);

        db.readTransaction(
            function(tx) {
                var result = tx.executeSql("SELECT * FROM channels");

                if (result.rows.length > 0) {
                    for (var i = 0; i < result.rows.length; ++i) {
                        addChannel(result.rows.item(i).name);
                    }
                } else {
                    for (var i = 0; i < 16; ++i) {
                        addChannel(availableChannels[i]);
                    }

                }
            }
        )
    }

    initialPage: mainPage

    MainPage {
        id: mainPage

        Component.onCompleted: {
            var db = openDatabaseSync("TelkkuSettingsDB", "1.0",
                                      "Settings for telkku", 10000);

            // Create settings database on initialization if needed
            db.transaction(
                 function(tx) {
                    tx.executeSql('CREATE TABLE IF NOT EXISTS \
                                  channels(id INTEGER PRIMARY KEY, name TEXT)');
                }
            )

            initChannelItems();
        }

        // Save channel settings on exit
        Component.onDestruction: {
            var db = openDatabaseSync("TelkkuSettingsDB", "1.0",
                                      "Settings for telkku", 10000);

            db.transaction(
                function(tx) {
                    tx.executeSql('DELETE FROM channels');

                    for (var i = 0; i < channelItems.count; ++i) {
                        tx.executeSql('INSERT INTO channels (name) VALUES (?)',
                                      channelItems.get(i).channelModel.channel);
                    }
                }
            )
        }
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
