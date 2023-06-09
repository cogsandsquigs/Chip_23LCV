SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "Chip_23LCV.h"

Chip_23LCV Memory;

void setup()
{
	Serial.begin(9600);
	Memory.begin(A5, A4);
}

byte *data2 =
	new byte[4]; // This has to be static, otherwise it will leak memory.

void loop()
{
	delay(1000);

	int addr = 0x0FFFD;

	byte data[4] = {(byte)random(), (byte)random(), (byte)random(), (byte)random()};

	Serial.print("Writing bytes: ");
	Serial.print(data[0], HEX);
	Serial.print(" ");
	Serial.print(data[1], HEX);
	Serial.print(" ");
	Serial.print(data[2], HEX);
	Serial.print(" ");
	Serial.println(data[3], HEX);

	Memory.write(addr, data, 4);

	Memory.read(addr, data2, 4);

	Serial.print("Reading bytes: ");
	Serial.print(data2[0], HEX);
	Serial.print(" ");
	Serial.print(data2[1], HEX);
	Serial.print(" ");
	Serial.print(data2[2], HEX);
	Serial.print(" ");
	Serial.println(data2[3], HEX);
	Serial.println("");
}