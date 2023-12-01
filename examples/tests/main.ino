SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "Chip_23LCV.h"

Chip_23LCV Memory;

void setup()
{
	Serial.begin(9600);
	Memory.begin(S3, S4);
	while (!Serial.isConnected())
	{
	}
	delay(100); // Sleep a small while to let the serial port catch up
}

void readAndCheckData(int address, int count, byte *expected)
{
	byte *dataAll = new byte[count];
	Memory.read(address, dataAll, count);
	for (int i = 0; i < count; i++)
	{
		if (dataAll[i] != expected[i])
		{
			Serial.printlnf("ERROR: data[%d] = 0x%02x, expected 0x%02x", i, dataAll[i], expected[i]);
		}
	}
}

void loop()
{
	test_ReadDifferentDataLengths();
	test_ReadAcrossChips();

	while (1)
	{
	}
}

void test_ReadDifferentDataLengths()
{
	Serial.println("Testing reading different data lengths...");

	int addr = 0x00100;
	byte data[] = {0x02, 0x04, 0x06, 0x08, 0x10, 0x12, 0x14, 0x16};

	Memory.write(addr, data, 8);

	readAndCheckData(addr, 8, data);
	readAndCheckData(addr, 6, data);
	readAndCheckData(addr, 4, data);
	readAndCheckData(addr, 3, data);
	readAndCheckData(addr, 2, data);
	readAndCheckData(addr, 1, data);

	Serial.println("Done!");
}

void test_ReadAcrossChips()
{
	Serial.println("Testing reading across chips...");

	int addr = 0x1FFFE;
	byte data[] = {0x02, 0x04, 0x06, 0x08, 0x10, 0x12, 0x14, 0x16};

	Memory.write(addr, data, 8);

	readAndCheckData(addr, 8, data);
	readAndCheckData(addr, 6, data);
	readAndCheckData(addr, 4, data);
	readAndCheckData(addr, 3, data);
	readAndCheckData(addr, 2, data);
	readAndCheckData(addr, 1, data);

	Serial.println("Done!");
}