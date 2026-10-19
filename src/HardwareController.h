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
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit HardwareController(QObject *parent = nullptr);

    QString profile() const { return m_profile; }
    QString conservation() const { return m_conservation; }
    QString temperature() const { return m_temperature; }
    QString memory() const { return m_memory; }
    QString battery() const { return m_battery; }
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

    bool m_busy = false;
    QProcess *m_process = nullptr;
};
