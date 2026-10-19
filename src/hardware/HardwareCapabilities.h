#pragma once

#include <QString>
#include <QStringList>

class HardwareCapabilities {
public:
    static QString deviceModel();
    static QString biosVersion();

    static QStringList performanceProfiles();

    static bool hasConservationMode();
    static bool hasFnLock();
    static bool hasFanMode();
    static bool hasKeyboardBacklight();
    static bool hasFanRpmMonitoring();

private:
    static bool platformAttributeExists(const QString &name);
};
