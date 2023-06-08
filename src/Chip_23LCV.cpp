#include "Chip_23LCV.h"
#include "Particle.h"

#define CHIP_SIZE 0x1FFFF

Chip_23LCV::Chip_23LCV() {}

void Chip_23LCV::begin(pin_t chip1)
{
	uint32_t chip_size;
	uint32_t addr_size;
	detect_chip_size(chip1, &chip_size, &addr_size);

	this->chips = new Chip[1]{Chip(chip1, chip_size, addr_size)};
	chip_count = 1;

	pinMode(chip1, OUTPUT);

	digitalWrite(chip1, HIGH);
}

void Chip_23LCV::begin(pin_t chip1, pin_t chip2)
{
	uint32_t chip1_size;
	uint32_t addr1_size;
	detect_chip_size(chip1, &chip1_size, &addr1_size);

	uint32_t chip2_size;
	uint32_t addr2_size;
	detect_chip_size(chip2, &chip2_size, &addr2_size);

	this->chips = new Chip[2]{Chip(chip1, chip1_size, addr1_size),
							  Chip(chip2, chip2_size, addr2_size)};
	chip_count = 2;

	pinMode(chip1, OUTPUT);
	pinMode(chip2, OUTPUT);

	digitalWrite(chip1, HIGH);
	digitalWrite(chip2, HIGH);
}

void Chip_23LCV::write(uint32_t address, byte *data, int length)
{
	/**
	 * Write to the specified address. If the data is longer than a single
	 * 23LCV1024 chip, write to the next chip in `chips`. If the address is
	 * greater than the end of the chip, write to the next chip in `chips`.
	 */

	// Calculate the starting chip index.
	uint32_t chip = get_chip_index(address);
	uint32_t remaining_bytes = length;

	// Calculate how many chips we need to write to.
	uint32_t chip_count = ((address % CHIP_SIZE) + length) / CHIP_SIZE + 1;

	// Write to each chip.
	for (uint32_t i = chip; i < (chip + chip_count); i++)
	{
		// Calculate the address to write to.
		uint32_t chip_address = address % CHIP_SIZE;

		// Calculate the data to write.
		uint32_t data_start = length - remaining_bytes;
		uint32_t data_length = min((CHIP_SIZE - chip_address), remaining_bytes);

		// Collect the data.
		byte chip_data[data_length];

		for (uint32_t j = 0; j < data_length; j++)
		{
			chip_data[j] = data[data_start + j];
		}

		// Write to the chip.
		write_single_chip(i, chip_address, 4, chip_data, data_length);

		remaining_bytes -= (CHIP_SIZE - chip_address);
	}
}

void Chip_23LCV::write_byte(uint32_t address, byte data)
{
	// Manually doing this because this improves performance.
	uint32_t chip = get_chip_index(address);
	uint32_t chip_address = address % this->chips[chip].chip_size;

	byte chip_data[1] = {data};
	write_single_chip(this->chips[chip].pin, chip_address,
					  this->chips[chip].address_size, chip_data, 1);
}

void Chip_23LCV::read(uint32_t address, byte *(&data), int length)
{
	/**
	 * Read from the specified address. If the data is longer than a single
	 * 23LCV1024 chip, read from the next chip in `chips`. If the address is
	 * greater than the end of the chip, read from the next chip in `chips`.
	 */

	// Calculate the starting chip index.
	uint32_t chip = get_chip_index(address);
	uint32_t remaining_bytes = length;

	// Calculate how many chips we need to read from.
	uint32_t chip_count = get_chip_count(address, chip, length);

	byte read[length];

	// Read from each chip.
	for (uint32_t i = chip; i < (chip + chip_count); i++)
	{
		// Get the size of the chip.
		uint32_t chip_size = this->chips[chip].chip_size;
		// Calculate the address to read from.
		uint32_t chip_address = address % chip_size;

		// Calculate the data to read.
		uint32_t data_start = length - remaining_bytes;
		uint32_t data_length = min((chip_size - chip_address), remaining_bytes);
		// Read from the chip.
		read_single_chip(this->chips[i].pin, chip_address, this->chips[i].address_size, read, data_length);

		// Read data into the buffer.
		for (uint32_t j = 0; j < data_length; j++)
		{
			data[data_start + j] = read[j];
		}

		remaining_bytes -= (chip_size - chip_address);
	}
}

byte Chip_23LCV::read_byte(uint32_t address)
{
	uint32_t chip = get_chip_index(address);
	uint32_t chip_address = address % this->chips[chip].chip_size;
	byte data[1];

	read_single_chip(chip, chip_address, this->chips[chip].address_size, data, 1);
	return data[0];
}

/* Private API */

