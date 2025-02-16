#ifndef EMULATOR_TIMER_H_
#define EMULATOR_TIMER_H_

#include <stdint.h>

#include "CPU/CPU.h"

#define DIV_CLOCK_SPEED 16384

class GameBoy;

class Timer
{
public:
    Timer(GameBoy* gameboy);
    ~Timer();

    void Update(uint32_t cycle_diff);

    void IncrementTIMA();
private:
    GameBoy* gb;

    uint32_t internal_clock = 0;
    uint32_t internal_div_clock = 0;
};

#endif