#include "HardwareController.h"

#include <QFile>
#include <QDir>
#include <QRegularExpression>
#include <QTimer>
#include <QProcess>
#include <algorithm>

namespace {

QString readFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    return QString::fromUtf8(file.readAll()).trimmed();
}

QString batteryPath()
{
    const QDir directory("/sys/class/power_supply");

    for (const QString &name :
         directory.entryList({"BAT*"}, QDir::Dirs | QDir::NoDotAndDotDot)) {
        const QString path = directory.filePath(name);

        if (readFile(path + "/type") == "Battery")
            return path;
    }

    return {};
}

QString cpuTemperature()
{
    const QDir directory("/sys/class/thermal");

    double preferred = -1;
    double fallback = -1;

    for (const QString &name :
         directory.entryList({"thermal_zone*"}, QDir::Dirs | QDir::NoDotAndDotDot)) {

        const QString path = directory.filePath(name);
        const QString type = readFile(path + "/type").toLower();

        bool valid = false;
        const double temperature =
            readFile(path + "/temp").toDouble(&valid) / 1000.0;

        if (!valid || temperature < 0 || temperature > 120)
            continue;

        if (type.contains("x86_pkg_temp") ||
            type.contains("cpu") ||
            type.contains("pkg")) {
            preferred = std::max(preferred, temperature);
        } else if (type.contains("acpitz") || type.contains("soc")) {
            fallback = std::max(fallback, temperature);
        }
    }

    const double value = preferred >= 0 ? preferred : fallback;

    return value >= 0
        ? QString::number(value, 'f', 1) + " °C"
        : "Not available";
}

QString memoryUsage()
{
    const QString contents = readFile("/proc/meminfo");

    qint64 total = 0;
    qint64 available = -1;

    for (const QString &line : contents.split('\n')) {
        const QStringList parts =
            line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        if (parts.size() < 2)
            continue;

        if (parts[0] == "MemTotal:")
            total = parts[1].toLongLong();

        if (parts[0] == "MemAvailable:")
            available = parts[1].toLongLong();
    }

    if (total <= 0 || available < 0)
        return "Not available";

    const double usedGiB = double(std::max<qint64>(0, total - available)) / 1048576.0;
    const double totalGiB = double(total) / 1048576.0;

    return QString("%1 / %2 GiB")
        .arg(usedGiB, 0, 'f', 1)
        .arg(totalGiB, 0, 'f', 1);
}

QString batteryStatus()
{
    const QString path = batteryPath();

    if (path.isEmpty())
        return "Not available";

    const QString capacity = readFile(path + "/capacity");
    const QString status = readFile(path + "/status");

    if (capacity.isEmpty())
        return status.isEmpty() ? "Not available" : status;

    return capacity + "%" + (status.isEmpty() ? "" : " · " + status);
}

}

HardwareController::HardwareController(QObject *parent)
    : QObject(parent)
{
    auto *timer = new QTimer(this);

    connect(timer, &QTimer::timeout,
            this, &HardwareController::refresh);

    timer->start(2000);

    refresh();
}

void HardwareController::refresh()
{
    m_temperature = cpuTemperature();
    m_memory = memoryUsage();
    m_battery = batteryStatus();

    const QString profile =
        readFile("/sys/firmware/acpi/platform_profile");

    m_profile = profile.isEmpty() ? "Not available" : profile;

    const QStringList conservationPaths = {
        "/sys/bus/platform/drivers/ideapad_acpi/VPC2004:00/conservation_mode",
        "/sys/bus/platform/devices/VPC2004:00/conservation_mode"
    };

    m_conservation = "Not available";

    for (const QString &path : conservationPaths) {
        const QString value = readFile(path);

        if (value == "1") {
            m_conservation = "Enabled";
            break;
        }

        if (value == "0") {
            m_conservation = "Disabled";
            break;
        }
    }

    emit statusChanged();
}

void HardwareController::setProfile(const QString &mode)
{
    if (mode != "low-power" &&
        mode != "balanced" &&
        mode != "performance")
        return;

    execute({"profile", mode});
}

void HardwareController::setConservation(bool enabled)
{
    execute({"conservation", enabled ? "on" : "off"});
}

void HardwareController::execute(const QStringList &arguments)
{
    if (m_busy)
        return;

    m_busy = true;
    emit busyChanged();

    auto *process = new QProcess(this);
    m_process = process;

    connect(process, &QProcess::finished, this,
            [this, process](int exitCode, QProcess::ExitStatus status) {
        const bool success =
            exitCode == 0 && status == QProcess::NormalExit;

        QString message = success
            ? "Setting updated successfully"
            : QString::fromUtf8(process->readAllStandardError()).trimmed();

        if (message.isEmpty())
            message = "Operation failed";

        m_busy = false;
        m_process = nullptr;

        emit busyChanged();
        emit operationFinished(success, message);

        refresh();
        process->deleteLater();
    });

    connect(process, &QProcess::errorOccurred, this,
            [this, process](QProcess::ProcessError error) {
        if (error != QProcess::FailedToStart)
            return;

        m_busy = false;
        m_process = nullptr;

        emit busyChanged();
        emit operationFinished(false, "Unable to start Lenovo Control");

        process->deleteLater();
    });

    // Reuse the installed, working CLI and its Polkit helper.
    process->start("lenovo-control", arguments);
}
