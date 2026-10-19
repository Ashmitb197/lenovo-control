#include "HardwareCapabilities.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

namespace {

QString readText(const QString &path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    return QString::fromUtf8(file.readAll()).trimmed();
}

}

QString HardwareCapabilities::deviceModel()
{
    return readText("/sys/class/dmi/id/product_version");
}

QString HardwareCapabilities::biosVersion()
{
    return readText("/sys/class/dmi/id/bios_version");
}

QStringList HardwareCapabilities::performanceProfiles()
{
    return readText(
        "/sys/firmware/acpi/platform_profile_choices"
    ).split(' ', Qt::SkipEmptyParts);
}

bool HardwareCapabilities::platformAttributeExists(const QString &name)
{
    const QDir devices("/sys/bus/platform/devices");

    for (const QFileInfo &device :
         devices.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {

        if (QFileInfo::exists(device.absoluteFilePath() + "/" + name))
            return true;
    }

    return false;
}

bool HardwareCapabilities::hasConservationMode()
{
    return platformAttributeExists("conservation_mode");
}

bool HardwareCapabilities::hasFnLock()
{
    return platformAttributeExists("fn_lock");
}

bool HardwareCapabilities::hasFanMode()
{
    return platformAttributeExists("fan_mode");
}

bool HardwareCapabilities::hasKeyboardBacklight()
{
    const QDir leds("/sys/class/leds");

    for (const QString &name : leds.entryList(QDir::AllEntries |
                                               QDir::NoDotAndDotDot)) {
        if (name.contains("kbd_backlight", Qt::CaseInsensitive))
            return true;
    }

    return false;
}

bool HardwareCapabilities::hasFanRpmMonitoring()
{
    const QDir hwmon("/sys/class/hwmon");

    for (const QString &name :
         hwmon.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {

        const QDir sensor(hwmon.absoluteFilePath(name));

        if (!sensor.entryList(
                {"fan*_input"}, QDir::Files).isEmpty())
            return true;
    }

    return false;
}
