// omult23.c

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

    if (threadContext->options->random > 0) {
        memory[2] = 4;  // test 32 bit x 32 bit
        memory[3] = 4;  // test 32 bit x 32 bit
    } else {
        memory[2] = 2;  // test 16 bit x 16 bit
        memory[3] = 2;  // test 16 bit x 16 bit
    }
    memory[0x14] = x & 255UL;
    memory[0x15] = (x / 256UL) & 255UL;
    memory[0x16] = (x / 65536UL) & 255UL;
    memory[0x17] = (x / 65536UL) / 256UL;
    memory[0x4] = y & 255UL;
    memory[0x5] = (y / 256UL) & 255UL;
    memory[0x6] = (y / 65536UL) & 255UL;
    memory[0x7] = (y / 65536UL) / 256UL;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t a = memory[0x14];
    uint64_t b = memory[0x15];
    uint64_t c = memory[0x16];
    uint64_t d = memory[0x17];
    uint64_t e = memory[0x18];
    uint64_t f = memory[0x19];
    uint64_t g = memory[0x1a];
    uint64_t h = memory[0x1b];

    return a + 256UL*(b + 256UL*(c + 256UL*(d + 256UL*(e + 256UL*(f + 256UL*(g+ 256*h))))));
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
