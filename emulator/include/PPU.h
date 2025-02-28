#ifndef EMULATOR_PPU_H_
#define EMULATOR_PPU_H_

#include <stdint.h>
#include <iostream>
#include <bitset>

#include "BitwiseUtils.h"

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

struct Fetcher
{
    uint16_t fetcher_x_position = 0;
    FIFOPixel pixels[8];
    uint8_t pixel_count = 0;
    uint8_t tile_id = 0;
    uint16_t tile_low_data = 0;
    uint16_t tile_high_data = 0;
};

enum FetcherType
{
    BACKGROUND,
    WINDOW,
    SPRITE
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

    uint32_t screen_pixels[160 * 144] = { 0 };
private:
    uint16_t GetTile(uint8_t id, bool obj); // returns tile address

    GameBoy* gb;

    uint8_t mode = 2;

    uint16_t pause_time = 0;
    uint16_t internal_clock = 0;
    uint16_t current_line_x = 0;

    uint16_t window_line_counter = 0;
    uint16_t scanline_time = 0;

    uint8_t fetcher_stage = 0;
    FetcherType fetcher_type;
    Fetcher background_fetcher;
    bool first_background_fetch = true;
    Fetcher sprite_fetcher;


    uint8_t vram[0x2000]; // 0x8000-0x9FFF
    uint8_t oam[0xA0]; // 0xFE00-0xFE9F

    Sprite object_buffer[40];
    uint8_t object_count = 0;
};

#endif