#pragma once

#include <Arduino.h>
#include <RadioLib.h>

// SX1262 (RadioLib 7.6.0) на Heltec Mesh Node T114.
// Пины из variants/heltec_mesh_node_t114/variant.h:
//   CS   = P0.24, DIO1 = P0.20 (IRQ), RST = P0.25, BUSY = P0.17
//   DIO2 — RF-коммутатор (внутри чипа), DIO3 — питание TCXO, 1.8 V
//   SPI0 (по умолчанию): SCK = P0.19, MISO = P0.23, MOSI = P0.22
// Режим работы: непрерывный приём; DIO1 прерывает при TX_DONE / RX_DONE.
class Lora
{
public:
    // Инициализировать SX1262 (LoRa) и начать непрерывный приём.
    // freqMHz — несущая частота, по умолчанию 433.0 МГц (Россия).
    // powerDbm — мощность передатчика, по умолчанию 10 dBm.
    bool init(float freqMHz = 433.0f, int8_t powerDbm = 10);

    // Начать отправку пакета (неблокирующая).
    // Результат завершившейся отправки будет в txState().
    bool send(String &msg);

    // Обработать флаг прерывания DIO1 — вызывать из loop().
    // После завершившейся отправки радио автоматически возвращается в приём.
    void poll();

    // --- Передача ---
    bool txBusy();     // идёт ли сейчас отправка
    int16_t txState(); // результат последней отправки (RADIOLIB_ERR_NONE = OK)

    // --- Приём ---
    bool rxAvailable(); // пакет получен и ждёт чтения (сбрасывает флаг)
    String rxRead();    // прочитать принятый пакет (текст)
    float rssi();       // RSSI последнего принятого пакета, dBm
    float snr();        // SNR последнего принятого пакета, dB

    // Последний код состояния RadioLib (для диагностики).
    int16_t lastState();

private:
    Module mod = Module(SX126X_CS, SX126X_DIO1, SX126X_RESET, SX126X_BUSY);
    SX1262 *radio = nullptr;
    bool inited_ = false;
    int16_t state_ = RADIOLIB_ERR_NONE;
    bool txActive_ = false; // идёт отправка
    bool rxReady_ = false;  // пакет ждёт чтения
};

// Один радиоинтерфейс на устройство.
extern Lora lora;
