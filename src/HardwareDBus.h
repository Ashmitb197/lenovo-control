#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

#include "HardwareController.h"

class HardwareDBus : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.lenovo.Control")

public:
    explicit HardwareDBus(HardwareController *hardware,
                          QObject *parent = nullptr);

public slots:
    bool busy() const;
    QString profile() const;
    QString conservation() const;
    QString temperature() const;
    QString battery() const;
    QString cpuUsage() const;
    QVariantMap capabilities() const;
    QString gpuTemperature() const;
    void refresh();
    void setProfile(const QString &mode);

signals:
    void operationBusyChanged(bool busy);
    void operationFinished(bool success, const QString &message);

private:
    HardwareController *m_hardware;
};