void Chip_23LCV::detect_chip_size(pin_t pin, uint32_t *chip_size,
								  uint32_t *address_size)
{
	uint32_t test_addr = 0x00FF;
	byte test_data[1] = {(byte)random()};

	// Check to see if it's a 23LCV1024 chip. If we can write to it with 4 address
	// bytes, then it's a 23LCV1024 chip. Otherwise, it's a 23LCV512 chip. Write
	// to the chip.
	write_single_chip(pin, test_addr, 3, test_data, 1);

	// Read the data from the chip, at 0x0000.
	byte data[2];
	read_single_chip(pin, 0x0000, 2, data, 2);

	// Check if the data is correct.
	// It's a 23LCV512 chip if we read the first byte as the last byte of the
	// address and the second byte as the test data.
	if (data[0] == 0xFF && data[1] == test_data[0])
	{
		*chip_size = 0x0FFFF;
		*address_size = 2;
	}
	// It's a 23LCV1024 chip.
	else
	{
		*chip_size = 0x1FFFF;
		*address_size = 3;
	}
}

uint32_t Chip_23LCV::get_chip_index(uint32_t address)
{
	// The total amount of memory we have looked through.
	uint32_t total_memory = 0;

	for (uint32_t i = 0; i < chip_count; i++)
	{
		// Add the chip size to the total memory, so we can check if the address is
		// greater than the total memory so far.
		total_memory += chips[i].chip_size;

		// If we have found a chip where the address is less than the total memory,
		// then we have found the chip we want.
		if (address < chips[i].chip_size)
		{
			return i;
		}

		// If the address is greater than the total memory, then we need to keep
		// searching!
	}

	// If we have gotten here, then we have a problem.
	// This should NEVER happen!!!
	return -1;
}

uint32_t Chip_23LCV::get_chip_count(uint32_t address, uint32_t chip_index,
									uint32_t length)
{
	// The total amount of memory that we have searched through so far.
	uint32_t total_memory = 0;

	for (uint32_t i = chip_index; i < chip_count; i++)
	{
		// Add the chip size to the total memory, so we can check if the address is
		// greater than the total memory so far.
		total_memory += chips[i].chip_size;

		// If this is the first chip, subtract the address from the total memory because we
		// don't want to count memory before the address.
		if (i == chip_index)
		{
			total_memory -= address;
		}

		// If the length is greater than the total memory, then we need to keep
		// searching!
		if (length > total_memory)
		{
			length -= total_memory;
			continue;
		}
		// Otherwise, we have found the chip we want.
		else
		{
			return i - chip_index + 1; // Inclusive of the current chip.
		}

		// If the address is greater than the total memory, then we need to keep
		// searching!
	}

	// If we have gotten here, then we have a problem.
	// This should NEVER happen!!!
	return -1;
}

void Chip_23LCV::write_single_chip(pin_t pin, uint32_t address,
								   uint32_t address_size, byte data[],
								   uint length)
{

	byte writing[length + address_size + 1];

	writing[0] = 0x02; // write command
	// Write the address to the data
	for (uint i = 0; i < address_size; i++)
	{
		// 8 * (address_size - i - 1) is the number of bits
		// to shift, so that we can get the most significant
		// 8 bits, then the next 8 bits, etc.
		writing[i + 1] = (address >> (8 * (address_size - i - 1))) & 0xFF;
	}

	// Add the data to write.
	for (uint i = 0; i < length; i++)
	{
		// +1 for the command byte.
		writing[i + address_size + 1] = data[i];
	}

	Serial.print("Writing to chip: ");
	for (uint i = 0; i < length + address_size + 1; i++)
	{
		Serial.print(writing[i], HEX);
		Serial.print(" ");
	}
	Serial.println("");

	SPI.begin(pin);
	SPI.beginTransaction();
	SPI.setDataMode(SPI_MODE0);

	digitalWrite(pin, LOW);

	SPI.transfer(writing, NULL, length + address_size + 1, nullptr);

	digitalWrite(pin, HIGH);

	SPI.endTransaction();
	SPI.end();
}

void Chip_23LCV::read_single_chip(pin_t pin, uint32_t address,
								  uint32_t address_size, byte (&data)[],
								  uint length)
{
	SPI.begin(pin);
	SPI.setDataMode(SPI_MODE0);

	digitalWrite(pin, LOW);

	SPI.transfer(0x03); // Read command

	// Write the address to the data
	for (uint32_t i = 0; i < address_size; i++)
	{
		// 8 * (address_size - i - 1) is the number of bits
		// to shift, so that we can get the most significant
		// 8 bits, then the next 8 bits, etc.
		// +1 for the command byte.
		SPI.transfer((address >> (8 * (address_size - i - 1))) & 0xFF);
	}

	// Read the incoming data from the chip.
	SPI.transfer(NULL, data, length, nullptr);

	digitalWrite(pin, HIGH);

	SPI.end();
}
