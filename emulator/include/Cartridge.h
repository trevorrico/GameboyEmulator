#ifndef CARTRIDGE_H_
#define CARTRIDGE_H_

#include <stdint.h>
#include <string>
#include <iostream>
#include <cstring>
#include <assert.h>

#include <portable-file-dialogs.h>

enum CartridgeType
{
	ROM_ONLY,
	MBC1_TYPE,
	MBC2_TYPE,
	MBC3_TYPE,
	MBC5_TYPE,
	MBC6_TYPE,
	MBC7_TYPE
};

enum ROMSize
{
	ROM_32KB = 0x00,
	ROM_64KB = 0x01,
	ROM_128KB = 0x02,
	ROM_256KB = 0x03,
	ROM_512KB = 0x04,
	ROM_1MB = 0x05,
	ROM_2MB = 0x06,
	ROM_4MB = 0x07,
	ROM_8MB = 0x08,
	ROM_1_1MB = 0x52,
	ROM_1_2MB = 0x53,
	ROM_1_5MB = 0x54
};

enum RAMSize
{
	RAM_NONE = 0x00,
	RAM_8KB = 0x02,
	RAM_32KB = 0x03,
	RAM_128KB = 0x04,
	RAM_64KB = 0x05
};

struct CartridgeHeader
{
	char title[16];
	uint8_t nintendo_logo[48];
	uint8_t licensee_code;
	uint8_t sgb_flag;
	uint8_t destination_code;
	uint8_t old_licensee_code;
	uint8_t mask_rom_version;
	uint8_t header_checksum;
	uint16_t global_checksum;
	
	CartridgeType cartridge_type;
	ROMSize rom_size;
	RAMSize ram_size;
};

class Mapper
{
public:
	Mapper(CartridgeHeader& header);
	virtual ~Mapper();

	virtual uint8_t ReadROM(uint16_t address) = 0;
	virtual void WriteROM(uint16_t address, uint8_t value) = 0;

	virtual uint8_t ReadRAM(uint16_t address) = 0;
	virtual void WriteRAM(uint16_t address, uint8_t value) = 0;

	uint8_t* rom;
	uint8_t* ram;

	size_t allocated_ram_size;
	size_t allocated_rom_size;

	uint8_t rom_bank_count;
	uint8_t ram_bank_count;
private:

};

class NoMBC : public Mapper
{
public:
	NoMBC(CartridgeHeader& header);
	~NoMBC();

	uint8_t ReadROM(uint16_t address) override;
	void WriteROM(uint16_t address, uint8_t value) override;

	uint8_t ReadRAM(uint16_t address) override;
	void WriteRAM(uint16_t address, uint8_t value) override;
private:

};

class MBC1 : public Mapper
{
public:
	MBC1(CartridgeHeader& header);
	~MBC1();

	uint8_t ReadROM(uint16_t address) override;
	void WriteROM(uint16_t address, uint8_t value) override;

	uint8_t ReadRAM(uint16_t address) override;
	void WriteRAM(uint16_t address, uint8_t value) override;
private:
	uint8_t rom_bank_number = 1;
	uint8_t ram_bank_number = 0;
	uint8_t banking_mode = 0;
	bool ram_enabled = false;
};

class Cartridge
{
public:
	Cartridge();
	~Cartridge();

	bool LoadROM(std::string path, uint8_t* rom, size_t data_size);
	void UnloadBootrom();

	uint8_t ReadROM(uint16_t address);
	void WriteROM(uint16_t address, uint8_t value);

	uint8_t ReadRAM(uint16_t address);
	void WriteRAM(uint16_t address, uint8_t value);

	std::string path;
	CartridgeHeader header = {0};
private:
	Mapper* active_mapper = nullptr;
	uint8_t stored_rom_binarys[256];

	bool LoadCartridgeHeader(uint8_t* rom);
};

#endif