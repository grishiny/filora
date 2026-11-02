#include "lora.h"

// Обработчик DIO1: срабатывает при завершении отправки (TX_DONE)
// или при приёме пакета (RX_DONE). Только флаг — вся работа в loop().
namespace {
volatile bool opDone = false;
}

void loraDio1Handler(void)
{
    opDone = true;
}

Lora lora;

bool Lora::init(float freqMHz, int8_t powerDbm)
{
    radio = new SX1262(&mod);

    // LoRa: BW 125 kHz, SF9, CR 4/7 — стандартные параметры RadioLib.
    // tcxoVoltage = 1.8 V — значение платы (SX126X_DIO3_TCXO_VOLTAGE,
    // variant.h); begin() сам включает DIO2 как RF-коммутатор и
    // настраивает TCXO.
    state_ = radio->begin(freqMHz, 125.0f, 9, 7,
                          RADIOLIB_SX126X_SYNC_WORD_PRIVATE,
                          powerDbm, 8, 1.8f);
    if (state_ != RADIOLIB_ERR_NONE)
        return false;

    radio->setDio1Action(loraDio1Handler);

    state_ = radio->startReceive();
    inited_ = (state_ == RADIOLIB_ERR_NONE);
    return inited_;
}

bool Lora::send(String &msg)
{
    if (!radio || !inited_)
        return false;

    state_ = radio->startTransmit(msg);
    if (state_ == RADIOLIB_ERR_NONE)
    {
        txActive_ = true;
        return true;
    }

    // Не удалось начать передачу — возвращаемся в приём.
    txActive_ = false;
    state_ = radio->startReceive();
    return false;
}

void Lora::poll()
{
    if (!opDone || !radio)
        return;
    opDone = false;

    if (txActive_)
    {
        // Отправка завершена — снова слушаем эфир.
        txActive_ = false;
        state_ = radio->startReceive();
    }
    else
    {
        // Получен пакет.
        rxReady_ = true;
    }
}

bool Lora::txBusy()
{
    return txActive_;
}

int16_t Lora::txState()
{
    return state_;
}

bool Lora::rxAvailable()
{
    bool ready = rxReady_;
    rxReady_ = false;
    return ready;
}

String Lora::rxRead()
{
    String s;
    if (radio)
        radio->readData(s);
    return s;
}

float Lora::rssi()
{
    return radio ? radio->getRSSI() : 0.0f;
}

float Lora::snr()
{
    return radio ? radio->getSNR() : 0.0f;
}

int16_t Lora::lastState()
{
    return state_;
}
