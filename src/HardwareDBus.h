#pragma once

#include <QObject>
#include <QString>

#include "HardwareController.h"

class HardwareDBus : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.lenovo.Control")

public:
    explicit HardwareDBus(HardwareController *hardware,
                          QObject *parent = nullptr);

public slots:
    QString profile() const;
    QString conservation() const;
    QString temperature() const;
    QString battery() const;
    QString cpuUsage() const;
    QString gpuTemperature() const;
    void refresh();
    void setProfile(const QString &mode);

private:
    HardwareController *m_hardware;
};
