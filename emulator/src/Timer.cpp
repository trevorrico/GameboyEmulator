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
    if(timer_counter + 1 > 0xFF)
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
        case 0x00: return 4096;
        case 0x01: return 262144;
        case 0x02: return 65536;
        case 0x03: return 16384;
        default: 
            std::cout << "qe" << std::endl;
            return 0;
    }
}

void Timer::Update(uint32_t cycle_diff)
{
    this->internal_div_clock += cycle_diff * 4; // multiplying by 4 means going from M-Cycles to T-Cycles

    // divide cpu clock speed by the div speed so it is synced with the CPU
    if(this->internal_div_clock > CLOCK_SPEED / DIV_CLOCK_SPEED)
    {
        this->internal_div_clock %= CLOCK_SPEED / DIV_CLOCK_SPEED;
        
        uint8_t div_reg = this->gb->mmu->Read(0xFF04);
        if(div_reg + 1 > 0xFF)
        {
            this->gb->mmu->Write(0xFF04, 0x00);
        }
        else
        {
            this->gb->mmu->Write(0xFF04, div_reg + 1);
        }
    }

    uint8_t TAC = this->gb->mmu->Read(0xFF07);
    if((TAC & 0x04) == 0)
    {
        // increment is disabled
        this->internal_clock = 0;
        return;
    }

    this->internal_clock += cycle_diff * 4;

    uint32_t frequency = GetFrequencyFromTAC(TAC);
    if(this->internal_clock > CLOCK_SPEED / frequency)
    {
        this->internal_clock %= CLOCK_SPEED / frequency;
        IncrementTIMA();
    }
}