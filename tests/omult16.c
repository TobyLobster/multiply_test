// omult16.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL * 65536UL;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    memory[2] = input & 255UL;
    memory[3] = (input / 256UL) & 255UL;
    memory[4] = (input/65536UL) & 255UL;
    memory[5] = (input/65536UL) / 256UL;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t a = threadContext->machine.state.y;
    uint64_t b = threadContext->machine.state.x;

    if ((threadContext->machine.state.p & 1) != 0) {
        return 65536;
    }
    return a + 256UL*b;
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    uint64_t x = input & 65535UL;
    uint64_t y = input / 65536UL;
    uint64_t e = (x * y);
    *expected = e;

    if ((actual_result == 65536) && (e > 65535)) {
        return -1;
    }
    return actual_result == (e & 65535);
}


// **************************************************************************************
void test_cleanup()
{
}
