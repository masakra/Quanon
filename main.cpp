#include "processor.h"
#include <QtGui/QGuiApplication>
#include <QQmlEngine>
#include <QtQuick/QQuickPaintedItem>
#include "qtquick2applicationviewer.h"


int main(int argc, char *argv[])
{
    qmlRegisterType< Processor >( "Processor", 1, 0, "Processor" );

    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;
    viewer.setMainQmlFile(QStringLiteral("qml/Quanon/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
