// multA.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 32768UL * 32768UL;


// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    threadContext->machine.state.p = 1;     // Set carry
    memory[2] = input & 255UL;
    memory[3] = (input / 256UL) & 127UL;
    memory[4] = (input/32768UL) & 255UL;
    memory[5] = ((input/32768UL) / 256UL) & 127UL;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t a = threadContext->machine.state.y;
    uint64_t b = threadContext->machine.state.x;
    uint64_t c = memory[6];
    uint64_t d = memory[7];

    return a + 256UL*(b + 256UL*(c + 256UL*d));
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    zuint8* memory = threadContext->machine.context;

    uint64_t x = input & 32767UL;
    uint64_t y = input / 32768UL;
    uint64_t e = x * y;
    *expected = e;

    return actual_result == e;
}


// **************************************************************************************
void test_cleanup()
{
}
