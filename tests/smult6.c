// smult6.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 256UL * 65536UL;

const int close_enough = 1;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    threadContext->machine.state.x = input & 255UL;
    threadContext->machine.state.y = (input / 256UL) & 255UL;
    threadContext->machine.state.a = (input/65536UL) & 255UL;
    if (threadContext->machine.state.y >= 128) {
        threadContext->machine.state.p = 0x80;
    } else {
        threadContext->machine.state.p = 0;
    }
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t a = threadContext->machine.state.y;
    uint64_t b = threadContext->machine.state.a;

    return a + 256UL*b;
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    int64_t x = input & 65535UL;
    int64_t y = input / 65536UL;
    if (x>=32768) {
        x -= 65536;
    }
    if (y>=128) {
        y -= 128;
        y = -y;
    }
    int64_t e = ((x * y) / 128) & 65535;
    //printf("x=%lld, y=%lld, e=%lld\n", x,y,e);
    *expected = e;

    if (abs((int) actual_result - (int) e) <= close_enough) {
        return -1;
    }
    return 0;
}


// **************************************************************************************
void test_cleanup()
{
}
