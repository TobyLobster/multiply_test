
struct disassembler_context {
    char *pc;
    char output[256];
};

extern struct disassembler_context disassembler;
extern void disassemble_next();