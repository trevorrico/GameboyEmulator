#ifndef EMULATOR_PPU_H_
#define EMULATOR_PPU_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <iostream>
#include <bitset>

#include "BitwiseUtils.h"

class GameBoy;

enum FetcherType
{
    BACKGROUND,
    WINDOW,
    SPRITE
};

struct FIFOPixel
{
    uint8_t color; // 0 to 3
    FetcherType type;
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
    uint16_t tile_low_data = 0;
    uint16_t tile_high_data = 0;
    uint8_t tile_id = 0;
};

// gameboy res: 160x144

class PPU
{
public:
    PPU(GameBoy* gameboy);
    ~PPU();

    void Reset();

    void Tick(uint8_t cycles);

    uint8_t ReadVRAM(uint32_t address);
    void WriteVRAM(uint32_t address, uint8_t data);

    bool IsVRAMAcessible();

    uint8_t ReadOAM(uint32_t address);
    void WriteOAM(uint32_t address, uint8_t data);

    void SwitchMode(uint8_t mode);

    uint32_t screen_pixels[160 * 144] = { 0 };

    bool requested_vram_debug_update = false;
private:
    uint16_t GetTile(uint8_t id, bool obj); // returns tile address

    void PushToLCD(uint8_t cycles);

    GameBoy* gb;

    uint8_t mode = 2;

    uint16_t pause_time = 0;
    uint16_t internal_clock = 0;
    uint16_t fifo_clock = 0;
    uint16_t current_line_x = 0;
    uint16_t line_processed_pixel_count = 0;

    uint16_t window_line_counter = 0;
    uint16_t scanline_time = 0;

    uint8_t fetcher_stage = 0;
    FetcherType fetcher_type;
    Fetcher background_fetcher;
    bool reached_window_in_frame = false;
    Fetcher sprite_fetcher;


    uint8_t vram[0x2000]; // 0x8000-0x9FFF
    uint8_t oam[0xA0] = { 0 }; // 0xFE00-0xFE9F

    Sprite object_buffer[40];
    uint8_t object_count = 0;

    FIFOPixel fifo[16] = { 0 };
    uint8_t fifo_pixel_count = 0;

    Sprite current_rendering_sprite = { 0 };
};

#endif