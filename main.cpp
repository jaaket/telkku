#include <QtGui/QApplication>
#include <QtDeclarative>
#include "qmlapplicationviewer.h"
#include "listingmodel.h"
#include "guidedata.h"
#include "descriptionfetcher.h"
#include <iostream>

Q_DECLARE_METATYPE(GuideData*);

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    qmlRegisterType<ListingModel>("Jake", 1, 0, "ListingModel");

    GuideData *guideData = new GuideData;

    DescriptionFetcher *descriptionFetcher = new DescriptionFetcher;
    descriptionFetcher->setGuideData(guideData);

    ListingModel::setDefaultGuideData(guideData);

    QmlApplicationViewer viewer;

    viewer.rootContext()->setContextProperty("availableChannels", QVariant::fromValue(guideData->availableChannels()));
    viewer.rootContext()->setContextProperty("guideData", QVariant::fromValue((QObject*)guideData));
    viewer.rootContext()->setContextProperty("descriptionFetcher", QVariant::fromValue((QObject*)descriptionFetcher));

    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/telkku/main.qml"));
    viewer.showExpanded();

    return app->exec();
}
