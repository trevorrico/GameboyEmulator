#include "PPU.h"
#include "GameBoy.h"

PPU::PPU(GameBoy* gameboy)
{
    this->gb = gameboy;

    this->Reset();
}

PPU::~PPU()
{

}

void PPU::Reset()
{
    this->SwitchMode(2);
    this->fetcher_type = BACKGROUND;
    
    for (int i = 0; i <= 0x1FFF; i++)
    {
        this->vram[i] = 0;
    }

    for (int i = 0; i <= 0x9F; i++)
    {
        this->oam[i] = 0;
    }
}

void PPU::Tick(uint8_t cycles)
{
    uint8_t lcd_control = this->gb->mmu->Read(0xFF40);
    if((lcd_control & 0x80) == 0)
    {
        //return; // disabled
    }

    uint8_t ly = this->gb->mmu->Read(0xFF44);
    uint8_t lcy = this->gb->mmu->Read(0xFF45);
    uint8_t lcd_status = this->gb->mmu->Read(0xFF41);

    //this->gb->mmu->Write(0xFF42, (uint8_t)(glfwGetTime() * 8));
    
    if (ly == lcy)
    {
        this->gb->cpu->SetInterruptFlag(1, true);
        
        SET_BIT(lcd_status, 6);
        SET_BIT(lcd_status, 2);
    }
    else
    {
        CLEAR_BIT(lcd_status, 6);
        CLEAR_BIT(lcd_status, 2);
    }

    if (lcd_status & 0x1C)
    {
        this->gb->cpu->SetInterruptFlag(1, true);
    }

    lcd_status &= (~0x03);
    lcd_status |= this->mode;

    this->gb->mmu->Write(0xFF41, lcd_status);

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
        uint8_t scy = this->gb->mmu->Read(0xFF42);
        uint8_t scx = this->gb->mmu->Read(0xFF43);

        if(this->fetcher_stage == 0 && this->internal_clock >= 2) // takes 2 T-Cycles to process
        {
            uint16_t map_location = 0;
            if (this->fetcher_type == SPRITE)
            {
                for (int i = 0; i < this->object_count; i++)
                {
                    if (this->object_buffer[i].x_pos <= this->current_line_x + 8)
                    {
                        this->fetcher_type = SPRITE;

                        if (GET_BIT(lcd_control, 2))
                        {
                            this->sprite_fetcher.tile_id = this->object_buffer[i].tile;
                            if (ly + 16 >= this->object_buffer[i].y_pos + 8)
                            {
                                SET_BIT(this->sprite_fetcher.tile_id, 0);
                            }
                        }
                        else
                        {
                            this->sprite_fetcher.tile_id = this->object_buffer[i].tile;
                        }

                        this->current_rendering_sprite = this->object_buffer[i];

                        // remove this object from obj buffer
                        for (int j = i + 1; j < this->object_count; j++)
                        {
                            this->object_buffer[j - 1] = this->object_buffer[j];
                        }

                        this->object_count--;

                        break;
                    }
                }
            }
            else
            {
                uint16_t offset = (this->background_fetcher.fetcher_x_position);
                uint8_t bit = 0;
                if (this->fetcher_type == BACKGROUND)
                {
                    bit = GET_BIT(lcd_control, 3);

                    offset = (offset + ((scx / 8)));
                    offset %= 0x20;
                    offset += 32 * ((((ly + scy) % 0x100) / 8));
                }
                else
                {
                    bit = GET_BIT(lcd_control, 6);
                    offset += 32 * (this->window_line_counter / 8);
                }

                if (bit)
                {
                    map_location = 0x9C00; // 0x9C00 - 0x9FFF
                }
                else
                {
                    map_location = 0x9800; // 0x9800 - 9BFF
                }

                map_location += (offset % 0x4000);
                this->background_fetcher.tile_id = ReadVRAM(map_location);
            }

            this->internal_clock -= 2;
            this->scanline_time += 2;
            this->fetcher_stage = 1;
        }

        if(this->fetcher_stage == 1 && this->internal_clock >= 2)
        {
            if (this->fetcher_type == SPRITE)
            {
                uint16_t offset = 2 * ((ly - current_rendering_sprite.y_pos + 16) % 8);
                if (GET_BIT(current_rendering_sprite.flags, 6))
                {
                    offset = 14 - offset;
                }
                this->sprite_fetcher.tile_low_data = this->GetTile(this->sprite_fetcher.tile_id, true) + offset;
            }
            else
            {
                uint16_t offset = 0;
                if (this->fetcher_type == BACKGROUND)
                {
                    offset = 2 * ((ly + scy) % 8);
                }
                else
                {
                    offset = 2 * (this->window_line_counter % 8);
                }

                this->background_fetcher.tile_low_data = this->GetTile(this->background_fetcher.tile_id, false) + offset;
            }
                
            this->internal_clock -= 2;
            this->scanline_time += 2;
            this->fetcher_stage = 2;
        }

        if(this->fetcher_stage == 2 && this->internal_clock >= 2)
        {
            this->internal_clock -= 2;
            this->scanline_time += 2;

            if (this->fetcher_type == SPRITE)
            {
                this->sprite_fetcher.tile_high_data = this->sprite_fetcher.tile_low_data + 1;
            }
            else
            {
                this->background_fetcher.tile_high_data = this->background_fetcher.tile_low_data + 1;
            }

            this->fetcher_stage = 3;
        }

        if(this->fetcher_stage == 3)
        {
            // attempts to push fifo
            if (this->internal_clock >= 2)
            {
                if (this->fetcher_type == SPRITE && this->fifo_pixel_count >= 8)
                {
                    this->internal_clock -= 2;
                    this->scanline_time += 2;

                    uint8_t b0 = this->gb->mmu->Read(this->sprite_fetcher.tile_high_data);
                    uint8_t b1 = this->gb->mmu->Read(this->sprite_fetcher.tile_low_data);

                    bool flipped = GET_BIT(current_rendering_sprite.flags, 5);

                    uint8_t obp0 = this->gb->mmu->Read(0xFF48);
                    uint8_t obp1 = this->gb->mmu->Read(0xFF49);

                    uint8_t bgp = this->gb->mmu->Read(0xFF47);

                    uint8_t color = 0;
                    for (uint8_t i = 0; i < 8; i++)
                    {
                        uint8_t pixel_x = current_rendering_sprite.x_pos + i;
                        if (pixel_x < 8 || pixel_x >= 168 || pixel_x < this->current_line_x + 8)
                        {
                            continue;
                        }

                        uint8_t fifo_id = pixel_x - (this->current_line_x + 8);

                        if (flipped)
                        {
                            color = (GET_BIT(b0, i) << 1) | GET_BIT(b1, i);
                        }
                        else
                        {
                            color = (GET_BIT(b0, 7 - i) << 1) | GET_BIT(b1, 7 - i);
                        }

                        // mix fifo
                        if (color > 0)
                        {
                            if (GET_BIT(current_rendering_sprite.flags, 7))
                            {
                                if (this->fifo[fifo_id].color != (GET_BIT(bgp, 0) | (GET_BIT(bgp, 1) << 1)))
                                {
                                    continue;
                                }
                            }

                            if (GET_BIT(current_rendering_sprite.flags, 4))
                            {
                                // OBP1
                                color = GET_BIT(obp1, color * 2) | (GET_BIT(obp1, color * 2 + 1) << 1);
                            }
                            else
                            {
                                // OBP0
                                color = GET_BIT(obp0, color * 2) | (GET_BIT(obp0, color * 2 + 1) << 1);
                            }  

                            this->fifo[fifo_id].color = color;
                            this->fifo[fifo_id].type = SPRITE;
                        }
                    }

                    this->fetcher_type = BACKGROUND;
                    this->fetcher_stage = 0;
                }
                else
                {
                    if (this->fifo_pixel_count <= 8)
                    {

                        this->internal_clock -= 2;
                        this->scanline_time += 2;

                        this->background_fetcher.fetcher_x_position++;

                        uint8_t b0 = this->gb->mmu->Read(this->background_fetcher.tile_high_data);
                        uint8_t b1 = this->gb->mmu->Read(this->background_fetcher.tile_low_data);

                        uint8_t bgp = this->gb->mmu->Read(0xFF47);

                        for (uint8_t i = 0; i < 8; i++)
                        {
                            uint8_t color = (GET_BIT(b0, 7 - i) << 1) | GET_BIT(b1, 7 - i);
                            uint8_t palette_color = GET_BIT(bgp, color * 2) | (GET_BIT(bgp, color * 2 + 1) << 1);
                            
                            this->fifo[this->fifo_pixel_count + i].color = palette_color;
                            this->fifo[this->fifo_pixel_count + i].type = BACKGROUND;
                        }

                        this->fifo_pixel_count += 8;

                        this->fetcher_stage = 0;
                    }
                }
            }
        }

        PushToLCD(cycles);
    }
    else if(this->mode == 0)
    {
        // go to next scanline
        uint8_t ly = this->gb->mmu->Read(0xFF44);

        if(this->reached_window_in_frame)
        {
            this->window_line_counter++;
        }

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

                this->requested_vram_debug_update = true;
            }
            else
            {
                // go to next vblank scanline
                this->gb->mmu->Write(0xFF44, ly + 1);
            }
        }
    }
}

