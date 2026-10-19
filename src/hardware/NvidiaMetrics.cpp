#include "NvidiaMetrics.h"

#include <QStringList>
#include <cmath>

NvidiaMetrics parseNvidiaMetrics(const QString &output)
{
    NvidiaMetrics metrics;

    const QStringList values =
        output.trimmed().section('\n', 0, 0).split(',');

    if (values.size() < 5)
        return metrics;

    const auto numeric = [](const QString &value, double &result) {
        bool ok = false;
        result = value.trimmed().toDouble(&ok);
        return ok && std::isfinite(result) && result >= 0;
    };

    const QString name = values[0].trimmed();

    if (!name.isEmpty() &&
        name.compare("N/A", Qt::CaseInsensitive) != 0)
        metrics.model = name;

    double number = 0;

    if (numeric(values[1], number))
        metrics.temperature = values[1].trimmed() + " °C";

    if (numeric(values[2], number) && number <= 100)
        metrics.usage = values[2].trimmed() + "%";

    double used = 0;
    double total = 0;

    if (numeric(values[3], used) &&
        numeric(values[4], total) &&
        total > 0 && used <= total) {
        metrics.memory = values[3].trimmed() +
                         " / " + values[4].trimmed() + " MiB";
    }

    return metrics;
}
