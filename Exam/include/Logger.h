#pragma once
#include <mbed.h>
#include <cstdarg>

class Logger {
private:
    Mutex mutex;
    Logger() {}

public:
    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    void LogInternal(const char *file, const char *func, int line, const char *format, ...) {
        mutex.lock();
        printf("[%-20s:%-4d] %-20s: ", file, line, func);

        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);

        printf("\n");
        mutex.unlock();
    }
    void PrintEmptyLine() {
    mutex.lock();
    printf("\n");
    mutex.unlock();
}
};

#define LOG_IF(enabled, fmt, ...) \
    do { if (enabled) Logger::Instance().LogInternal(__FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__); } while (0)

#define LINE_IF(enabled) \
    do { if (enabled) Logger::Instance().PrintEmptyLine(); } while (0)

