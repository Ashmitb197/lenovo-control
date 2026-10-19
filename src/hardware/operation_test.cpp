#include "../HardwareController.h"

#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QFileDevice>
#include <QProcessEnvironment>
#include <QTemporaryDir>
#include <QThread>
#include <QTimer>

#include <functional>
#include <cstdio>

static bool waitUntil(const std::function<bool()> &condition,
                      int timeoutMs = 5000)
{
    QElapsedTimer timer;
    timer.start();

    while (!condition() && timer.elapsed() < timeoutMs) {
        QCoreApplication::processEvents(
            QEventLoop::AllEvents, 20);
        QThread::msleep(5);
    }

    return condition();
}

static bool createScript(const QString &path,
                         const QByteArray &contents)
{
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    if (file.write(contents) != contents.size())
        return false;

    file.close();

    return file.setPermissions(
        QFileDevice::ReadOwner |
        QFileDevice::WriteOwner |
        QFileDevice::ExeOwner);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QTemporaryDir temp;
    if (!temp.isValid())
        return 1;

    const QByteArray originalPath = qgetenv("PATH");
    const QString cli = temp.filePath("lenovo-control");

    // Controller timers and telemetry are harmless; no real CLI is invoked.
    HardwareController controller;

    int completions = 0;
    int busyChanges = 0;
    bool lastSuccess = false;
    QString lastMessage;

    QObject::connect(
        &controller, &HardwareController::operationFinished,
        &app, [&](bool success, const QString &message) {
            ++completions;
            lastSuccess = success;
            lastMessage = message;
        });

    QObject::connect(
        &controller, &HardwareController::busyChanged,
        &app, [&]() { ++busyChanges; });

    auto reset = [&]() {
        completions = 0;
        busyChanges = 0;
        lastSuccess = false;
        lastMessage.clear();
    };

    auto check = [&](bool condition, const char *description) {
        if (!condition)
            fprintf(stderr, "FAIL: %s\n", description);
        else
            printf("PASS: %s\n", description);

        return condition;
    };

    // Test 1: successful command.
    if (!createScript(cli, "#!/bin/sh\nexit 0\n"))
        return 1;

    qputenv("PATH",
            (temp.path() + ":" + QString::fromLocal8Bit(originalPath))
                .toLocal8Bit());

    reset();
    controller.setProfile("balanced");

    if (!check(waitUntil([&] { return completions == 1; }),
               "successful operation completes"))
        return 1;

    if (!check(lastSuccess && !controller.busy() &&
               completions == 1 && busyChanges == 2,
               "success produces one completion and resets busy"))
        return 1;

    // Test 2: command exits unsuccessfully.
    if (!createScript(cli,
            "#!/bin/sh\n"
            "echo simulated-error >&2\n"
            "exit 7\n"))
        return 1;

    reset();
    controller.setProfile("balanced");

    if (!check(waitUntil([&] { return completions == 1; }),
               "failed operation completes"))
        return 1;

    if (!check(!lastSuccess && !controller.busy() &&
               lastMessage.contains("simulated-error") &&
               completions == 1 && busyChanges == 2,
               "failure produces one completion and resets busy"))
        return 1;

    // Test 3: executable exists but cannot start.
    if (!createScript(cli,
            "#!/definitely-nonexistent-interpreter\n"))
        return 1;

    reset();
    controller.setProfile("balanced");

    if (!check(waitUntil([&] { return completions == 1; }),
               "startup failure completes"))
        return 1;

    // Process any subsequently queued signals.
    QCoreApplication::processEvents();

    if (!check(!lastSuccess && !controller.busy() &&
               completions == 1 && busyChanges == 2,
               "startup failure is reported exactly once"))
        return 1;

    // Test 4: overlapping operations are ignored.
    if (!createScript(cli,
            "#!/bin/sh\n"
            "sleep 1\n"
            "exit 0\n"))
        return 1;

    reset();
    controller.setProfile("balanced");
    controller.setProfile("performance");

    if (!check(controller.busy() && busyChanges == 1,
               "overlapping request does not start another process"))
        return 1;

    if (!check(waitUntil([&] { return completions == 1; }),
               "first operation completes"))
        return 1;

    if (!check(completions == 1 && !controller.busy() &&
               busyChanges == 2,
               "overlapping request produces no extra completion"))
        return 1;

    qputenv("PATH", originalPath);

    puts("All operation lifecycle tests passed.");
    return 0;
}
