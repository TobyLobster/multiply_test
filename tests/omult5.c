// omult5.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL * 65536UL;


const int close_enough = 1;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    memory[2] = (input & 255UL) & 255;
    memory[3] = (input / 256UL) & 255UL;
    memory[4] = (input/65536UL) & 255UL;
    memory[5] = (input/65536UL) / 256UL;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t low = memory[11];
    uint64_t high = threadContext->machine.state.a;

    return low + 256UL*high;
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    int64_t a = input & 65535UL;
    int64_t b = (input / 65536UL) & 65535UL;

    if (a & 1) {
        a = -a;
    }

    if (b >= 32768) {
        b = b - 65536;
    }

    int64_t e = a*b;

    // Rounding
//    if (e < 0) {
//        e = e - 32768;
//    } else {
//        e = e + 32768;
//   }
    e = e / 65536;
//    if (e < 0) {
//        e = e + 65536UL;
//    }
    int test = actual_result;
    if (actual_result >= 32768) {
        test = actual_result - 65536;
    }

    *expected = e;
    if (abs(test - (int) e) <= close_enough) {
        return -1;
    }
    if (abs((int) actual_result - (int) e) <= close_enough) {
        printf("QUESTIONABLE: A: %lld, B: %lld, e: %lld, test=%d\n", a, b, e, test);
        return -1;
    }
    printf("A: %lld, B: %lld, e: %lld, test=%d\n", a, b, e, test);
    return 0;
//    return actual_result == e;
}


// **************************************************************************************
void test_cleanup()
{
}
