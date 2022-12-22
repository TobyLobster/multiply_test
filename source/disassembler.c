// only slightly modified from https://gist.github.com/slembcke/4b746cb71d435f16c4763ae2de3201f3

#include <stdint.h>
#include <stdio.h>

#include "disassembler.h"

// 6502 instruction tables.
enum OPCODE {
	OP_LDA, OP_LDX, OP_LDY, OP_STA, OP_STX, OP_STY, OP_ADC, OP_SBC,
	OP_INC, OP_INX, OP_INY, OP_DEC, OP_DEX, OP_DEY, OP_ASL, OP_LSR,
	OP_ROL, OP_ROR, OP_AND, OP_ORA, OP_EOR, OP_CMP, OP_CPX, OP_CPY,
	OP_BIT, OP_BPL, OP_BMI, OP_BVC, OP_BVS, OP_BCC, OP_BCS, OP_BNE,
	OP_BEQ, OP_TAX, OP_TXA, OP_TAY, OP_TYA, OP_TSX, OP_TXS, OP_PHA,
	OP_PLA, OP_PHP, OP_PLP, OP_JMP, OP_JSR, OP_RTS, OP_RTI, OP_SEC,
	OP_SED, OP_SEI, OP_CLC, OP_CLD, OP_CLI, OP_CLV, OP_NOP, OP_BRK,
	OP_XXX,
};

// Convert an opcode to an mnemonic.
static const char OPCODE_MNEMONICS[][3] = {
	"lda", "ldx", "ldy", "sta", "stx", "sty", "adc", "sbc",
	"inc", "inx", "iny", "dec", "dex", "dey", "asl", "lsr",
	"rol", "ror", "and", "ora", "eor", "cmp", "cpx", "cpy",
	"bit", "bpl", "bmi", "bvc", "bvs", "bcc", "bcs", "bne",
	"beq", "tax", "txa", "tay", "tya", "tsx", "txs", "pha",
	"pla", "php", "plp", "jmp", "jsr", "rts", "rti", "sec",
	"sed", "sei", "clc", "cld", "cli", "clv", "nop", "brk",
	"???",
};

enum ADDR_MODE {AM_ACC, AM_IMP, AM_IMM, AM_ABS, AM_IND, AM_ZPG, AM_REL, AM_AXI, AM_AYI, AM_ZXI, AM_ZYI, AM_XII, AM_YII, AM_XXX};

// Get a printf format string for a given address mode.
static const char *ADDR_MODE_FORMATS[] = {
	"%.3s", // AADR_MODE_ACCUMULATOR
	"%.3s", // AADR_MODE_IMPLIED
	"%.3s #$%02x", // AADR_MODE_IMMEDIATE
	"%.3s $%04x", // AADR_MODE_ABSOLUTE,
	"%.3s ($%04x)", // Indirect
	"%.3s $%02x", // AADR_MODE_ZERO_PAGE,
	"%.3s $%02x", // AADR_MODE_RELATIVE,
	"%.3s $%04x,X", // AADR_MODE_ABSOLUTE_X_INDEXED,
	"%.3s $%04x,Y", // AADR_MODE_ABSOLUTE_Y_INDEXED,
	"%.3s $%02x,X", // AADR_MODE_ZERO_PAGE_X_INDEXED,
	"%.3s $%02x,Y", // AADR_MODE_ZERO_PAGE_Y_INDEXED,
	"%.3s ($%02x,X)", // AADR_MODE_ZERO_PAGE_X_INDEXED_INDIRECT,
	"%.3s ($%02x),Y", // AADR_MODE_ZERO_PAGE_Y_INDIRECT_INDEXED,
	"BAD INSTRUCTION",
};

// Get the instruction length for a given address mode.
static const uint8_t ADDR_MODE_LENGTHS[] = {
	1, // AADR_MODE_ACCUMULATOR
	1, // AADR_MODE_IMPLIED
	2, // AADR_MODE_IMMEDIATE
	3, // AADR_MODE_ABSOLUTE,
	3, // Indirect
	2, // AADR_MODE_ZERO_PAGE,
	2, // AADR_MODE_RELATIVE,
	3, // AADR_MODE_ABSOLUTE_X_INDEXED,
	3, // AADR_MODE_ABSOLUTE_Y_INDEXED,
	2, // AADR_MODE_ZERO_PAGE_X_INDEXED,
	2, // AADR_MODE_ZERO_PAGE_Y_INDEXED,
	2, // AADR_MODE_ZERO_PAGE_X_INDEXED_INDIRECT,
	2, // AADR_MODE_ZERO_PAGE_Y_INDIRECT_INDEXED,
	1,
};

