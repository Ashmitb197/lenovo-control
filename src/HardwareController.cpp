#include "HardwareController.h"

#include <QFile>
#include <QDir>
#include <QRegularExpression>
#include <QTimer>
#include <QProcess>
#include <algorithm>
#include <QPointer>
#include <QSettings>

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


namespace {


QString intelGpuPath()
{
    const QDir directory("/sys/class/drm");

    for (const QString &name :
         directory.entryList({"card[0-9]*"}, QDir::Dirs | QDir::NoDotAndDotDot)) {

        const QString path = directory.filePath(name);

        if (readFile(path + "/device/vendor").toLower() == "0x8086")
            return path;
    }

    return {};
}

QString intelGpuFrequencyValue(const QString &path, bool maximum)
{
    if (path.isEmpty())
        return "Not available";

    const QStringList candidates = maximum
        ? QStringList{
            path + "/gt_max_freq_mhz",
            path + "/gt/gt0/rps_max_freq_mhz"
          }
        : QStringList{
            path + "/gt_cur_freq_mhz",
            path + "/gt/gt0/rps_cur_freq_mhz"
          };

    for (const QString &candidate : candidates) {
        bool ok = false;
        const int frequency = readFile(candidate).toInt(&ok);

        if (ok && frequency >= 0)
            return QString::number(frequency) + " MHz";
    }

    return "Not available";
}

QString cpuModelName()
{
    const QString contents = readFile("/proc/cpuinfo");

    for (const QString &line : contents.split('\n')) {
        if (line.startsWith("model name")) {
            const int colon = line.indexOf(':');
            if (colon >= 0)
                return line.mid(colon + 1).trimmed();
        }
    }

    return "Not available";
}

QString batteryHealthValue()
{
    const QString path = batteryPath();
    if (path.isEmpty())
        return "Not available";

    QString full = readFile(path + "/energy_full");
    QString design = readFile(path + "/energy_full_design");

    if (full.isEmpty() || design.isEmpty()) {
        full = readFile(path + "/charge_full");
        design = readFile(path + "/charge_full_design");
    }

    bool okFull = false;
    bool okDesign = false;

    const double f = full.toDouble(&okFull);
    const double d = design.toDouble(&okDesign);

    if (!okFull || !okDesign || d <= 0)
        return "Not available";

    return QString::number(100.0 * f / d, 'f', 1) + "%";
}

QString batteryCapacityValue(bool design)
{
    const QString path = batteryPath();

    if (path.isEmpty())
        return "Not available";

    const QString energy = readFile(
        path + (design ? "/energy_full_design" : "/energy_full"));

    bool ok = false;
    const double microWh = energy.toDouble(&ok);

    if (ok && microWh > 0)
        return QString::number(microWh / 1000000.0, 'f', 2) + " Wh";

    return "Not available";
}

QString batteryCycleValue()
{
    const QString path = batteryPath();
    if (path.isEmpty())
        return "Not available";

    const QString cycles = readFile(path + "/cycle_count");
    return cycles.isEmpty() ? "Not available" : cycles;
}

}

HardwareController::HardwareController(QObject *parent)
    : QObject(parent)
{
    QSettings settings("LenovoControl", "LenovoControl");

    const int savedInterval = settings.value("refreshInterval", 2000).toInt();
    if (savedInterval == 1000 || savedInterval == 2000 ||
        savedInterval == 3000 || savedInterval == 5000)
        m_refreshInterval = savedInterval;

    m_useFahrenheit = settings.value("useFahrenheit", false).toBool();

    m_hardwareTimer = new QTimer(this);

    connect(m_hardwareTimer, &QTimer::timeout,
            this, &HardwareController::refresh);

    m_hardwareTimer->start(m_refreshInterval);


    refresh();

    m_gpuTimer = new QTimer(this);
    m_gpuTimer->setInterval(m_refreshInterval);

    connect(m_gpuTimer, &QTimer::timeout, this, [this]() {
        if (m_gpuQueryRunning)
            return;

        m_gpuQueryRunning = true;
        auto *process = new QProcess(this);
        m_gpuProcess = process;

        process->setProgram("nvidia-smi");
        process->setArguments({
            "--query-gpu=name,temperature.gpu,utilization.gpu,memory.used,memory.total",
            "--format=csv,noheader,nounits"
        });

        connect(process, &QProcess::finished, this,
                [this, process](int exitCode, QProcess::ExitStatus status) {
            if (status == QProcess::NormalExit && exitCode == 0) {
                const QStringList values =
                    QString::fromUtf8(process->readAllStandardOutput())
                        .trimmed()
                        .section('\n', 0, 0)
                        .split(',');

                if (values.size() >= 5) {
                    m_gpuModel = values[0].trimmed();
                    m_gpuTemperature = values[1].trimmed() + " °C";
                    m_gpuUsage = values[2].trimmed() + "%";
                    m_gpuMemory = values[3].trimmed() +
                                  " / " + values[4].trimmed() + " MiB";
                    emit statusChanged();
                }
            }

            m_gpuQueryRunning = false;
            if (m_gpuProcess == process)
                m_gpuProcess = nullptr;
            process->deleteLater();
        });

        connect(process, &QProcess::errorOccurred, process,
                [this, process](QProcess::ProcessError error) {
            if (error == QProcess::FailedToStart) {
                m_gpuQueryRunning = false;
                if (m_gpuProcess == process)
                    m_gpuProcess = nullptr;
                process->deleteLater();
            }
        });

        process->start();
    });

    m_gpuTimer->start();
}

