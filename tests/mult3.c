// mult3.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL * 65536UL;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    memory[4] = input & 255UL;
    memory[5] = (input / 256UL) & 255UL;
    memory[2] = (input/65536UL) & 255UL;
    memory[3] = (input/65536UL) / 256UL;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t a = memory[6];
    uint64_t b = memory[7];
    uint64_t c = memory[8];
    uint64_t d = memory[9];

    return a + 256UL*(b + 256UL*(c + 256UL*d));
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    uint64_t x = input & 65535UL;
    uint64_t y = input / 65536UL;
    uint64_t e = x * y;
    *expected = e;

    return actual_result == e;
}


// **************************************************************************************
void test_cleanup()
{
}
