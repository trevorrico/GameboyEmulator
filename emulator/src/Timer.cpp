#include "Timer.h"
#include "GameBoy.h"

Timer::Timer(GameBoy* gameboy)
{
    this->gb = gameboy;

    this->Reset();
}

Timer::~Timer()
{

}

void Timer::Reset()
{
    this->gb->mmu->Write(0xFF04, 0xAB);
}

void Timer::IncrementTIMA()
{    
    uint8_t timer_counter = this->gb->mmu->Read(0xFF05);
    if(timer_counter == 0xFF)
    {
        this->gb->mmu->Write(0xFF05, this->gb->mmu->Read(0xFF06));
        this->gb->cpu->SetInterruptFlag(2, true);
    }
    else
    {
        this->gb->mmu->Write(0xFF05, timer_counter + 1);
    }
}

uint32_t GetFrequencyFromTAC(uint8_t tac)
{
    switch(tac & 0x03)
    {
        case 0x00: return 1024;
        case 0x01: return 16;
        case 0x02: return 64;
        case 0x03: return 256;
        default: 
            return 0;
    }
}

void Timer::Update(uint32_t cycle_diff)
{
    this->internal_div_clock += cycle_diff * 4; // multiplying by 4 means going from M-Cycles to T-Cycles

    if(this->internal_div_clock >= 256)
    {
        this->internal_div_clock -= 256;
        
        //uint8_t div_reg = this->gb->mmu->Read(0xFF04);
        if(this->div + 1 > 0xFF)
        {
            this->div = 0;
        }
        else
        {
            this->div++;
        }
    }

    uint8_t TAC = this->gb->mmu->Read(0xFF07);
    if((TAC & 0x04) == 0)
    {
        // increment is disabled
        return;
    }

    this->internal_clock += cycle_diff * 4;

    uint32_t frequency = GetFrequencyFromTAC(TAC);
    if(this->internal_clock >= frequency)
    {
        this->internal_clock -= frequency;
        IncrementTIMA();
    }
}