#include "HardwareDBus.h"

HardwareDBus::HardwareDBus(HardwareController *hardware,
                           QObject *parent)
    : QObject(parent), m_hardware(hardware)
{
}

QString HardwareDBus::profile() const
{
    return m_hardware->profile();
}

QString HardwareDBus::conservation() const
{
    return m_hardware->conservation();
}

QString HardwareDBus::temperature() const
{
    return m_hardware->temperature();
}

QString HardwareDBus::battery() const
{
    return m_hardware->battery();
}

QString HardwareDBus::cpuUsage() const
{
    return m_hardware->cpuUsage();
}

QString HardwareDBus::gpuTemperature() const
{
    return m_hardware->gpuTemperature();
}

void HardwareDBus::refresh()
{
    m_hardware->refresh();
}

void HardwareDBus::setProfile(const QString &mode)
{
    m_hardware->setProfile(mode);
}
