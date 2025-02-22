#include "PPU.h"
#include "GameBoy.h"

PPU::PPU(GameBoy* gb)
{
    this->SwitchMode(2);
}

PPU::~PPU()
{

}

void PPU::Tick(uint8_t cycles)
{
    this->internal_clock += cycles * 4;

    if(this->mode == 2)
    {
        // Scan OAM
        if(this->internal_clock >= pause_time)
        {
            this->internal_clock -= pause_time;
            this->SwitchMode(3);
        }    
    }
}

uint8_t PPU::ReadVRAM(uint16_t address)
{
    assert(address >= 0x8000 && address <= 0x9FFF);

    return this->vram[address - 0x8000];
}

void PPU::WriteVRAM(uint16_t address, uint8_t data)
{
    assert(address >= 0x8000 && address <= 0x9FFF);

    this->vram[address - 0x8000] = data;
}

bool PPU::IsVRAMAcessible()
{
    return mode != 3;
}

uint8_t PPU::ReadOAM(uint16_t address)
{
    assert(address >= 0xFE00 && address <= 0xFE9F);

    return this->oam[address - 0xFE00];
}

void PPU::WriteOAM(uint16_t address, uint8_t data)
{
    assert(address >= 0xFE00 && address <= 0xFE9F);

    this->oam[address - 0xFE00] = data;
}

uint16_t PPU::GetTile(uint8_t id, bool obj)
{
    uint8_t LCDC = this->gb->mmu->Read(0xFF40);

    if(obj || LCDC && 0x10)
    {
        // 0x8000 method
        return (0x8000 + id * 0x10);
    }
    else if(LCDC && 0x10 == 0)
    {
        // 0x8800 method 
        if(id >= 128 && id < 256)
        {
            return (0x8800 + (id-128) * 0x10);
        }
        else
        {
            return (0x9000 + id * 0x10);
        }
    }
}

void PPU::SwitchMode(uint8_t m)
{
    uint8_t ly = this->gb->mmu->Read(0xFF44);
    uint8_t lcd_control = this->gb->mmu->Read(0xFF40);

    if(m == 2) // OAM Scan
    {
        uint8_t sprite_height = (lcd_control & 0x04) ? 16 : 8;

        this->pause_time = 80;  

        // scan for objects on this scanline
        this->object_count = 0;
        for(int i = 0; i < 40; i++)
        {
            if(this->object_count > 10)
            {
                break;
            }

            uint16_t address = 0xFE00 + 0x04 * i;
            
            Sprite& sp = this->object_buffer[this->object_count];

            sp.y_pos = this->ReadOAM(address);
            sp.x_pos = this->ReadOAM(address + 1);
            sp.tile = this->ReadOAM(address + 2);
            sp.flags = this->ReadOAM(address + 3);

            if(sp.x_pos == 0 || ly + 16 < sp.y_pos || ly + 16 >= sp.y_pos + sprite_height)
            {
                return;
            }

            this->object_count++;
        }  
    }
    
    this->mode = m;
}