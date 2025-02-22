#ifndef EMULATOR_PPU_H_
#define EMULATOR_PPU_H_

#include <stdint.h>
#include <iostream>

class GameBoy;

struct FIFOPixel
{
    uint8_t color; // 0 to 3
    uint8_t palette;
    uint8_t background_priority;
};

struct Sprite
{
    uint8_t y_pos;
    uint8_t x_pos;
    uint8_t tile;
    uint8_t flags;
};

// gameboy res: 160x144

class PPU
{
public:
    PPU(GameBoy* gameboy);
    ~PPU();

    void Tick(uint8_t cycles);

    uint8_t ReadVRAM(uint16_t address);
    void WriteVRAM(uint16_t address, uint8_t data);

    bool IsVRAMAcessible();

    uint8_t ReadOAM(uint16_t address);
    void WriteOAM(uint16_t address, uint8_t data);

    void SwitchMode(uint8_t mode);
private:
    uint16_t GetTile(uint8_t id, bool obj); // returns tile address

    GameBoy* gb;

    uint8_t mode = 2;

    uint16_t pause_time = 0;
    uint16_t internal_clock = 0;
    uint16_t current_line_x = 0;

    FIFOPixel background_pixels[256][256];
    FIFOPixel sprite_pixels[160][144];
    uint8_t screen_pixels[160][144];

    uint8_t vram[0x2000]; // 0x8000-0x9FFF
    uint8_t oam[0xA0]; // 0xFE00-0xFE9F

    Sprite object_buffer[40];
    uint8_t object_count = 0;
};

#endif