HardwareController::~HardwareController()
{
    if (m_hardwareTimer)
        m_hardwareTimer->stop();

    if (m_gpuTimer)
        m_gpuTimer->stop();

    if (m_gpuProcess &&
        m_gpuProcess->state() != QProcess::NotRunning) {
        m_gpuProcess->terminate();

        if (!m_gpuProcess->waitForFinished(1000)) {
            m_gpuProcess->kill();
            m_gpuProcess->waitForFinished(1000);
        }
    }

    if (m_process &&
        m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

void HardwareController::refresh()
{

    const QString intelPath = intelGpuPath();

    m_intelGpuModel = intelPath.isEmpty()
        ? "Not available"
        : "Intel Iris Xe Graphics";

    m_intelGpuFrequency = intelGpuFrequencyValue(intelPath, false);
    m_intelGpuMaxFrequency = intelGpuFrequencyValue(intelPath, true);

    m_cpuModel = cpuModelName();
    m_batteryHealth = batteryHealthValue();
    m_batteryCycles = batteryCycleValue();
    m_batteryFullCapacity = batteryCapacityValue(false);
    m_batteryDesignCapacity = batteryCapacityValue(true);

    // /proc/stat counters: total time and idle time.
    {
        const QString stat = readFile("/proc/stat");
        const QString firstLine = stat.section('\n', 0, 0);
        const QStringList fields =
            firstLine.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

        if (fields.size() >= 5 && fields[0] == "cpu") {
            quint64 total = 0;
            for (int i = 1; i < fields.size(); ++i)
                total += fields[i].toULongLong();

            const quint64 idle =
                fields[4].toULongLong() +
                (fields.size() > 5 ? fields[5].toULongLong() : 0);

            if (m_previousCpuTotal > 0 &&
                total > m_previousCpuTotal &&
                idle >= m_previousCpuIdle) {

                const quint64 deltaTotal = total - m_previousCpuTotal;
                const quint64 deltaIdle = idle - m_previousCpuIdle;

                const double usage =
                    100.0 * (1.0 - double(deltaIdle) / deltaTotal);

                m_cpuUsage =
                    QString::number(std::clamp(usage, 0.0, 100.0), 'f', 1) + "%";
            }

            m_previousCpuTotal = total;
            m_previousCpuIdle = idle;
        }
    }

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

void HardwareController::setRefreshInterval(int interval)
{
    if (interval != 1000 && interval != 2000 &&
        interval != 3000 && interval != 5000)
        return;

    if (m_refreshInterval == interval)
        return;

    m_refreshInterval = interval;

    if (m_hardwareTimer)
        m_hardwareTimer->setInterval(interval);

    if (m_gpuTimer)
        m_gpuTimer->setInterval(interval);

    QSettings settings("LenovoControl", "LenovoControl");
    settings.setValue("refreshInterval", interval);

    emit preferencesChanged();
}

void HardwareController::setUseFahrenheit(bool enabled)
{
    if (m_useFahrenheit == enabled)
        return;

    m_useFahrenheit = enabled;

    QSettings settings("LenovoControl", "LenovoControl");
    settings.setValue("useFahrenheit", enabled);

    emit preferencesChanged();
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
