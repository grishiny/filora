#include "console.h"

void Console::init()
{
    Serial.begin(115200);
}

void Console::println(const String &s)
{
    Serial.println(s);
}

void Console::println(const char *s)
{
    Serial.println(s);
}
