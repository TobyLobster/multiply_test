// smult4.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL;

static int result[65536UL];
__thread int test_input = 0;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    memory[2] = input & 255UL;
    memory[3] = (input / 256UL) & 255UL;

    test_input = input;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t high = memory[5];
    uint64_t low  = memory[4];

    result[test_input] = 256*high + low;
    return 256*high + low;
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    int64_t a = input & 255UL;
    int64_t b = (input / 256UL) & 255UL;

    if (a >= 128) {
        a = a - 256;
    }
    if (b >= 128) {
        b = b - 256;
    }

    int64_t e = a*b;
    if (e < 0) {
        e = e + 65536;
    }
    *expected = e;

    return actual_result == e;
}

// **************************************************************************************
void test_cleanup()
{
}
