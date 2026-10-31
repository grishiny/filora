#pragma once

#include <Arduino.h>

// Обёртка над USB-CDC Serial (COM7 при работе).
class Console
{
public:
    // Открыть порт на 115200 бод.
    static void init();

    // Логирование.
    static void println(const String &s);
    static void println(const char *s);
};
