#include "Particle.h"

class Chip_23LCV1024
{
public:
    /**
     * @brief Constructs an interface to the 23LCV1024 chip.
     */
    Chip_23LCV1024();

    /**
     * @brief Initializes the chip interface and SPI
     *
     * @param cs1 The chip select pin to use for all of the memory addresses.
     */
    void begin(pin_t cs1);

    /**
     * @brief Initializes the chip interface and SPI
     *
     * @param cs1 The chip select pin to use for the lower half of the memory addresses.
     * @param cs2 The chip select pin to use for the upper half of the memory addresses.
     */
    void begin(pin_t cs1, pin_t cs2);

    /**
     * @brief Writes a byte to the specified address.
     *
     * @param address The address to write to.
     * @param data The data to write.
     * @param length The number of bytes to write.
     */
    void write(uint32_t address, byte data[], int length);

    /**
     * @brief Writes a byte to the specified address.
     *
     * @param address The address to read from.
     * @param data The buffer to store the data in.
     */
    void write_byte(uint32_t address, byte data);

    /**
     * @brief Reads a byte from the specified address.
     *
     * @param address The address to read from.
     * @param data The buffer to read the data into.
     * @param length The number of bytes to read.
     */
    void read(uint32_t address, byte *(&data), int length);

    /**
     * @brief Reads a byte from the specified address.
     *
     * @param address The address to read from.
     * @returns The byte read
     */
    byte read_byte(uint32_t address);

private:
    /**
     * @brief Writes bytes to the specified chip and address.
     *
     * @param chip The index of the chip to write to.
     * @param address The address to write to.
     * @param data The data to write.
     * @param length The number of bytes to write.
     */
    void write_single_chip(uint32_t chip, uint32_t address, byte data[], uint length);

    /**
     * @brief Reads bytes from the specified chip and address.
     *
     * @param chip The index of the chip to read from.
     * @param address The address to read from.
     * @param data The buffer to read the data into.
     * @param length The number of bytes to read.
     */
    void read_single_chip(uint32_t chip, uint32_t address, byte *(&data), uint length);

    pin_t *cs;
    uint32_t cs_num;
};