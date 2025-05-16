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

        // Get current thread ID
        auto thread_id = ThisThread::get_id();

        printf("[%-20s:%-4d] %-20s [TID: 0x%08x]: ", file, line, func, reinterpret_cast<uint32_t>(thread_id));

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

