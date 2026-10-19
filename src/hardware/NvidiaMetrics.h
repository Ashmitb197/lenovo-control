#pragma once

#include <QString>

struct NvidiaMetrics {
    QString model = "Not available";
    QString temperature = "Not available";
    QString usage = "Not available";
    QString memory = "Not available";
};

NvidiaMetrics parseNvidiaMetrics(const QString &output);
