#include <Arduino.h>

#include "console.h"
#include "display.h"
#include "led.h"
#include "lora.h"

void setup()
{
    Console::init();
    Led::init();

    Console::println("Initializing TFT (ST7789)...");
    if (!display.init()) {
        Console::println("TFT init() failed");
        return;
    }
    Console::println("TFT init OK");

    Console::println("Initializing LoRa (SX1262)...");
    if (lora.init()) {
        Console::println("LoRa init OK (433 MHz, 10 dBm)");
    } else {
        Console::println("LoRa init FAILED, code " + String(lora.lastState()));
    }

    // Консольный вывод: строки появляются друг за другом и скроллятся.
    display.consoleClear();
    display.consolePrintln("Hello World!");
    display.consolePrintln("TFT 240x135 OK");
    display.consolePrintln("Console ready");
}

void loop()
{
    Led::blink(); // зелёный LED: 100 мс вкл / 900 мс выкл

    // Обработка прерываний SX1262 (приём/отправка).
    lora.poll();

    // Принятый пакет — выводим в консоль на TFT.
    if (lora.rxAvailable()) {
        display.consolePrintln("RX: " + lora.rxRead() + "  [" +
                               String(lora.rssi(), 1) + " dBm, " +
                               String(lora.snr(), 1) + " dB]");
    }

    // Каждые 3 с отправляем пакет — для проверки связи со второй T114.
    static uint32_t lastTx = 0;
    uint32_t now = millis();
    if (now - lastTx >= 3000) {
        lastTx = now;
        String msg = "PING " + String(now / 1000);
        if (lora.send(msg))
            display.consolePrintln("TX: " + msg);
    }
}
