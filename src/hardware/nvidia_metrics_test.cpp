#include "NvidiaMetrics.h"

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    const auto valid = parseNvidiaMetrics(
        "NVIDIA GeForce GTX 1650, 50, 0, 12, 4096");

    if (valid.model != "NVIDIA GeForce GTX 1650" ||
        valid.temperature != "50 °C" ||
        valid.usage != "0%" ||
        valid.memory != "12 / 4096 MiB")
        return 1;

    const auto invalid = parseNvidiaMetrics(
        "NVIDIA GPU, N/A, N/A, N/A, N/A");

    if (invalid.temperature != "Not available" ||
        invalid.usage != "Not available" ||
        invalid.memory != "Not available")
        return 2;

    const auto negative = parseNvidiaMetrics(
        "NVIDIA GPU, -5, -1, -10, 4096");

    if (negative.temperature != "Not available" ||
        negative.usage != "Not available" ||
        negative.memory != "Not available")
        return 3;

    const auto empty = parseNvidiaMetrics("");

    if (empty.model != "Not available" ||
        empty.usage != "Not available")
        return 4;

    qInfo() << "NVIDIA metrics regression tests passed";
    return 0;
}
