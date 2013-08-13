#include <QtWidgets>
#include <QtQml>
#include <QtQuick>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQuickView view;
    view.setSource(QUrl("qrc:/bibletime/qml/metro.qml"));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    return app.exec();
}
