// omult22.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL * 65536UL;

// **************************************************************************************
void get_x_y_from_input(thread_context_t* threadContext, uint64_t input, uint64_t *x, uint64_t *y) {
    if (threadContext->options->random > 0) {
        *x = input & ((65536UL * 65536UL)-1);
        *y = input >> 32;
    } else {
        *x = input & 65535;
        *y = (input / 65536) & 65535;
    }
}

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    uint64_t x, y;
    get_x_y_from_input(threadContext, input, &x, &y);

    memory[2] = x & 255UL;
    memory[3] = (x / 256UL) & 255UL;
    memory[4] = (x / 65536UL) & 255UL;
    memory[5] = (x / (256UL * 65536UL)) & 255UL;

    memory[6] = y & 255UL;
    memory[7] = (y / 256UL) & 255UL;
    memory[8] = (y / 65536UL) & 255UL;
    memory[9] = (y / (256UL * 65536UL)) & 255UL;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t a = memory[6];
    uint64_t b = memory[7];
    uint64_t c = memory[8];
    uint64_t d = memory[9];
    uint64_t e = memory[10];
    uint64_t f = memory[11];
    uint64_t g = memory[12];
    uint64_t h = memory[13];

    return a + 256UL*(b + 256UL*(c + 256UL*(d + 256UL*(e + 256UL*(f + 256UL*(g + 256*h))))));
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    uint64_t x, y;
    get_x_y_from_input(threadContext, input, &x, &y);

    uint64_t e = x * y;
    *expected = e;

    return actual_result == e;
}


// **************************************************************************************
void test_cleanup()
{
}
