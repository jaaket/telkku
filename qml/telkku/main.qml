import QtQuick 1.1
import com.nokia.meego 1.0
import Jake 1.0

PageStackWindow {
    id: appWindow

    initialPage: mainPage

    MainPage {
        id: mainPage
    }

    OptionsPage {
        id: optionsPage
    }

    platformStyle: PageStackWindowStyle {
            id: defaultStyle
            background: 'image://theme/meegotouch-video-background'
            backgroundFillMode: Image.Stretch
    }
}
