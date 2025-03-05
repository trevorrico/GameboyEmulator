#include "Cartridge.h"

Mapper::Mapper(CartridgeHeader& header)
{
	// Get rom and ram size
	uint64_t rom_size = 0x8000 * (1 << header.rom_size);

	uint64_t ram_size = 0;
	switch (header.ram_size)
	{
	case RAM_NONE: ram_size = 0; break;
	case RAM_8KB: ram_size = 8196; break;
	case RAM_32KB: ram_size = 32768; break;
	case RAM_128KB: ram_size = 131072; break;
	case RAM_64KB: ram_size = 65536; break;
	}

	// Get the amount of banks it is needed for each
	this->rom_bank_count = (rom_size / 0x4000);
	this->ram_bank_count = ram_size / 0x2000;

	// Allocate enough rom
	this->rom = new uint8_t[rom_size];
	this->ram = new uint8_t[ram_size];

	// Reset ram to 0
	memset(this->rom, 0, rom_size);

	// Reset ram to 0
	memset(this->ram, 0, ram_size);

	this->allocated_rom_size = rom_size;
	this->allocated_ram_size = ram_size;
}

Mapper::~Mapper()
{
	delete[] this->rom;
	delete[] this->ram;
}

NoMBC::NoMBC(CartridgeHeader& header) : Mapper(header)
{

}

NoMBC::~NoMBC()
{

}

uint8_t NoMBC::ReadROM(uint16_t address)
{
	return this->rom[address];
}

void NoMBC::WriteROM(uint16_t address, uint8_t value)
{
	
}

uint8_t NoMBC::ReadRAM(uint16_t address)
{
	return this->ram[address];
}

void NoMBC::WriteRAM(uint16_t address, uint8_t value)
{
	this->ram[address] = value;
}

MBC1::MBC1(CartridgeHeader& header) : Mapper(header)
{

}

MBC1::~MBC1()
{

}

uint8_t MBC1::ReadROM(uint16_t address)
{
	if (address <= 0x3FFF)
	{
		return this->rom[address + 0x4000 * (this->ram_bank_number << 5) * this->banking_mode];
	}
	else
	{
		return this->rom[address + 0x4000 * ((this->rom_bank_number | (this->ram_bank_number << 5)) - 1)];
	}
}

void MBC1::WriteROM(uint16_t address, uint8_t value)
{
	if (address <= 0x1FFF)
	{
		if ((value & 0x0F) == 0xA)
		{
			this->ram_enabled = true;
		}
		else
		{
			this->ram_enabled = false;
		}
	}
	else if(address <= 0x3FFF)
	{
		this->rom_bank_number = value & 0x1F;
		if (this->rom_bank_number == 0)
		{
			this->rom_bank_number = 1;
		}
		else if (this->rom_bank_number == 0x20)
		{
			this->rom_bank_number = 0x21;
		}
		else if (this->rom_bank_number == 0x40)
		{
			this->rom_bank_number = 0x41;
		}
		else if (this->rom_bank_number == 0x60)
		{
			this->rom_bank_number = 0x61;
		}
	}
	else if (address <= 0x5FFF)
	{
		this->ram_bank_number = value & 0x03;
	}
	else
	{
		this->banking_mode = value & 0x01;
	}
}

uint8_t MBC1::ReadRAM(uint16_t address)
{
	if (this->ram_enabled)
	{
		uint16_t rel_address = address - 0xA000;
		uint8_t bank = this->banking_mode * this->ram_bank_number;

		return this->ram[rel_address + 0x2000 * bank];
	}
	return 0xFF;
}

void MBC1::WriteRAM(uint16_t address, uint8_t value)
{
	if (this->ram_enabled)
	{
		uint16_t rel_address = address - 0xA000;
		uint8_t bank = this->banking_mode * this->ram_bank_number;

		this->ram[rel_address + 0x2000 * bank] = value;
	}
}

Cartridge::Cartridge()
{
	
}

Cartridge::~Cartridge()
{
	delete this->active_mapper;
}

bool Cartridge::LoadROM(std::string path, uint8_t* rom, size_t data_size)
{
	this->path = path;

	bool result = LoadCartridgeHeader(rom);
	if (result)
	{
		if (data_size % 16384 != 0) {
			std::cout << "ROM size must be a multiple of 16 KB" << std::endl;
			return false;
		}

		delete this->active_mapper;

		// Create mapper
		switch (this->header.cartridge_type)
		{
		case MBC1_TYPE: this->active_mapper = new MBC1(this->header); std::cout << "MBC1!" << std::endl; break;
		case ROM_ONLY:
		default:
			this->active_mapper = new NoMBC(this->header);
			break;
		}

		// Copy contents to the allocated memory
		memcpy(this->active_mapper->rom, rom, data_size);
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
		header.cartridge_type = CartridgeType::MBC1_TYPE; break;
	case 0x05:
	case 0x06:
		header.cartridge_type = CartridgeType::MBC2_TYPE; break;
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		header.cartridge_type = CartridgeType::MBC3_TYPE; break;
	case 0x19:
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1D:
	case 0x1E:
		header.cartridge_type = CartridgeType::MBC5_TYPE; break;
	case 0x20:
		header.cartridge_type = CartridgeType::MBC6_TYPE; break;
	case 0x22:
		header.cartridge_type = CartridgeType::MBC7_TYPE; break;
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

	return this->active_mapper->ReadROM(address);
}

void Cartridge::WriteROM(uint16_t address, uint8_t value)
{
	assert(address <= 0x7FFF);

	this->active_mapper->WriteROM(address, value);
}

uint8_t Cartridge::ReadRAM(uint16_t address)
{
	assert(address >= 0xA000 && address <= 0xBFFF);

	if (this->active_mapper->allocated_ram_size == 0)
	{
		return 0xFF;
	}
	
	return this->active_mapper->ReadRAM(address);
}

void Cartridge::WriteRAM(uint16_t address, uint8_t value)
{
	assert(address >= 0xA000 && address <= 0xBFFF);

	if (this->active_mapper->allocated_ram_size == 0)
	{
		return;
	}

	this->active_mapper->WriteRAM(address, value);	
}