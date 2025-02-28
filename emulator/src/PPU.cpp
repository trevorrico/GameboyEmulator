#include "PPU.h"
#include "GameBoy.h"

PPU::PPU(GameBoy* gameboy)
{
    this->gb = gameboy;

    this->SwitchMode(2);
    this->fetcher_type = BACKGROUND;
}

PPU::~PPU()
{

}

void PPU::Tick(uint8_t cycles)
{
    uint8_t lcd_control = this->gb->mmu->Read(0xFF40);
    if(lcd_control & 0x80 == 0)
    {
        return; // disabled
    }

    //std::cout << this->mode << std::endl;

    this->internal_clock += cycles * 4;

    if(this->internal_clock > 0)
    {
        if(this->internal_clock >= this->pause_time)
        {
            this->internal_clock -= pause_time;
            this->scanline_time += pause_time;
            this->pause_time = 0;
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }

    if(this->mode == 2)
    {
        this->SwitchMode(3);
    }
    else if(this->mode == 3)
    {
        uint8_t ly = this->gb->mmu->Read(0xFF44);
        uint8_t scy = this->gb->mmu->Read(0xFF42);
        uint8_t scx = this->gb->mmu->Read(0xFF43);

        if(this->fetcher_type == BACKGROUND || this->fetcher_type == WINDOW)
        {
            if(this->fetcher_stage == 0 && this->internal_clock >= 2) // takes 2 T-Cycles to process
            {
                uint16_t map_location = 0;
                uint16_t offset = (this->background_fetcher.fetcher_x_position + ((scx / 8) & 0x1f));
                uint8_t bit = 0;
                if(this->fetcher_type == BACKGROUND)
                {
                    bit = GET_BIT(lcd_control, 3);
                    offset += 32 * (((ly + scy) & 0xFF) / 8);
                }
                else
                {
                    bit = GET_BIT(lcd_control, 6);
                    offset += 32 * (this->window_line_counter / 8);
                }
                
                if(bit)
                {
                    map_location = 0x9C00; // 0x9C00 - 0x9FFF
                }
                else
                {
                    map_location = 0x9800; // 0x9800 - 9BFF
                }

                map_location += (offset & 0x3FFF);
                this->background_fetcher.tile_id = ReadVRAM(map_location);

                this->internal_clock -= 2;
                this->scanline_time += 2;
                this->fetcher_stage = 1;
            }

            if(this->fetcher_stage == 1 && this->internal_clock >= 2)
            {
                uint8_t offset = 0;
                if(this->fetcher_type == BACKGROUND)
                {
                    offset = 2 * ((ly + scy) % 8);
                }           
                else
                {
                    offset = 2 * (this->window_line_counter % 8);
                }

                this->background_fetcher.tile_low_data = this->GetTile(this->background_fetcher.tile_id, false) + offset;
                
                this->internal_clock -= 2;
                this->scanline_time += 2;
                this->fetcher_stage = 2;
            }

            if(this->fetcher_stage == 2 && this->internal_clock >= 2)
            {
                this->internal_clock -= 2;
                this->scanline_time += 2;

                this->background_fetcher.tile_high_data = this->background_fetcher.tile_low_data + 1;

                if(this->first_background_fetch)
                {
                    this->first_background_fetch = false;
                    this->fetcher_stage = 0;
                    return;
                }

                this->fetcher_stage = 3;
            }

            if(this->fetcher_stage == 3)
            {
                // attempts to push fifo
                if(this->background_fetcher.pixel_count == 0)
                {
                    if(this->internal_clock >= 2)
                    {
                        this->internal_clock -= 2;
                        this->scanline_time += 2;
                        this->background_fetcher.fetcher_x_position++;
                        
                        this->background_fetcher.pixel_count = 8;
                        uint8_t b0 = this->gb->mmu->Read(this->background_fetcher.tile_high_data);
                        uint8_t b1 = this->gb->mmu->Read(this->background_fetcher.tile_low_data);
                        for(uint8_t i = 0; i < 8; i++)
                        {
                            this->background_fetcher.pixels[i].color = (GET_BIT(b0, 8 - i) << 1) | GET_BIT(b1, 8 - i);
                            this->background_fetcher.pixels[i].palette = 0;
                            this->background_fetcher.pixels[i].background_priority = 0;
                        }

                        this->fetcher_stage = 0;
                    }
                }
            }

            // attempt to send pixels to screen
            if(this->internal_clock >= this->background_fetcher.pixel_count)
            {
                uint8_t c = this->background_fetcher.pixel_count;
                if(this->internal_clock < c)
                {
                    c = this->internal_clock;
                }
                
                this->internal_clock -= c;
                this->scanline_time += c;

                uint8_t i = 0;
                for(; i < c; i++)
                {
                    //uint8_t id = this->background_fetcher.pixel_count - i - 1;
                    uint8_t resulting_color = this->background_fetcher.pixels[i].color;
                    uint8_t x = this->current_line_x;

                    this->current_line_x++;
                    if(x < scx % 8)
                    {
                        this->screen_pixels[x + ly * 160] = 0;
                        continue;
                    }

                    if (x < 160)
                    {
                        this->screen_pixels[x + ly * 160] = resulting_color;
                    }
                    else
                    {
                        break;
                    }
                }


                this->background_fetcher.pixel_count -= i;
                
                if(this->current_line_x >= 160)
                {
                    this->background_fetcher.fetcher_x_position = 0;
                    this->SwitchMode(0);
                }
            }
        }
    }
    else if(this->mode == 0)
    {
        // go to next scanline
        uint8_t ly = this->gb->mmu->Read(0xFF44);
        this->gb->mmu->Write(0xFF44, ly + 1);
        
        this->current_line_x = 0;

        if(ly == 143)
        {
            // if it reached the end of screen scanlines then go to vblank
            this->SwitchMode(1);
        }
        else
        {
            // go to next scanline
            this->SwitchMode(2);
        }
    }
    else if (this->mode == 1)
    {
        // each scanline takes 456 t cycles
        if (this->internal_clock >= 456)
        {
            this->internal_clock -= 456;

            uint8_t ly = this->gb->mmu->Read(0xFF44);
            if (ly >= 153)
            {
                // if it reached the end of vblank then reset scanline y
                this->gb->mmu->Write(0xFF44, 0);
                this->SwitchMode(2);
            }
            else
            {
                // go to next vblank scanline
                this->gb->mmu->Write(0xFF44, ly + 1);
            }
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

    if(obj || (LCDC & 0x10))
    {
        // 0x8000 method
        return (0x8000 + id * 0x10);
    }
    else if((LCDC & 0x10) == 0)
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

    std::cout << "qeeee" << std::endl;
    return 0x0;
}

void PPU::SwitchMode(uint8_t m)
{
    if(m == this->mode)
    {
        return;
    }

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
                continue;
            }

            this->object_count++;
        }  
    }
    else if(m == 3) // Drawing
    {
    }
    else if(m == 0) // H-Blank
    {
        if(this->scanline_time < 456)
        {
            this->pause_time = 456 - this->scanline_time;
            this->scanline_time = 0;
        }
    }
    else if(m == 1) // V-Blank
    {
        this->gb->cpu->SetInterruptFlag(0, true);
        this->window_line_counter = 0;
    }
    
    this->mode = m;
}