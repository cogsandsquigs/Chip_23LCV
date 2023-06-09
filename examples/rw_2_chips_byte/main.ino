SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "Chip_23LCV.h"

Chip_23LCV Memory;

void setup()
{
	Serial.begin(9600);
	Memory.begin(A4);
}

void loop()
{
	delay(1000);

	int addr = 0x0000;

	byte data = (byte)random();

	Serial.print("Writing byte: ");
	Serial.println(data, HEX);

	Memory.write_byte(addr, data);

	byte received = Memory.read_byte(addr);

	Serial.print("Reading byte: ");
	Serial.println(received, HEX);
	Serial.println("");
}