// Get the opcode for a given instruction.
static const uint8_t INSTRUCTION_OPCODES[] = {
	OP_BRK, OP_ORA, OP_XXX, OP_XXX, OP_XXX, OP_ORA, OP_ASL, OP_XXX, OP_PHP, OP_ORA, OP_ASL, OP_XXX, OP_XXX, OP_ORA, OP_ASL, OP_XXX,
	OP_BPL, OP_ORA, OP_XXX, OP_XXX, OP_XXX, OP_ORA, OP_ASL, OP_XXX, OP_CLC, OP_ORA, OP_XXX, OP_XXX, OP_XXX, OP_ORA, OP_ASL, OP_XXX,
	OP_JSR, OP_AND, OP_XXX, OP_XXX, OP_BIT, OP_AND, OP_ROL, OP_XXX, OP_PLP, OP_AND, OP_ROL, OP_XXX, OP_BIT, OP_AND, OP_ROL, OP_XXX,
	OP_BMI, OP_AND, OP_XXX, OP_XXX, OP_XXX, OP_AND, OP_ROL, OP_XXX, OP_SEC, OP_AND, OP_XXX, OP_XXX, OP_XXX, OP_AND, OP_ROL, OP_XXX,
	OP_RTI, OP_EOR, OP_XXX, OP_XXX, OP_XXX, OP_EOR, OP_LSR, OP_XXX, OP_PHA, OP_EOR, OP_LSR, OP_XXX, OP_JMP, OP_EOR, OP_LSR, OP_XXX,
	OP_BVC, OP_EOR, OP_XXX, OP_XXX, OP_XXX, OP_EOR, OP_LSR, OP_XXX, OP_CLI, OP_EOR, OP_XXX, OP_XXX, OP_XXX, OP_EOR, OP_LSR, OP_XXX,
	OP_RTS, OP_ADC, OP_XXX, OP_XXX, OP_XXX, OP_ADC, OP_ROR, OP_XXX, OP_PLA, OP_ADC, OP_ROR, OP_XXX, OP_JMP, OP_ADC, OP_ROR, OP_XXX,
	OP_BVS, OP_ADC, OP_XXX, OP_XXX, OP_XXX, OP_ADC, OP_ROR, OP_XXX, OP_SEI, OP_ADC, OP_XXX, OP_XXX, OP_XXX, OP_ADC, OP_ROR, OP_XXX,
	OP_XXX, OP_STA, OP_XXX, OP_XXX, OP_STY, OP_STA, OP_STX, OP_XXX, OP_DEY, OP_XXX, OP_TXA, OP_XXX, OP_STY, OP_STA, OP_STX, OP_XXX,
	OP_BCC, OP_STA, OP_XXX, OP_XXX, OP_STY, OP_STA, OP_STX, OP_XXX, OP_TYA, OP_STA, OP_TXS, OP_XXX, OP_XXX, OP_STA, OP_XXX, OP_XXX,
	OP_LDY, OP_LDA, OP_LDX, OP_XXX, OP_LDY, OP_LDA, OP_LDX, OP_XXX, OP_TAY, OP_LDA, OP_TAX, OP_XXX, OP_LDY, OP_LDA, OP_LDX, OP_XXX,
	OP_BCS, OP_LDA, OP_XXX, OP_XXX, OP_LDY, OP_LDA, OP_LDX, OP_XXX, OP_CLV, OP_LDA, OP_TSX, OP_XXX, OP_LDY, OP_LDA, OP_LDX, OP_XXX,
	OP_CPY, OP_CMP, OP_XXX, OP_XXX, OP_CPY, OP_CMP, OP_DEC, OP_XXX, OP_INY, OP_CMP, OP_DEX, OP_XXX, OP_CPY, OP_CMP, OP_DEC, OP_XXX,
	OP_BNE, OP_CMP, OP_XXX, OP_XXX, OP_XXX, OP_CMP, OP_DEC, OP_XXX, OP_CLD, OP_CMP, OP_XXX, OP_XXX, OP_XXX, OP_CMP, OP_DEC, OP_XXX,
	OP_CPX, OP_SBC, OP_XXX, OP_XXX, OP_CPX, OP_SBC, OP_INC, OP_XXX, OP_INX, OP_SBC, OP_NOP, OP_XXX, OP_CPX, OP_SBC, OP_INC, OP_XXX,
	OP_BEQ, OP_SBC, OP_XXX, OP_XXX, OP_XXX, OP_SBC, OP_INC, OP_XXX, OP_SED, OP_SBC, OP_XXX, OP_XXX, OP_XXX, OP_SBC, OP_INC, OP_XXX,
};