void PPU::PushToLCD(uint8_t cycles)
{
    if (this->fifo_pixel_count > 8 && this->fetcher_type != SPRITE)
    {
        uint8_t ly = this->gb->mmu->Read(0xFF44);
        uint8_t scx = this->gb->mmu->Read(0xFF43);
        uint8_t lcdc = this->gb->mmu->Read(0xFF40);
        uint8_t wy = this->gb->mmu->Read(0xFF4A);
        uint8_t wx = this->gb->mmu->Read(0xFF4B);

        if(wy == ly)
        {
            reached_window_in_frame = true;
        }

        this->fifo_clock += cycles * 4;

        while (this->fifo_clock > 0)
        {
            if (this->fifo_pixel_count <= 8 || this->fetcher_type == SPRITE)
            {
                break;
            }

            this->fifo_clock--;

            if(this->fetcher_type == BACKGROUND)
            {
                // window fetch
                if(GET_BIT(lcdc, 5))
                {
                    if(reached_window_in_frame)
                    {
                        if(current_line_x >= wx - 7)
                        {
                            this->fifo_pixel_count = 0;
                            this->background_fetcher.fetcher_x_position = 0;
                            this->fetcher_stage = 0;
                            this->fetcher_type = WINDOW;
                            return;
                        }
                    }
                }
            }

            // check objects in current X
            for (int i = 0; i < this->object_count; i++)
            {
                if (this->object_buffer[i].x_pos <= this->current_line_x + 8)
                {
                    this->fetcher_stage = 0;
                    this->fetcher_type = SPRITE;
                    return;
                }
            }

            // pop current pixel
            FIFOPixel pixel = this->fifo[0];

            for (int i = 1; i < this->fifo_pixel_count; i++)
            {
                this->fifo[i - 1] = this->fifo[i];
            }

            this->fifo_pixel_count--;

            // ignore if pixel is in the scroll
            if (this->line_processed_pixel_count < scx % 8)
            {
                this->line_processed_pixel_count++;
                continue;
            }

            this->line_processed_pixel_count++;

            // set screen pixel
            this->screen_pixels[this->current_line_x + ly * 160] = pixel.color;
            this->current_line_x++;

            if (this->current_line_x == 160)
            {
                this->SwitchMode(0);
                break;
            }
        }
    }
}

