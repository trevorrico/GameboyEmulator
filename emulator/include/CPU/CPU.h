#ifndef EMULATOR_CPU_H_
#define EMULATOR_CPU_H_

#include <stdint.h>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

class GameBoy;

class CPU
{
public:
	CPU(GameBoy* gb);
	~CPU();

	void Reset();
	void Step();

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
	} registers;	
	
	uint32_t cycles;

	uint8_t get_carry_flag();
	uint8_t get_zero_flag();
	uint8_t get_subtraction_flag();
	uint8_t get_half_carry_flag();

	uint32_t log_lines = 0;
private:
	std::stringstream log_string;

	void ProcessOpcode(uint8_t opcode);
	void ProcessCBOpcode(uint8_t opcode);

	void set_zero_flag(bool set); // Z
	void set_carry_flag(bool set); // C
	void set_subtraction_flag(bool set); // N
	void set_half_carry_flag(bool set); // H

	// OPCODES FUNCTIONS
	void opcode_LD_r8_n8(uint8_t& r8);
	void opcode_LD_r16(uint16_t& r16);
	void opcode_LD_r16_A(uint16_t& r16);
	void opcode_LD_addr16_A();
	void opcode_LD_r8_r16address(uint8_t& r8, uint16_t r16address);
	void opcode_LD_r8_n16(uint8_t& r8);
	void opcode_LD_n16_r16(uint16_t& r16);
	void opcode_LD_r8_r8(uint8_t& r8_0, uint8_t& r8_1);
	void opcode_LD_r16_r16(uint16_t& r8_0, uint16_t& r8_1);
	void opcode_LD_hl_n8();
	void opcode_LD_hl_sp_n8();
	void opcode_LD_hl_r8(uint8_t& r8);
	void opcode_LDH_addr8_r8(uint8_t& addr, uint8_t& r8);
	void opcode_LDH_r8_addr8(uint8_t& r8, uint8_t addr8);
	void opcode_LDH_n8_r8(uint8_t& r8);
	void opcode_LDH_r8_n8(uint8_t& r8);
	void opcode_INC_r16(uint16_t& r16);
	void opcode_INC_r8(uint8_t& r8);
	void opcode_INC_hl();
	void opcode_DEC_r8(uint8_t& r8);
	void opcode_DEC_r16(uint16_t& r16);
	void opcode_DEC_hl();
	void opcode_ADD_hl(uint16_t& r16);
	void opcode_ADD_A_r8(uint8_t& r8);
	void opcode_ADD_SP_e8();
	void opcode_ADD_A_n8();
	void opcode_ADD_A_hl();
	void opcode_ADC_A_r8(uint8_t& r8);
	void opcode_ADC_A_n8();
	void opcode_ADC_A_hl();
	void opcode_SUB_A_r8(uint8_t& r8);
	void opcode_SUB_A_n8();
	void opcode_SUB_A_hl();
	void opcode_SBC_A_r8(uint8_t& r8);
	void opcode_SBC_A_n8();
	void opcode_SBC_A_hl();
	void opcode_AND_A_r8(uint8_t& r8);
	void opcode_AND_A_n8();
	void opcode_AND_A_hl();
	void opcode_XOR_A_r8(uint8_t& r8);
	void opcode_XOR_A_n8();
	void opcode_XOR_A_hl();
	void opcode_OR_A_r8(uint8_t& r8);
	void opcode_OR_A_n8();
	void opcode_OR_A_hl();
	void opcode_CP_A_r8(uint8_t& r8);
	void opcode_CP_A_n8();
	void opcode_CP_A_hl();
	void opcode_RLCA();
	void opcode_RRCA();
	void opcode_RLA();
	void opcode_RRA();
	void opcode_JR();
	void opcode_JR_condition(bool condition);
	void opcode_DAA();
	void opcode_CPL();
	void opcode_SCF();
	void opcode_CCF();
	void opcode_POP_r16(uint16_t& r16);
	void opcode_POP_AF();
	void opcode_PUSH_r16(uint16_t& r16);
	void opcode_RET();
	void opcode_RET_cond(uint8_t cond);
	void opcode_JP_r16(uint16_t& r16);
	void opcode_JP_n16();
	void opcode_JP_cond(uint8_t cond);
	void opcode_JP_hl();
	void opcode_CALL();
	void opcode_CALL_cond(uint8_t cond);
	void opcode_RST(uint16_t address);
	void opcode_EI();
	void opcode_DI();
	void opcode_RETI();

	// CB opcodes
	void opcode_RLC(uint8_t& r8);
	void opcode_RLC_hl();
	void opcode_RRC(uint8_t& r8);
	void opcode_RRC_hl();
	void opcode_RL(uint8_t& r8);
	void opcode_RL_hl();
	void opcode_RR(uint8_t& r8);
	void opcode_RR_hl();
	void opcode_SLA(uint8_t& r8);
	void opcode_SLA_hl();
	void opcode_SRA(uint8_t& r8);
	void opcode_SRA_hl();
	void opcode_SWAP(uint8_t& r8);
	void opcode_SWAP_hl();
	void opcode_SRL(uint8_t& r8);
	void opcode_SRL_hl();
	void opcode_BIT(uint8_t u3, uint8_t r8);
	void opcode_BIT_hl(uint8_t u3);
	void opcode_RES(uint8_t u3, uint8_t& r8);
	void opcode_RES_hl(uint8_t u3);
	void opcode_SET(uint8_t u3, uint8_t& r8);
	void opcode_SET_hl(uint8_t u3);

	GameBoy* gb;
};

#endif