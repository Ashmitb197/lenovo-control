#include "HardwareCapabilities.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qInfo() << "Model:" << HardwareCapabilities::deviceModel();
    qInfo() << "BIOS:" << HardwareCapabilities::biosVersion();
    qInfo() << "Profiles:" << HardwareCapabilities::performanceProfiles();
    qInfo() << "Conservation:" << HardwareCapabilities::hasConservationMode();
    qInfo() << "Fn Lock:" << HardwareCapabilities::hasFnLock();
    qInfo() << "Fan Mode:" << HardwareCapabilities::hasFanMode();
    qInfo() << "Keyboard Backlight:" << HardwareCapabilities::hasKeyboardBacklight();
    qInfo() << "Fan RPM:" << HardwareCapabilities::hasFanRpmMonitoring();

    return 0;
}
