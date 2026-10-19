#include "HardwareBridge.h"

#include <QDBusConnection>
#include <QDBusServiceWatcher>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QProcess>

HardwareBridge::HardwareBridge(QObject *parent)
    : QObject(parent),
      m_interface(
          "org.lenovo.Control",
          "/Hardware",
          "org.lenovo.Control",
          QDBusConnection::sessionBus())
{
    connect(&m_timer, &QTimer::timeout,
            this, &HardwareBridge::refresh);

    auto *serviceWatcher = new QDBusServiceWatcher(
        QStringLiteral("org.lenovo.Control"),
        QDBusConnection::sessionBus(),
        QDBusServiceWatcher::WatchForOwnerChange,
        this);

    connect(serviceWatcher,
            &QDBusServiceWatcher::serviceOwnerChanged,
            this,
            [this](const QString &,
                   const QString &,
                   const QString &newOwner) {
        ++m_serviceGeneration;
        m_capabilitiesRequestPending = false;
        m_capabilities.clear();

        if (newOwner.isEmpty()) {
            m_battery = "Unavailable";
            m_temperature = "Unavailable";
            m_cpuUsage = "Unavailable";
            m_profile = "Unavailable";
            m_conservation = "Unavailable";
            m_gpuTemperature = "Unavailable";
        }

        emit updated();

        if (!newOwner.isEmpty())
            refresh();
    });

    m_timer.start(2000);
    refresh();
}

void HardwareBridge::refreshCapabilities()
{
    if (m_capabilitiesRequestPending || !m_capabilities.isEmpty())
        return;

    m_capabilitiesRequestPending = true;
    const quint64 generation = m_serviceGeneration;

    auto *watcher = new QDBusPendingCallWatcher(
        m_interface.asyncCall("capabilities"), this);

    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, [this, watcher, generation]() {
        if (generation != m_serviceGeneration) {
            watcher->deleteLater();
            return;
        }

        m_capabilitiesRequestPending = false;

        QDBusPendingReply<QVariantMap> reply = *watcher;

        if (reply.isValid() && !reply.value().isEmpty() &&
            m_capabilities != reply.value()) {
            m_capabilities = reply.value();
            emit updated();
        }

        watcher->deleteLater();
    });
}

void HardwareBridge::refresh()
{
    refreshCapabilities();
    struct Reading {
        const char *method;
        QString HardwareBridge::*member;
    };

    static const Reading readings[] = {
        {"battery", &HardwareBridge::m_battery},
        {"temperature", &HardwareBridge::m_temperature},
        {"cpuUsage", &HardwareBridge::m_cpuUsage},
        {"profile", &HardwareBridge::m_profile},
        {"conservation", &HardwareBridge::m_conservation},
        {"gpuTemperature", &HardwareBridge::m_gpuTemperature}
    };

    const quint64 generation = m_serviceGeneration;

    for (const auto &reading : readings) {
        auto *watcher = new QDBusPendingCallWatcher(
            m_interface.asyncCall(reading.method), this);

        connect(watcher, &QDBusPendingCallWatcher::finished,
                this, [this, watcher, reading, generation]() {
            if (generation != m_serviceGeneration) {
                watcher->deleteLater();
                return;
            }

            QDBusPendingReply<QString> reply = *watcher;

            const QString value = reply.isValid()
                ? reply.value()
                : QStringLiteral("Unavailable");

            if (this->*(reading.member) != value) {
                this->*(reading.member) = value;
                emit updated();
            }

            watcher->deleteLater();
        });
    }
}

void HardwareBridge::setProfile(const QString &mode)
{
    if (mode != "low-power" &&
        mode != "balanced" &&
        mode != "performance")
        return;

    auto *watcher = new QDBusPendingCallWatcher(
        m_interface.asyncCall("setProfile", mode), this);

    const quint64 generation = m_serviceGeneration;

    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, [this, watcher, generation]() {
        QDBusPendingReply<> reply = *watcher;

        if (generation == m_serviceGeneration) {
            if (!reply.isError()) {
                refresh();
            } else {
                qWarning() << "Profile change failed:"
                           << reply.error().message();
            }
        }

        watcher->deleteLater();
    });
}

void HardwareBridge::openApplication()
{
    QProcess::startDetached("lenovo-control-native", {});
}
