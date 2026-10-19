#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "HardwareController.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    app.setApplicationName("Lenovo Control");
    app.setApplicationVersion("0.4.0");

    HardwareController hardware;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty(
        "hardware", &hardware
    );

    engine.loadFromModule("LenovoControl", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
