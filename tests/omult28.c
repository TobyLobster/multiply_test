// omult28.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL * 65536UL;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    memory[2] = input & 255UL;
    memory[3] = (input / 256UL) & 255UL;
    memory[4] = (input / 65536UL) & 255UL;
    memory[5] = (input / 65536UL) / 256UL;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t a = memory[6];
    uint64_t b = memory[7];
    uint64_t c = threadContext->machine.state.a;

    return a + 256UL*(b + 256UL*c);
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    uint64_t a = input & 255UL;
    uint64_t b = (input / 256UL) & 255UL;
    uint64_t c = (input / 65536UL) & 255UL;
    uint64_t d = (input / 65536UL) / 256UL;

    uint64_t e = a * (b + 256*(c + 256*d));
    e = e / 256UL;
    *expected = e;

    return actual_result == e;
}

// **************************************************************************************
void test_cleanup()
{
}
