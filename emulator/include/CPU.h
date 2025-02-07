#ifndef EMULATOR_CPU_H_
#define EMULATOR_CPU_H_

#include <stdint.h>

namespace CPU
{
	struct Registers
	{
		union
		{
			// Lower 8 bits:
			// Bit 7: Zero flag
			// Bit 6: Subtraction flag
			// Bit 5: Half Carry flag
			// Bit 4: Carry flag
			struct
			{
				uint8_t F;
				uint8_t A;
			};
			uint16_t AF; // Accumulator & Flags
		};

		union
		{
			struct
			{
				uint8_t C;
				uint8_t B;
			};        
			uint16_t BC;
		};

		union
		{
			struct
			{
				uint8_t E;
				uint8_t D;
			};
			uint16_t DE;
		};

		union
		{
			struct
			{
				uint8_t L;
				uint8_t H;
			};
			uint16_t HL;
		};

		uint16_t SP; // Stack pointer
		uint16_t PC; // Program counter
	};

	bool Initialize();
	void Shutdown();

	void Reset();
	void Step();
}

#endif