#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

#include "HardwareController.h"
#include "HardwareDBus.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    app.setApplicationName("Lenovo Control Service");
    app.setApplicationVersion("0.4.0");

    HardwareController hardware;
    HardwareDBus dbus(&hardware);

    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.registerService("org.lenovo.Control")) {
        qCritical() << "Cannot register Lenovo Control D-Bus service:"
                    << bus.lastError().message();
        return 1;
    }

    if (!bus.registerObject(
            "/Hardware",
            &dbus,
            QDBusConnection::ExportAllSlots)) {
        qCritical() << "Cannot register Lenovo Control hardware object:"
                    << bus.lastError().message();
        bus.unregisterService("org.lenovo.Control");
        return 1;
    }

    qInfo() << "Lenovo Control background service started";
    return app.exec();
}
