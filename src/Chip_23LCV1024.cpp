#include "Chip_23LCV1024.h"
#include "Particle.h"

#define CHIP_SIZE 0x1FFFF

Chip_23LCV1024::Chip_23LCV1024() {}

void Chip_23LCV1024::begin(pin_t cs1)
{
    this->cs = new pin_t[1]{cs1};
    cs_num = 1;
    pinMode(cs1, OUTPUT);
    digitalWrite(cs1, HIGH);
}

void Chip_23LCV1024::begin(pin_t cs1, pin_t cs2)
{
    this->cs = new pin_t[2]{cs1, cs2};
    cs_num = 2;
    pinMode(cs1, OUTPUT);
    pinMode(cs2, OUTPUT);
    digitalWrite(cs1, HIGH);
    digitalWrite(cs2, HIGH);
}

void Chip_23LCV1024::write(uint32_t address, byte *data, int length)
{
    /**
     * Write to the specified address. If the data is longer than a single
     * 23LCV1024 chip, write to the next chip in `cs`. If the address is
     * greater than the end of the chip, write to the next chip in `cs`.
     */

    // Calculate the starting chip index.
    uint32_t chip = address / CHIP_SIZE;
    uint32_t remaining_bytes = length;

    // Calculate how many chips we need to write to.
    uint32_t chip_count = ((address % CHIP_SIZE) + length) / CHIP_SIZE + 1;
    if (chip_count > cs_num)
    {
        chip_count = cs_num;
    }

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
        write_single_chip(i, chip_address, chip_data, data_length);

        remaining_bytes -= (CHIP_SIZE - chip_address);
    }
}

void Chip_23LCV1024::write_byte(uint32_t address, byte data)
{
    // Manually doing this because this improves performance.
    uint32_t chip = address / CHIP_SIZE;
    uint32_t chip_address = address % CHIP_SIZE;
    byte chip_data[1] = {data};
    write_single_chip(chip, chip_address, chip_data, 1);
}

void Chip_23LCV1024::write_single_chip(uint32_t chip, uint32_t address, byte data[], uint length)
{
    byte writing[length + 4];

    writing[0] = 0x02;                         // write command
    writing[1] = (byte)(address >> 16) & 0xFF; // address high
    writing[2] = (byte)(address >> 8) & 0xFF;  // address mid
    writing[3] = (byte)(address)&0xFF;         // address low
    for (uint i = 0; i < length; i++)
    {
        writing[i + 4] = data[i];
    }

    SPI.begin(cs[chip]);
    SPI.beginTransaction();
    SPI.setDataMode(SPI_MODE0);
    digitalWrite(cs[chip], LOW);
    SPI.transfer(writing, NULL, length + 4, nullptr);
    digitalWrite(cs[chip], HIGH);
    SPI.endTransaction();
    SPI.end();
}

void Chip_23LCV1024::read(uint32_t address, byte *(&data), int length)
{
    /**
     * Read from the specified address. If the data is longer than a single
     * 23LCV1024 chip, read from the next chip in `cs`. If the address is
     * greater than the end of the chip, read from the next chip in `cs`.
     */

    // Calculate the starting chip index.
    uint32_t chip = address / CHIP_SIZE;
    uint32_t remaining_bytes = length;

    // Calculate how many chips we need to read from.
    uint32_t chip_count = ((address % CHIP_SIZE) + length) / CHIP_SIZE + 1;
    if (chip_count > cs_num)
    {
        chip_count = cs_num;
    }

    byte read[length];

    // Read from each chip.
    for (uint32_t i = chip; i < (chip + chip_count); i++)
    {
        // Calculate the address to read from.
        uint32_t chip_address = address % CHIP_SIZE;

        // Calculate the data to read.
        uint32_t data_start = length - remaining_bytes;
        uint32_t data_length = min((CHIP_SIZE - chip_address), remaining_bytes);

        // Read from the chip.
        read_single_chip(i, chip_address, read, data_length);

        // Read data into the buffer.
        for (uint32_t j = 0; j < data_length; j++)
        {
            data[data_start + j] = read[j];
        }

        remaining_bytes -= (CHIP_SIZE - chip_address);
    }
}

byte Chip_23LCV1024::read_byte(uint32_t address)
{
    uint32_t chip = address / CHIP_SIZE;
    uint32_t chip_address = address % CHIP_SIZE;
    byte data[1];

    read_single_chip(chip, chip_address, data, 1);
    return data[0];
}

void Chip_23LCV1024::read_single_chip(uint32_t chip, uint32_t address, byte (&data)[], uint length)
{
    SPI.begin(cs[chip]);
    SPI.setDataMode(SPI_MODE0);

    digitalWrite(cs[chip], LOW);

    SPI.transfer(0x03); // Read command

    // add the address to the data
    SPI.transfer((address >> 16) & 0xFF);
    SPI.transfer((address >> 8) & 0xFF);
    SPI.transfer(address & 0xFF);

    SPI.transfer(NULL, data, length, nullptr);

    digitalWrite(cs[chip], HIGH);

    SPI.end();
}