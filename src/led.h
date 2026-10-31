#pragma once

#include <Arduino.h>

// Зелёный светодиод статусного индикатора (PIN_LED1, LOW = включён).
class Led
{
public:
    // Инициализация пинов (LED выключен).
    static void init();

    // Включить/выключить.
    static void set(bool on);

    // Неблокирующее мигание: вызывать из loop().
    // onMs/offMs — длительность фаз в миллисекундах.
    static void blink(uint16_t onMs = 100, uint16_t offMs = 900);
};
