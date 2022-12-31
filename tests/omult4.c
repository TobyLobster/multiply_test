// omult4.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL * 65536UL;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    memory[2] = input & 255UL;
    memory[3] = (input / 256UL) & 255UL;
    threadContext->machine.state.a = (input/65536UL) & 255UL;
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

    int signA = (a & (128UL*65536UL)) != 0;
    int signB = (b & 128) != 0;

    int64_t e = (a & ~(128UL*65536UL))*(b & 127UL);
    if (signA != signB) {
        e |= (128UL*256UL*65536UL);
    }

    if ((actual_result == 0) && (e == 2147483648UL)) {
        e = 0;
    } else if ((actual_result == 2147483648UL) && (e == 0)) {
        e = 2147483648UL;
    }

    *expected = e;

    // DEBUG!
    if (actual_result != e) {
        printf("A:%llu, B:%llu, abs(A):%llu, abs(B):%llu, e:%llu, actual_result:%llu\n", a, b, (a & ~(128UL*65536UL)), (b & 127UL), e, actual_result);
        printf("signA:%d, signB:%d\n", signA, signB);
    }

    return actual_result == e;
}


// **************************************************************************************
void test_cleanup()
{
}
