#include "program.h"

#include "mbed.h"
#include "alarm2.h"
#include "structs.h"
#include <cstdio>
#include "Logger.h"

constexpr bool LOG_ENABLED = true;

#define LOG(fmt, ...) LOG_IF(LOG_ENABLED, fmt, ##__VA_ARGS__)
#define LINE() LINE_IF(LOG_ENABLED)

// Velg en ledig PWM-pin, eller la være å bruke buzzer-utgangen


int main() {
    Program program;
    //program.ProgramLoop();

        // 1) Opprett data og manager
    AlarmData    data;
    AlarmManager alarm(data, D13);

    // 3) Sett test-tid: vi trigger manuelt med en egen Timeout
    // (Alternativt kan du bruke alarm.setTime() + alarm.start() for daglig schedule.)
    Timeout t;
    t.attach(callback(&alarm, &AlarmManager::triggerAlarm), 5s);

    // 4) For å teste auto-muting etter kort tid, la oss mute etter f.eks. 3s:
    Timeout t2;
    t2.attach(callback(&alarm, &AlarmManager::mute), 8s);

    printf("Testing AlarmManager: alarm will trigger in 5s, then mute in 8s\n");

    // 5) La programmet gå i loop så vi ser callback-printene
    while (true) {
        ThisThread::sleep_for(1s);
    }
}
