// omult4.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL * 65536UL;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    threadContext->machine.state.a = input & 255UL;
    memory[2] = (input / 256UL) & 255UL;
    memory[3] = (input/65536UL) & 255UL;
    memory[4] = (input/65536UL) / 256UL;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t a = memory[7];
    uint64_t b = memory[8];
    uint64_t c = memory[9];
    uint64_t d = memory[10];

    return a + 256UL*b + 65536*c + 256*65536*d;
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    int64_t a = input & ((256UL*65536UL)-1);
    int64_t b = input / (256UL*65536UL);

    if (a >= (128UL*65536UL)) {
        a = a - (128UL*65536UL);
    }
    if (b >= 128) {
        b = b - 128;
    }

    int64_t e = a*b;
    if (e < 0) {
        e = e + 65536UL*65536UL;
    }
    *expected = e;

    return actual_result == e;
}


// **************************************************************************************
void test_cleanup()
{
}
