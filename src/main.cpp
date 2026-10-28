#include <Arduino.h>

// Green LED is active-low on P1.03 (PIN_LED1 defined in variant.h)

void setup()
{
    Serial.begin(115200);
    pinMode(PIN_LED1, OUTPUT);
    digitalWrite(PIN_LED1, HIGH); // off
    Serial.println("Heltec T114 ready");
}

void loop()
{
    digitalWrite(PIN_LED1, LOW);  // on
    delay(100);
    digitalWrite(PIN_LED1, HIGH); // off
    delay(2900);
    Serial.println("hello");
}
