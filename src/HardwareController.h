#pragma once

#include <QObject>
#include <QString>
#include <QProcess>

class HardwareController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString profile READ profile NOTIFY statusChanged)
    Q_PROPERTY(QString conservation READ conservation NOTIFY statusChanged)
    Q_PROPERTY(QString temperature READ temperature NOTIFY statusChanged)
    Q_PROPERTY(QString memory READ memory NOTIFY statusChanged)
    Q_PROPERTY(QString battery READ battery NOTIFY statusChanged)

    Q_PROPERTY(QString cpuModel READ cpuModel NOTIFY statusChanged)
    Q_PROPERTY(QString cpuUsage READ cpuUsage NOTIFY statusChanged)
    Q_PROPERTY(QString intelGpuModel READ intelGpuModel NOTIFY statusChanged)
    Q_PROPERTY(QString intelGpuFrequency READ intelGpuFrequency NOTIFY statusChanged)
    Q_PROPERTY(QString intelGpuMaxFrequency READ intelGpuMaxFrequency NOTIFY statusChanged)
    Q_PROPERTY(QString gpuModel READ gpuModel NOTIFY statusChanged)
    Q_PROPERTY(QString gpuTemperature READ gpuTemperature NOTIFY statusChanged)
    Q_PROPERTY(QString gpuUsage READ gpuUsage NOTIFY statusChanged)
    Q_PROPERTY(QString gpuMemory READ gpuMemory NOTIFY statusChanged)
    Q_PROPERTY(QString batteryHealth READ batteryHealth NOTIFY statusChanged)
    Q_PROPERTY(QString batteryCycles READ batteryCycles NOTIFY statusChanged)
    Q_PROPERTY(QString batteryFullCapacity READ batteryFullCapacity NOTIFY statusChanged)
    Q_PROPERTY(QString batteryDesignCapacity READ batteryDesignCapacity NOTIFY statusChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit HardwareController(QObject *parent = nullptr);

    QString profile() const { return m_profile; }
    QString conservation() const { return m_conservation; }
    QString temperature() const { return m_temperature; }
    QString memory() const { return m_memory; }
    QString battery() const { return m_battery; }

    QString cpuModel() const { return m_cpuModel; }
    QString cpuUsage() const { return m_cpuUsage; }
    QString intelGpuModel() const { return m_intelGpuModel; }
    QString intelGpuFrequency() const { return m_intelGpuFrequency; }
    QString intelGpuMaxFrequency() const { return m_intelGpuMaxFrequency; }
    QString gpuModel() const { return m_gpuModel; }
    QString gpuTemperature() const { return m_gpuTemperature; }
    QString gpuUsage() const { return m_gpuUsage; }
    QString gpuMemory() const { return m_gpuMemory; }
    QString batteryHealth() const { return m_batteryHealth; }
    QString batteryCycles() const { return m_batteryCycles; }
    QString batteryFullCapacity() const { return m_batteryFullCapacity; }
    QString batteryDesignCapacity() const { return m_batteryDesignCapacity; }

    bool busy() const { return m_busy; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setProfile(const QString &mode);
    Q_INVOKABLE void setConservation(bool enabled);

signals:
    void statusChanged();
    void busyChanged();
    void operationFinished(bool success, const QString &message);

private:
    void execute(const QStringList &arguments);

    QString m_profile = "Loading";
    QString m_conservation = "Loading";
    QString m_temperature = "Not available";
    QString m_memory = "Not available";
    QString m_battery = "Not available";


    QString m_cpuModel = "Not available";
    QString m_cpuUsage = "Not available";
    QString m_intelGpuModel = "Not available";
    QString m_intelGpuFrequency = "Not available";
    QString m_intelGpuMaxFrequency = "Not available";
    QString m_gpuModel = "Not available";
    QString m_gpuTemperature = "Not available";
    QString m_gpuUsage = "Not available";
    QString m_gpuMemory = "Not available";
    QString m_batteryHealth = "Not available";
    QString m_batteryCycles = "Not available";
    QString m_batteryFullCapacity = "Not available";
    QString m_batteryDesignCapacity = "Not available";

    quint64 m_previousCpuTotal = 0;
    quint64 m_previousCpuIdle = 0;

    bool m_busy = false;
    QProcess *m_process = nullptr;
};