uint8_t PPU::ReadVRAM(uint32_t address)
{
    assert(address >= 0x8000 && address <= 0x9FFF);

    return this->vram[address - 0x8000];
}

void PPU::WriteVRAM(uint32_t address, uint8_t data)
{
    assert(address >= 0x8000 && address <= 0x9FFF);

    this->vram[address - 0x8000] = data;
}

bool PPU::IsVRAMAcessible()
{
    return mode != 3;
}

uint8_t PPU::ReadOAM(uint32_t address)
{
    assert(address >= 0xFE00 && address <= 0xFE9F);

    return this->oam[address - 0xFE00];
}

void PPU::WriteOAM(uint32_t address, uint8_t data)
{
    assert(address >= 0xFE00 && address <= 0xFE9F);

    //std::cout << "New oam data: " << (uint32_t)data << std::endl;
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

    return 0x0;
}

void PPU::SwitchMode(uint8_t m)
{
    if(m == this->mode)
    {
        return;
    }

    uint8_t ly = this->gb->mmu->Read(0xFF44);
    uint8_t lcd_status = this->gb->mmu->Read(0xFF41);
    uint8_t lcd_control = this->gb->mmu->Read(0xFF40);

    CLEAR_BIT(lcd_status, 3);
    CLEAR_BIT(lcd_status, 4);
    CLEAR_BIT(lcd_status, 5);

    if(m == 2) // OAM Scan
    {
        SET_BIT(lcd_status, 5);

        uint8_t sprite_height = (lcd_control & 0x04) ? 16 : 8;

        this->pause_time = 80;  

        // scan for objects on this scanline
        this->object_count = 0;
        for(int i = 0; i < 40; i++)
        {
            uint16_t address = 4 * i;
            
            Sprite sp;

            sp.y_pos = this->oam[address];
            sp.x_pos = this->oam[address + 1];
            sp.tile = this->oam[address + 2];
            sp.flags = this->oam[address + 3];

            if (sp.x_pos > 0)
            {
                if (ly + 16 >= sp.y_pos)
                {
                    if (ly + 16 < sp.y_pos + sprite_height)
                    {
                        if (sprite_height == 16)
                        {
                            CLEAR_BIT(sp.tile, 0);
                        }

                        this->object_buffer[this->object_count] = sp;
                        this->object_count++;

                        if (this->object_count >= 10)
                        {
                            break;
                        }
                    }
                }
            }
        }  
    }
    else if(m == 3) // Drawing
    {
        this->fetcher_stage = 0;
        this->line_processed_pixel_count = 0;
        this->current_line_x = 0;
        this->background_fetcher.fetcher_x_position = 0;
        this->fetcher_type = BACKGROUND;

        this->fifo_pixel_count = 0;
    }
    else if(m == 0) // H-Blank
    {
        SET_BIT(lcd_status, 3);

        if(this->scanline_time < 456)
        {
            this->pause_time = 456 - this->scanline_time;
            this->scanline_time = 0;
        }
    }
    else if(m == 1) // V-Blank
    {
        SET_BIT(lcd_status, 4);

        this->gb->cpu->SetInterruptFlag(0, true);
        this->window_line_counter = 0;
        this->reached_window_in_frame = false;
    }
    
    this->mode = m;

    this->gb->mmu->Write(0xFF41, lcd_status);
}