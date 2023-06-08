#include "Particle.h"

class Chip_23LCV
{
public:
	/**
	 * @brief Constructs an interface to the 23LCV1024 chip.
	 */
	Chip_23LCV();

	/**
	 * @brief Initializes the chip interface and SPI
	 *
	 * @param chip1 The chip select pin to use for all of the memory addresses.
	 */
	void begin(pin_t chip1);

	/**
	 * @brief Initializes the chip interface and SPI
	 *
	 * @param chip1 The chip select pin to use for the lower half of the memory
	 * addresses.
	 * @param chip2 The chip select pin to use for the upper half of the memory
	 * addresses.
	 */
	void begin(pin_t chip1, pin_t chip2);

	/**
	 * @brief Writes bytes to the specified address.
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
	 * @brief Reads bytes from the specified address.
	 *
	 * WARNING: If you do not delete() your data array
	 * afterwards, or don't make your data array
	 * static, you will leak memory.
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
	 * @brief Detect the type of chip that is being interfaced with
	 *
	 * @param pin The pin to use for the chip select.
	 * @param chip_size The size of the chip in bytes. This is a pointer which will be set
	 * to the correct chip size, as an output.
	 * @param address_size The number of bytes in the address. This is a pointer which will
	 * be set to the correct address size, as an output.
	 */
	void detect_chip_size(pin_t pin, uint32_t *chip_size, uint32_t *address_size);

	/**
	 * @brief Gets the starting chip index for the specified address.
	 *
	 * @param address The address to get the chip index for.
	 */
	uint32_t get_chip_index(uint32_t address);

	/**
	 * @brief Calculates the amount of chips to read from for the specified address, chip index,
	 * and length.
	 *
	 * @param address The address to read from.
	 * @param chip_index The index of the chip to read from.
	 * @param length The number of bytes to read.
	 */
	uint32_t get_chip_count(uint32_t address, uint32_t chip_index, uint32_t length);

	/**
	 * @brief Writes bytes to the specified chip and address.
	 *
	 * @param pin The pin of the chip to write to.
	 * @param address The address to write to.
	 * @param address_size The number of bytes in the address.
	 * @param data The data to write.
	 * @param length The number of bytes to write.
	 */
	void write_single_chip(pin_t pin, uint32_t address, uint32_t address_size, byte data[], uint length);

	/**
	 * @brief Reads bytes from the specified chip and address.
	 *
	 * @param pin The index of the chip to read from.
	 * @param address The address to read from.
	 * @param address_size The number of bytes in the address.
	 * @param data The buffer to read the data into.
	 * @param length The number of bytes to read.
	 */
	void read_single_chip(pin_t pin, uint32_t address, uint32_t address_size, byte (&data)[], uint length);

	class Chip
	{

	public:
		/**
		 * @brief Constructs a new chip.
		 *
		 * @param chip_pin The chip select pin for this chip.
		 * @param chip_size The size of the address space of this chip.
		 * @param address_size The number of bytes in the address.
		 */
		Chip(pin_t chip_pin, uint32_t chip_size_, uint32_t address_size_)
		{
			pin = chip_pin;
			chip_size = chip_size_;
			address_size = address_size_;
		};

		/**
		 * @brief The chip select pin for this chip.
		 */
		pin_t pin;

		/**
		 * @brief The number of bytes in the address.
		 */
		uint8_t address_size;

		/**
		 * @brief The size of the address space of this chip.
		 */
		uint32_t chip_size;
	};

	/**
	 * @brief The list of chips to write to.
	 */
	Chip *chips;

	/**
	 * @brief The number of chips in `chips`.
	 */
	uint32_t chip_count;
};