// Get the addressing mode for a given instruction.
static const uint8_t INSTRUCTION_ADDRESS_MODES[] = {
	AM_IMP, AM_XII, AM_XXX, AM_XXX, AM_XXX, AM_ZPG, AM_ZPG, AM_XXX, AM_IMP, AM_IMM, AM_ACC, AM_XXX, AM_XXX, AM_ABS, AM_ABS, AM_XXX,
	AM_REL, AM_YII, AM_XXX, AM_XXX, AM_XXX, AM_ZXI, AM_ZXI, AM_XXX, AM_IMP, AM_AYI, AM_XXX, AM_XXX, AM_XXX, AM_AXI, AM_AXI, AM_XXX,
	AM_ABS, AM_XII, AM_XXX, AM_XXX, AM_ZPG, AM_ZPG, AM_ZPG, AM_XXX, AM_IMP, AM_IMM, AM_ACC, AM_XXX, AM_ABS, AM_ABS, AM_ABS, AM_XXX,
	AM_REL, AM_YII, AM_XXX, AM_XXX, AM_XXX, AM_ZXI, AM_ZXI, AM_XXX, AM_IMP, AM_AYI, AM_XXX, AM_XXX, AM_XXX, AM_AXI, AM_AXI, AM_XXX,
	AM_IMP, AM_XII, AM_XXX, AM_XXX, AM_XXX, AM_ZPG, AM_ZPG, AM_XXX, AM_IMP, AM_IMM, AM_ACC, AM_XXX, AM_ABS, AM_ABS, AM_ABS, AM_XXX,
	AM_REL, AM_YII, AM_XXX, AM_XXX, AM_XXX, AM_ZXI, AM_ZXI, AM_XXX, AM_IMP, AM_AYI, AM_XXX, AM_XXX, AM_XXX, AM_AXI, AM_AXI, AM_XXX,
	AM_IMP, AM_XII, AM_XXX, AM_XXX, AM_XXX, AM_ZPG, AM_ZPG, AM_XXX, AM_IMP, AM_IMM, AM_ACC, AM_XXX, AM_IND, AM_ABS, AM_ABS, AM_XXX,
	AM_REL, AM_YII, AM_XXX, AM_XXX, AM_XXX, AM_ZXI, AM_ZXI, AM_XXX, AM_IMP, AM_AYI, AM_XXX, AM_XXX, AM_XXX, AM_AXI, AM_AXI, AM_XXX,
	AM_XXX, AM_XII, AM_XXX, AM_XXX, AM_ZPG, AM_ZPG, AM_ZPG, AM_XXX, AM_IMP, AM_XXX, AM_IMP, AM_XXX, AM_ABS, AM_ABS, AM_ABS, AM_XXX,
	AM_REL, AM_YII, AM_XXX, AM_XXX, AM_ZXI, AM_ZXI, AM_ZYI, AM_XXX, AM_IMP, AM_AYI, AM_IMP, AM_XXX, AM_XXX, AM_AXI, AM_XXX, AM_XXX,
	AM_IMM, AM_XII, AM_IMM, AM_XXX, AM_ZPG, AM_ZPG, AM_ZPG, AM_XXX, AM_IMP, AM_IMM, AM_IMP, AM_XXX, AM_ABS, AM_ABS, AM_ABS, AM_XXX,
	AM_REL, AM_YII, AM_XXX, AM_XXX, AM_ZXI, AM_ZXI, AM_ZYI, AM_XXX, AM_IMP, AM_AYI, AM_IMP, AM_XXX, AM_AXI, AM_AXI, AM_AYI, AM_XXX,
	AM_IMM, AM_XII, AM_XXX, AM_XXX, AM_ZPG, AM_ZPG, AM_ZPG, AM_XXX, AM_IMP, AM_IMM, AM_IMP, AM_XXX, AM_ABS, AM_ABS, AM_ABS, AM_XXX,
	AM_REL, AM_YII, AM_XXX, AM_XXX, AM_XXX, AM_ZXI, AM_ZXI, AM_XXX, AM_IMP, AM_AYI, AM_XXX, AM_XXX, AM_XXX, AM_AXI, AM_AXI, AM_XXX,
	AM_IMM, AM_XII, AM_XXX, AM_XXX, AM_ZPG, AM_ZPG, AM_ZPG, AM_XXX, AM_IMP, AM_IMM, AM_IMP, AM_XXX, AM_ABS, AM_ABS, AM_ABS, AM_XXX,
	AM_REL, AM_YII, AM_XXX, AM_XXX, AM_XXX, AM_ZXI, AM_ZXI, AM_XXX, AM_IMP, AM_AYI, AM_XXX, AM_XXX, AM_XXX, AM_AXI, AM_AXI, AM_XXX,
};

struct disassembler_context disassembler;

// The assembly of this is not pretty and could be massively improved if it wasn't a toy.
// Compare with the Apple II monitor utility at half the size and *way* more functionality.
void disassemble_next(){
	uint8_t instruction = disassembler.pc[0];
	uint8_t opcode = INSTRUCTION_OPCODES[instruction];
	uint8_t addr_mode = INSTRUCTION_ADDRESS_MODES[instruction];
	uint8_t instruction_length = ADDR_MODE_LENGTHS[addr_mode];
	uint16_t value = disassembler.pc[1] & 255;

	if(instruction_length == 3){
		value |= (((uint16_t) disassembler.pc[2]) << 8);
	}

	snprintf(disassembler.output, sizeof(disassembler.output), ADDR_MODE_FORMATS[addr_mode], OPCODE_MNEMONICS[opcode], value);
	disassembler.pc += instruction_length;
}
