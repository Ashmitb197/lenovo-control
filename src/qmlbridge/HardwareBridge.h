#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QTimer>
#include <QDBusInterface>
#include <QtQml/qqmlregistration.h>

class HardwareBridge : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QVariantMap capabilities READ capabilities NOTIFY updated)
    Q_PROPERTY(QString battery READ battery NOTIFY updated)
    Q_PROPERTY(QString temperature READ temperature NOTIFY updated)
    Q_PROPERTY(QString cpuUsage READ cpuUsage NOTIFY updated)
    Q_PROPERTY(QString profile READ profile NOTIFY updated)
    Q_PROPERTY(QString conservation READ conservation NOTIFY updated)
    Q_PROPERTY(QString gpuTemperature READ gpuTemperature NOTIFY updated)

public:
    explicit HardwareBridge(QObject *parent = nullptr);

    QVariantMap capabilities() const { return m_capabilities; }
    QString battery() const { return m_battery; }
    QString temperature() const { return m_temperature; }
    QString cpuUsage() const { return m_cpuUsage; }
    QString profile() const { return m_profile; }
    QString conservation() const { return m_conservation; }
    QString gpuTemperature() const { return m_gpuTemperature; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setProfile(const QString &mode);
    Q_INVOKABLE void openApplication();

signals:
    void updated();

private:
    void refreshCapabilities();

    quint64 m_serviceGeneration = 0;
    bool m_capabilitiesRequestPending = false;
    QDBusInterface m_interface;
    QTimer m_timer;

    QVariantMap m_capabilities;
    QString m_battery = "Loading...";
    QString m_temperature = "Loading...";
    QString m_cpuUsage = "Loading...";
    QString m_profile = "Loading...";
    QString m_conservation = "Loading...";
    QString m_gpuTemperature = "Loading...";
};
