#include "HardwareBridge.h"

#include <QDBusConnection>
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

    m_timer.start(2000);
    refresh();
}

void HardwareBridge::refreshCapabilities()
{
    if (m_capabilitiesRequestPending || !m_capabilities.isEmpty())
        return;

    m_capabilitiesRequestPending = true;

    auto *watcher = new QDBusPendingCallWatcher(
        m_interface.asyncCall("capabilities"), this);

    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, [this, watcher]() {
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

    for (const auto &reading : readings) {
        auto *watcher = new QDBusPendingCallWatcher(
            m_interface.asyncCall(reading.method), this);

        connect(watcher, &QDBusPendingCallWatcher::finished,
                this, [this, watcher, reading]() {
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

    connect(watcher, &QDBusPendingCallWatcher::finished,
            watcher, &QObject::deleteLater);
}

void HardwareBridge::openApplication()
{
    QProcess::startDetached("lenovo-control-native", {});
}
