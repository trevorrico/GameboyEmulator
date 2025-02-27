#include "Cartridge.h"

Cartridge::Cartridge()
{
	this->ram = nullptr;
	this->rom = nullptr;
}

Cartridge::~Cartridge()
{
	if(this->ram)
	{
		delete[] this->ram;
		delete[] this->rom;
	}
}

bool Cartridge::LoadROM(std::string path, uint8_t* rom, size_t data_size)
{
	this->path = path;

	bool result = LoadCartridgeHeader(rom);
	if(result)
	{
		if (data_size % 16384 != 0) {
			std::cout << "ROM size must be a multiple of 16 KB" << std::endl;
			return false;
		}

		// Get rom and ram size
		uint64_t rom_size = 0x8000 * (1 << this->header.rom_size);

		uint64_t ram_size = 0;
		switch(this->header.ram_size)
		{
		case RAM_NONE: ram_size = 0; break;
		case RAM_8KB: ram_size = 8196; break;
		case RAM_32KB: ram_size = 32768; break;
		case RAM_128KB: ram_size = 131072; break;
		case RAM_64KB: ram_size = 65536; break;
		}
		
		// Get the amount of banks it is needed for each
		uint8_t rom_bank_count = rom_size / 0x4000;
		uint8_t ram_bank_count = ram_size / 0x2000;
		
		// Allocate enough rom
		this->rom = new uint8_t[rom_bank_count * 0x4000];
		this->ram = new uint8_t[ram_bank_count * 0x2000];
		
		// Copy contents to the allocated memory
		memcpy(this->rom, rom, data_size);

		// Reset ram to 0
		memset(this->ram, 0, ram_bank_count * 0x2000);
		
		this->allocated_rom_size = rom_bank_count * 0x4000;
		this->allocated_ram_size = ram_bank_count * 0x2000;
	}

	return result;
}

bool Cartridge::LoadCartridgeHeader(uint8_t* rom)
{
	for(int i = 0; i < 48; i++)
	{
		header.nintendo_logo[i] = rom[0x0104 + i];
	}

	for(int i = 0; i < 16; i++)
	{
		header.title[i] = rom[0x0134 + i];
	}

	header.old_licensee_code = rom[0x014B];
	if(header.old_licensee_code == 0x33)
	{
		header.licensee_code = rom[0x0144];
	}

	header.sgb_flag = rom[0x146];

	// Get cartridge type
	switch (rom[0x147])
	{
	case 0x00: 
	case 0x08:
	case 0x09:
		header.cartridge_type = CartridgeType::ROM_ONLY; break;
	case 0x01:
	case 0x02:
	case 0x03: 
		header.cartridge_type = CartridgeType::MBC1; break;
	case 0x05:
	case 0x06:
		header.cartridge_type = CartridgeType::MBC2; break;
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		header.cartridge_type = CartridgeType::MBC3; break;
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
		header.cartridge_type = CartridgeType::MBC5; break;
	case 0x20:
		header.cartridge_type = CartridgeType::MBC6; break;
	case 0x22:
		header.cartridge_type = CartridgeType::MBC7; break;
	default:
		std::cout << "Emulator does not support this cartridge type! Using ROM Only, just hope it works" << std::endl;
		header.cartridge_type = CartridgeType::ROM_ONLY; break;
	}

	header.rom_size = (ROMSize)rom[0x148];
	header.ram_size = (RAMSize)rom[0x149];

	header.destination_code = rom[0x14A];
	
	header.mask_rom_version = rom[0x14C];
	
	uint8_t checksum = 0;
	for (uint16_t address = 0x0134; address <= 0x014C; address++) {
		checksum = checksum - rom[address] - 1;
	}

	if(checksum != rom[0x14D])
	{
		std::cout << "Warning: Header checksum is invalid" << std::endl;
		auto m = pfd::message("Header checksum invalid",
			"Header checksum is invalid.",
			pfd::choice::ok,
			pfd::icon::error);
		
		return false;
	}
	return true;
}

uint8_t Cartridge::ReadROM(uint16_t address)
{
	assert(address <= 0x7FFF);

	switch(header.cartridge_type)
	{
	case ROM_ONLY:
		return this->rom[address];
	default:
		// Still didn't implement other types
		return this->rom[address];
	}
}

void Cartridge::WriteROM(uint16_t address, uint8_t value)
{
	assert(address <= 0x7FFF);

	switch(header.cartridge_type)
	{
	case ROM_ONLY:
		// fun fact: rom isn't writeable
		break;
	default:
		// Still didn't implement other types	
		this->rom[address] = value;
	}
}

uint8_t Cartridge::ReadRAM(uint16_t address)
{
	assert(address >= 0xA000 && address <= 0xBFFF);
	
	if(allocated_ram_size == 0)
	{
		return 0x00;
	}

	switch(header.cartridge_type)
	{
	case ROM_ONLY:
		return this->ram[address - 0xA000];
	default:
		// Still didn't implement other types	
		return this->ram[address - 0xA000];
	}
	
	return 0;
}

void Cartridge::WriteRAM(uint16_t address, uint8_t value)
{
	assert(address >= 0xA000 && address <= 0xBFFF);

	if(allocated_ram_size == 0)
	{
		return;
	}

	if(this->ram)
	switch(header.cartridge_type)
	{
	case ROM_ONLY:
		this->ram[address - 0xA000] = value;
		break;
	default:
		// Still didn't implement other types	
		this->ram[address - 0xA000] = value;
	}
}