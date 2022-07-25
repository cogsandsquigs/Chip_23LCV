SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "Chip_23LCV1024.h"

Chip_23LCV1024 Memory;

void setup()
{
    Serial.begin(9600);
    Memory.begin(A5);
}

void loop()
{
    delay(1000);

    int addr = 0x00000;

    byte data[4] = {0x1, 0x2, 0x3, 0x4};

    Memory.write(addr, data, 4);

    byte *data2 = new byte[4];
    Memory.read(addr, data2, 4);
    Serial.print("Read: ");
    Serial.print(data2[0], HEX);
    Serial.print(", ");
    Serial.print(data2[1], HEX);
    Serial.print(", ");
    Serial.print(data2[2], HEX);
    Serial.print(", ");
    Serial.println(data2[3], HEX);
}